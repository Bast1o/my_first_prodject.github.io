#include <stdio.h>

int delit(int a ,int b){
    int c = 0;
    while(a>=b){
        a -= b;
        c++;}
return c;
}



int del(int a ,int b){
    while(a>=b) a -= b;
    return a==0; 
}



int main(){
    char z;
    int x;
   if(scanf("%d%c",&x,&z) !=2 || z != '\n') {
        printf("n/a\n");
       return 0;
    }
    
    
    int max_del = -1;
    int temp = x;

    


    while(del(temp,2)){
        max_del = 2;
        temp = delit(temp,2);
    }

    for(int d = 3;d * d<=temp;d+=2){
        while(del(temp,d)){
        max_del = d;
        temp = delit(temp,d);
    }}

    if(temp > 2) max_del = temp;{
printf("%d\n",max_del);

}
return 0;
}


