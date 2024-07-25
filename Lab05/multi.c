#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void printHistogram(int hist[], int decrease){
    if (decrease <= 0){
        decrease = 1;
    }
    for (int i = 0; i < 25; i++){
        for (int j = 0; j < hist[i]/decrease; j++){
            printf("*");
        }
        printf("[%d]\n",i);
    }
}
typedef struct{
    int data[25];
} Hist;
int id;
Hist *hist;

int main(int argc, char const *argv[]){
    int iterations ;
    int decrease ;
    int c = 0;
    time_t t;
    printf("iterations :");
    scanf("%d",&iterations);
    printf("decrease number :");
    scanf("%d",&decrease);

    srand(time(NULL));

    if ((id = shmget(IPC_PRIVATE, sizeof(Hist), IPC_CREAT | 0666)) < 0){
        exit(-1);
    }
    t = clock();
    for (int i = 0; i < 5; i++){
        int pid = fork();
        if (pid == 0){
            hist = (Hist *)shmat(id, NULL, 0);
            int counter;
            for (int i = 0; i < iterations / 5; i++){
                counter = 0;
                for (int j = 0; j < 12; j++){
                    int random = rand() % 101;
                    if (random >= 49){
                        counter+=1;
                    }
                    else{
                        counter-=1;
                    }
                }
                hist->data[counter + 12]+=1;
            }
            exit(0);
        }else if(pid > 0){
            if (c == 0){
                if ((hist = (Hist *)shmat(id, NULL, 0)) == (Hist *)-1){
                    exit(-1);
                }
                c = 1;
            }
            wait(NULL);
        }else{
            exit(-1);
        }
    }
    printHistogram(hist->data,decrease);
    t = clock() - t;
    printf("Time : %f",((double)t)/CLOCKS_PER_SEC);
    return 0;
}