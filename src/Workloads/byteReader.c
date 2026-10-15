#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#define SIZE 4*1024*1024

int main(int argc, char* argv[]){
    int t = 60;
    if(argc > 1){
        t = atoi(argv[1]);
    }

    //previously when I was writing this, I used a normal file which was not producing io operations,
    //that was because the file was getting cached so the cpu was just accessing the cache again and again
    //had to force disk actions using O_DIRECT and O_RDONLY
    
    int fd_read = open("../Junk/input_file.bin", O_RDONLY | O_DIRECT); //for direct disk operations
    int fd_write = open("../Junk/output_file.bin", O_WRONLY | O_CREAT | O_DIRECT, 0644);

    if(fd_read < 0 || fd_write < 0){
        return 1;
    }
    
    char* buffer = malloc(SIZE);
    if(!buffer){
        return 1;
    }

    time_t start = time(NULL);

    while(difftime(time(NULL), start) < t){
        read(fd_read, buffer, SIZE);
        write(fd_write, buffer, SIZE);
    }

    free(buffer);
    close(fd_read);
    close(fd_write);
    return EXIT_SUCCESS;
}