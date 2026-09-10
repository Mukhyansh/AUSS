#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>
#include<stdarg.h>
#include<limits.h>

#define TIME_CUT 100
#define MAX_PIDS 10 //to be changed later obv
#define MAX_SAMPLES 500//Skeptical about its value

#define OUTPUT_FILE "record.csv"
#define INPUT_FILE "../Workloads/output_file.txt"

typedef struct workload_cpu{
    long utime;
    long stime;
    int nice
}workload_cpu;

typedef struct workload_io{
    unsigned int read_ops;
    unsigned int write_ops;
    unsigned int byte_read;
    unsigned int write_read;
}workload_io;

typedef struct sample{
    workload_cpu stat;
    workload_io io;
}sample;

typedef struct workload{
    char name[400];
    char type[40];
    int pid;
    time_t start_t;
    sample* samples;
    int no_samples;
}workload;

typedef struct result{
    char name[400];
    char type[40];
    int pid;
    long elapsed_t;
    double cpu_p;
    double io_throughput;
    double io_ops_per_sec;
    int samples_collected;
}result;

int read_info_stat(int pid,workload_cpu* proc);
int read_info_io(int pid,workload_io* proc);
void read_info_uptime();
void infoLog();

int read_info_io(int pid,workload_io* proc){
    char path[256];
    FILE* fp=fopen(path,"r");
    if(!fp) return 0;
    char line[256];

    // puts(path[0]);
    snprintf(path,sizeof(path),"/proc/%d/io",pid);

    proc->byte_read=0;
    proc->write_read=0;
    proc->read_ops=0;
    proc->write_ops=0;

    while(fgets(line,sizeof(line),fp)){
        if(strncmp(line,"read_bytes:", 11)==0){
            sscanf(line,"read_bytes: %d",&proc->byte_read);
        }
        else if(strncmp(line,"write_bytes:", 12)==0){
            sscanf(line,"write_bytes: %ld",&proc->write_read);
        }
        else if(strncmp(line,"read_ops:", 9)==0){
            sscanf(line,"read_ops: %ld",&proc->read_ops);
        }
        else if(strncmp(line,"write_ops:",10)==0){
            sscanf(line,"write_ops: %ld",&proc->write_ops);
        }
    }
    fclose(fp);
    return 1;
}

int isAlive(int pid){
    return kill(pid,0)==0;
}

void collect_telemetry(workload* proc){
    //display info of the process in the future

    time_t start_time=time(NULL);
    int maxm=120;

    proc->samples=malloc(sizeof(sample)*MAX_SAMPLES);
}

int read_info_stat(int pid,workload_cpu* proc){
    char path[MAX_PIDS*20];
    snprintf(path,sizeof(path),"/proc/%d/stat",pid);
    FILE* fp=fopen(path,"r");
    if(!fp)  return 0;
    char buffer[4096];

    
    long long count=0;
    int n;
    // puts(path[0]);
    if(fgets(buffer,sizeof(buffer),fp)==NULL){
        fclose(fp);
        return 0;
    }
    // for(int i=0;buffer[i]!='\0';i++){
    //     printf("%c",buffer[i]);
    // }
    fclose(fp);
    if (sscanf(buffer,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %*d %*d %d",
           &proc->utime,&proc->stime,&proc->nice)!=3){
        return 0;
    }
    // fprintf(out, "%ld %ld\n", (proc->utime), (proc->stime));
    // printf("%c",buffer[0]);

        
    return 1;
}
