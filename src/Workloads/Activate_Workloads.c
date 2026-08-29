#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

#define TIME_SLICE "20"
// #define OFILE "output_file.txt"

int main(void){
    // FILE* fp=fopen("/src/Workloads/output_file.txt","a");
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
        else{
            FILE* fp=fopen("output_file.txt","a");
            if(fp){
                fprintf(fp,"pid: %d\n",pid);
                fprintf(stderr,"Wrote the pid to file!\n");
                fclose(fp);
            }
            else{
                fprintf(stderr,"Error opening output_file.txt!\n");
            }
            waitpid(pid,NULL,0);
        } 
        time_t end=time(NULL);
        double sec=difftime(end,start);
        fprintf(stderr,"%.0f seconds.\n",sec);
        fprintf(stderr,"Byte Reader has ended\n");
    }
    puts("\n\n\nAnother workload is starting!\n\n");
    sleep(1);

    /*
        MATRIX_MULTIPLIER
    */

    for(int i=0;i<5;i++){
        if(access("./matrixMultiplication",X_OK)!=0){
            fprintf(stderr,"Executable named 'matrixMultiplication' not found!\n");
            return 1;
        }
        time_t start=time(NULL);
        fprintf(stderr,"matrixMultiplication (I/O) has started!\n");
        pid_t pid=fork();
        if(pid<0){
            fprintf(stderr,"fork failed!\n");
            return 1;
        }
        else if(pid==0){
            execvp("./matrixMultiplication",(char*[]){"./matrixMultiplication",TIME_SLICE,NULL});
        }
        else{
            FILE* fp=fopen("output_file.txt","a");
            if(fp){
                fprintf(fp,"pid: %d\n",pid);
                fprintf(stderr,"Wrote the pid to file!\n");
                fclose(fp);
            }
            else{
                fprintf(stderr,"Error opening output_file.txt!\n");
            }
            waitpid(pid,NULL,0);
        } 
        time_t end=time(NULL);
        double sec=difftime(end,start);
        fprintf(stderr,"%.0f seconds.\n",sec);
        fprintf(stderr,"matrixMultiplication has ended\n");
    }

    puts("\n\n\nAnother workload is starting!\n\n");
    sleep(1);
    /*
        COMPRESSOR
    */

    for(int i=0;i<5;i++){
        if(access("./compressor",X_OK)!=0){
            fprintf(stderr,"Executable named 'compressor' not found!\n");
            return 1;
        }
        time_t start=time(NULL);
        fprintf(stderr,"Compressor has started!\n");
        pid_t pid=fork();
        if(pid<0){
            fprintf(stderr,"fork failed!\n");
            return 1;
        }
        else if(pid==0){
            execvp("./compressor",(char*[]){"./compressor",TIME_SLICE,NULL});
        }
        else{
            FILE* fp=fopen("output_file.txt","a");
            if(fp){
                fprintf(fp,"pid: %d\n",pid);
                fprintf(stderr,"Wrote the pid to file!\n");
                fclose(fp);
            }
            else{
                fprintf(stderr,"Error opening output_file.txt!\n");
            }
            waitpid(pid,NULL,0);
        } 
        time_t end=time(NULL);
        double sec=difftime(end,start);
        fprintf(stderr,"%.0f seconds.\n",sec);
        fprintf(stderr,"compressor has ended\n");
    }
    rm("rm output_file.txt");
    return 0;
}