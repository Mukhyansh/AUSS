#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<time.h>
#include<string.h>

int main(int argc,char* argv[]){
    int n=1000;
    int t=30;
    
    if(argc>1){
        t=atoi(argv[1]);
    }

    double* A=(double*)malloc((size_t)n*n*sizeof(double));
    double* B=(double*)malloc((size_t)n*n*sizeof(double));
    double* C=(double*)malloc((size_t)n*n*sizeof(double));

    if(!A || !B || !C){
        perror("Failed to allocate memory for A, B or C!\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            A[i*n +j]=1.0;
            B[i*n +j]=2.0;
            C[i*n +j]=0.0;
        }
    }

    time_t start=time(NULL);
    time_t end;

    long long iter=0;
    while(1){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                C[i*n +j]=0.0;
            }
        }

        for(int i=0;i<n;i++){
            for(int k=0;k<n;k++){
                double res1=A[i*n + k];
                for(int j=0;j<n;j++){
                    C[i*n +j]+=res1*B[k*n +j];
                }
            }
        }
        iter++;
        time_t current=time(NULL);
        if(difftime(current,start)>=t) break;
    }
    end=time(NULL);

    printf("Completed!\n");
    printf("Total number of iterations:  %lld.\n",iter);
    printf("Runtime is: %.0f.\n",difftime(end,start));

    return EXIT_SUCCESS;
}