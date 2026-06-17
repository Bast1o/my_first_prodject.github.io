#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_SIZE 3
#define WIN_SCORE 21

typedef struct {
    int x, y;
} Paddle;

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    Paddle left;
    Paddle right;
    Ball ball;
    int score_left;
    int score_right;
} Game;

// Инициализация
void nah_game(Game *g) {
    g->left.x = 2;
    g->left.y = HEIGHT / 2 - PADDLE_SIZE / 2;

    g->right.x = WIDTH - 3;
    g->right.y = HEIGHT / 2 - PADDLE_SIZE / 2;
    
    g->ball.x = WIDTH / 2;
    g->ball.y = HEIGHT / 2;
    g->ball.dx = -1;
    g->ball.dy = 1;
    g->score_left = 0;
    g->score_right = 0;
}

// Отрисовка
void draw_game(Game *g) {
    printf("\033[H\033[J");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1) {
                printf("-");
            }
            else if (x == 0 || x == WIDTH - 1) {
                printf("|");
            }
            else if (x == g->left.x && y >= g->left.y && y < g->left.y + PADDLE_SIZE) {
                printf("[");
            }
            else if (x == g->right.x && y >= g->right.y && y < g->right.y + PADDLE_SIZE) {
                printf("]");
            }
            else if (x == g->ball.x && y == g->ball.y) {
                printf("o");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("Left: %d | Right: %d\n", g->score_left, g->score_right);
}

// Движение ракеток
void move_paddle_up(Paddle *p) {
    if (p->y > 1) {
        p->y--;
    }
}

void move_paddle_down(Paddle *p) {
    if (p->y + PADDLE_SIZE < HEIGHT - 1) {
        p->y++;
    }
}

// Движение мяча
void move_ball(Game *g) {
    g->ball.x += g->ball.dx;
    g->ball.y += g->ball.dy;

    if (g->ball.y <= 1 || g->ball.y >= HEIGHT - 2) {
        g->ball.dy = -g->ball.dy;
    }

    if (g->ball.x == g->left.x + 1 &&
        g->ball.y >= g->left.y &&
        g->ball.y < g->left.y + PADDLE_SIZE) {
        g->ball.dx = -g->ball.dx;
    }

    if (g->ball.x == g->right.x - 1 &&
        g->ball.y >= g->right.y &&
        g->ball.y < g->right.y + PADDLE_SIZE) {
        g->ball.dx = -g->ball.dx;
    }

    if (g->ball.x <= 0) {
        g->score_right++;
        g->ball.x = WIDTH / 2;
        g->ball.y = HEIGHT / 2;
        g->ball.dx = 1;
        g->ball.dy = 1;
    }

    if (g->ball.x >= WIDTH - 1) {
        g->score_left++;
        g->ball.x = WIDTH / 2;
        g->ball.y = HEIGHT / 2;
        g->ball.dx = -1;
        g->ball.dy = 1;
    }
}

// Включение неблокирующего режима
void set_terminal_raw() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); 
    t.c_cc[VMIN] = 0;                
    t.c_cc[VTIME] = 0;              
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Возврат обычного режима
void set_terminal_normal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main() {
    Game game;
    nah_game(&game);
    
    set_terminal_raw();          
    
    char input;
    int game_over = 0;
    int frame = 0;
    
    while (!game_over) {
        // Читаем ВСЕ нажатые клавиши (сколько успели нажать с прошлого кадра)
        while (read(STDIN_FILENO, &input, 1) > 0) {
            switch (input) {
                case 'a': case 'A':
                    move_paddle_up(&game.left);
                    break;
                case 'z': case 'Z':
                    move_paddle_down(&game.left);
                    break;
                case 'k': case 'K':
                    move_paddle_up(&game.right);
                    break;
                case 'm': case 'M':
                    move_paddle_down(&game.right);
                    break;
                case ' ':
                    break;
            }
        }
        
        
        if (frame % 2 == 0) {
            move_ball(&game);
        }
        frame++;
        
        // Отрисовка
        draw_game(&game);
        
        // Проверка победы
        if (game.score_left >= WIN_SCORE) {
            draw_game(&game);
            printf("\nLeft player wins!\n");
            game_over = 1;
        }
        else if (game.score_right >= WIN_SCORE) {
            draw_game(&game);
            printf("\nRight player wins!\n");
            game_over = 1;
        }
        
        usleep(20000); 
    }
    
    set_terminal_normal();   
    return 0;
}