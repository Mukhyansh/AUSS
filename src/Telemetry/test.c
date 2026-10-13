#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define MAX_OPS 20

int main(void){
    int x=MAX_OPS;
    while(x--) system("./collector");
    return 0;
}