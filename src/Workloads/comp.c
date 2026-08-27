#include "Compressor/comp.h"
#include<time.h>
#include<unistd.h>

int main(int argc,char* argv[]){
    time_t start=time(NULL);
    if(argc<2){
        perror("Not enough arguments!");
        return 1;
    }
    int n=atoi(argv[1]);
    while(time(NULL)-start!=n){
        printf("Compressing file...\n");
        compressFile();
        printf("File Compressed!\n");
        printf("Decompressing File...\n");
        decompressFile();
        printf("File decompressed!\n");
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}