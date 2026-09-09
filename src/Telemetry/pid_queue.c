#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<stdarg.h>
#include<string.h>
#include<sys/wait.h>
#include<limits.h>

typedef struct pid_queue{
    pid_queue* rear;
    pid_queue* front;
    int pid;
}pid_queue;

int main(void){
        
    return 0;
}