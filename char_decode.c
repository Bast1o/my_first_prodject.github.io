#include <stdio.h>
int fib(int n){
    if (n==0)
    return 0;
    if (n==1)
    return 1;
    return fib(n-1) + fib(n-2);
}
int main(){
    char z;
    int x;
    if(scanf("%d%c",&x,&z) !=2 || z != '\n') {
        printf("n/a\n");
       return 0;}
    printf("%d\n",fib(x));
    return 0;

}