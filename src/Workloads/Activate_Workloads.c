#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

#define TIME_SLICE "2"

int main(void){
    fprintf(stderr,"5 instances of a workload has started!\n");
    for(int i=0;i<5;i++){
        if(access("./byteReader",X_OK)!=0){
            fprintf(stderr,"Executable named 'byteReader' not found!\n");
            return 1;
        }
        time_t start=time(NULL);
        fprintf(stderr,"Byte Reader (I/O) has started!\n");
        pid_t pid=fork();
        if(pid<0){
            fprintf(stderr,"fork failed!\n");
            return 1;
        }
        else if(pid==0){
            execvp("./byteReader",(char*[]){"./byteReader",TIME_SLICE,NULL});
        }
        else waitpid(pid,NULL,0);
        time_t end=time(NULL);
        double sec=difftime(end,start);
        fprintf(stderr,"%.0f seconds.\n",sec);
        fprintf(stderr,"Byte Reader has ended\n");
        printf("pid: %d\n",pid);
        fflush(stdout);
    }
    fprintf(stderr,"5 instances of a workload has ended!\n");
    return 0;
}