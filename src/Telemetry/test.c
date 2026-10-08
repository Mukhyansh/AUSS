#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void){
    int x=10;
    while(x--) system("./collector");
    return 0;
}