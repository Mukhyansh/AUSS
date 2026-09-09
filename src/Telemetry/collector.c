#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>
#include<stdarg.h>
#include<limits.h>

#define TIME_CUT 100
#define MAX_PIDS 10 //to be changed later obv

#define OUTPUT_FILE "record.csv"
#define INPUT_FILE "../Workloads/output_file.txt"

typedef struct workload_cpu{
    long double utime;
    long double stime;
    float cpu_p;
    double elapsed_t;
    int start_t;
}workload_cpu;

typedef struct workload_io{
    unsigned int io_throughput;
    
}workload_io;

int read_info_stat(workload_cpu* proc);
int read_info_io(workload_cpu* proc);
void read_info_uptime();
void infoLog();

int read_info_io(workload_cpu* proc){
    
}

int read_info_stat(workload_cpu* proc){
    FILE* fp=fopen(INPUT_FILE,"r");
    FILE* out=fopen("test.txt","w+");
    if(!fp || !out) {
        return 0;
    }
    
    char str[20];
    char ch;
    long long idx=0;
    int temp;
    int* pids=(int*)malloc(sizeof(int)*MAX_PIDS);
    while(fscanf(fp,"%19s%c %d",str,&ch,&temp)==3){
        pids[idx++]=temp;
    }
    for(int i=0;i<idx;i++){
        int pid=pids[i];
        char path[MAX_PIDS*20][MAX_PIDS*20];
        char buffer[4096];
    
        snprintf(path[i],sizeof(path[i]),"/proc/%d/stat",pid);
        
        long long count=0;
        int n;
        int utime=0,stime=0;

        FILE *f=fopen(path[i],"r");
        puts(path[i]);

        if(f==NULL){
            perror("fopen");
            return 0;
        }

        if(fgets(buffer,sizeof(buffer),f)==NULL){
            fclose(f);
            return 0;
        }

        // for(int i=0;buffer[i]!='\0';i++){
        //     printf("%c",buffer[i]);
        // }

        fclose(f);

        if (sscanf(buffer,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld",
               &proc->utime,&proc->stime)!=2){
            return 0;
        }
        fprintf(out, "%ld %ld\n", (proc->utime), (proc->stime));
        // printf("%s",buffer[0]);

        }
    return 1;
}

int main(int argc,char* argv[]){
    const char* ch;
    workload_cpu proc;
    proc.utime=0;
    proc.stime=0;
    int x=read_info_stat(&proc);
    return EXIT_SUCCESS;
}