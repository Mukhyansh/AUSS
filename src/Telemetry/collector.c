#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>
#include<stdarg.h>

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
    if(!fp) return 0;
    
    int count=0;
    int n;
    while(fscanf(fp,"%d",&n)){
        count++;
        if(count==14){
            //read utime
        }
        if(count==15){
            //read stime
        }
    }
}

int main(int argc,char* argv[]){
    
    return EXIT_SUCCESS;
}
