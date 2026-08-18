#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
#include<malloc.h>

#define SIZE 4*1024*1024

int main(int argc,char* argv[]){
    int t=60;
    if(argc>1){
        t=atoi(argv[1]);
    }

    FILE* file=fopen("../Junk/input_file.bin","rb");

    if(!file){
        perror("Couldn't open file!\n");
        return 1;
    }
    char* buffer=malloc(SIZE);

    if(!buffer){
        perror("Memory allocation failed!\n");
        return 1;
    }

    printf("Durations is: %d seconds.\n",t);

    time_t start=time(NULL);

    unsigned long long total_bytes=0;
    unsigned long long total_reads=0;

    while(difftime(time(NULL),start)<t){
        int bytes_read=fread(buffer,1,SIZE,file);

        if(bytes_read>0){
            total_bytes+=bytes_read;
            total_reads++;
        }

        if(bytes_read<SIZE){
            clearerr(file);

            if(fseek(file,0,SEEK_SET)!=0){
                perror("fseek failed!");
                break;
            }
        }
    }

    double looped_time=difftime(time(NULL),start);

    printf("Completed!\n");
    printf("Runtime: %.0f seconds!\n",looped_time);
    printf("Read operations: %d.\n",total_reads);
    printf("Total read: %.2f MB.\n",((double)total_bytes/(1024*1024))/looped_time);

    free(buffer);
    fclose(file);
    return EXIT_SUCCESS;
}