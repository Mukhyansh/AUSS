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
#define JIFFY_TO_MS 10;

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
    if(!proc->samples){
        perror("Memory Allocation Failed!\n");
        return;
    }
    proc->no_samples=0;

    while(isAlive(proc->pid)){
        if(time(NULL)-start_time> maxm){
            perror("timeout collecting the Telemetry!\n");
            return;
        }
        if(proc->no_samples >= MAX_SAMPLES){
            perror("Sample overload!\n");
            return;
        }

        sample* s=&proc->samples[proc->no_samples];

        if(read_info_stat(proc->pid,&s->stat) && read_info_io(proc->pid,&s->io)){
            proc->no_samples++;
        }
        
        usleep(TIME_CUT * 1000);   
    }

    if(proc->no_samples==0){
        perror("NO samples collected!\n");
        free(proc->samples);
        proc->samples=NULL;
        return;
    }

    printf("Collected %d samples for PID: %d",proc->no_samples,proc->pid);
}

void calculate_result(workload* proc,result* res){
    if(!proc->samples || proc->no_samples < 2){
        perror("Cannot! Not enough samples!\n");
        return;
    }
    sample* first=&proc->samples[0];
    sample* last=&proc->samples[proc->no_samples-1];


    //CPU time
    long elapsed_ms=(long)(proc->no_samples-1) * TIME_CUT;
    if(elapsed_ms<=0) elapsed_ms=1;

    //cpu time is in jiffies(small/blink unit of time)
    long cpu_jiffies=(last->stat.utime - first->stat.utime)+ (last->stat.stime - first->stat.stime);
    
    long cpu_time_ms=cpu_jiffies*JIFFY_TO_MS;

    double cpu_percent=(double)cpu_time_ms/elapsed_ms *100.0;

    //IO DATA

    long total_read=last->io.byte_read-first->io.byte_read;
    long toral_write=last->io.write_read-first->io.write_read;
    long total_io_bytes=total_read+toral_write;

    double io_throughput_mb=(double)total_io_bytes/elapsed_ms;
    io_throughput_mb/=1000000;

    long io_ops=(last->io.read_ops-first->io.read_ops)+(last->io.write_ops-first->io.write_ops);
    double io_ops_per_sec=io_ops/(elapsed_ms/1000);

    res->cpu_p=cpu_percent;
    res->pid=proc->pid;
    res->elapsed_t=elapsed_ms;
    strncpy(res->name,proc->name,sizeof(res->name)-1);
    strncpy(res->type,proc->type,sizeof(res->type)-1);
    res->io_ops_per_sec=io_ops_per_sec;
    res->samples_collected=proc->no_samples;
    res->io_throughput=io_ops;
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
