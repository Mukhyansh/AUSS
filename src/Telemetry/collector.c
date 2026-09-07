#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>
#include<stdarg.h>

#define TIME_CUT 100
#define MAX_PIDS 100000; //to be changed later obv

#define OUTPUT_FILE "record.csv"
#define INPUT_FILE "../Workloads/output_file.txt"

typedef struct workload_cpu{
    unsigned int utime;
    unsigned int stime;
    float cpu_p;
    double elapsed_t;
    int start_t;
}workload_cpu;

typedef struct workload_io{
    unsigned int io_throughput;
    
}workload_io;

int read_info_stat(const char* ch);
void read_info_io();
void read_info_uptime();
void infoLog();

int read_info_stat(const char* ch){
    FILE* fp=fopen(INPUT_FILE,"r");
    FILE* out=fopen("test.txt","w+");
    if(!fp) return 0;
    
    int count=0;
    char* str;
    char ch;
    int n;
    int* pids=(int*)malloc(sizeof(MAX_PIDs));
    while(fscanf(fp,"%s %c %d",&str,&ch,&n)==3){
        count++;
        
    }
    return 1;
}

int main(int argc,char* argv[]){
    const char* ch;
    int x=read_info_stat(ch);
    return EXIT_SUCCESS;
}
