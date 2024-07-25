#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    int count;
    int counter2;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex2;
} Data;
int shmid;

void reader() {
    Data* data = (Data *)shmat(shmid, NULL, 0);
    while (true) {
        pthread_mutex_lock(&(data->mutex));
        (data->counter2)++;
        if (data->counter2 == 1) {
            pthread_mutex_lock(&(data->mutex2));
        }
        pthread_mutex_unlock(&(data->mutex));
        printf("PID(R): %d\tcount: %d\n", getpid(), data->count);
        if (data->count >= 2) {
            pthread_mutex_lock(&(data->mutex));
            (data->counter2)--;
            if (data->counter2 == 0) {
                pthread_mutex_unlock(&(data->mutex2));
            }
            pthread_mutex_unlock(&(data->mutex));
            sleep(1);
            break;
        }
        pthread_mutex_lock(&(data->mutex));
        (data->counter2)--;
        if (data->counter2 == 0) {
            pthread_mutex_unlock(&(data->mutex2));
        }
        pthread_mutex_unlock(&(data->mutex));
        sleep(1);
    }
    shmdt(data);
}

void writer() {
    Data* data = (Data *)shmat(shmid, NULL, 0);
    while (true) {
        pthread_mutex_lock(&(data->mutex2));
        data->count++;
        printf("PID(W): %d\tcount: %d\n", getpid(), data->count);
        if (data->count == 2) {
            pthread_mutex_unlock(&(data->mutex2));
            sleep(1);
            break;
        }
        pthread_mutex_unlock(&(data->mutex2));
        sleep(1);
    }
    shmdt(data);
}

int main(int argc, char const *argv[]){

    pthread_mutexattr_t attr;

    shmid = shmget(IPC_PRIVATE, sizeof(Data), IPC_CREAT | 0666);
    Data* d;
    d = (Data *)shmat(shmid, NULL, 0);

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(d->mutex), &attr);
    pthread_mutex_init(&(d->mutex2), &attr);
    shmdt(d);
   
    if (fork() == 0) { 
        writer();
    }else{
        int pid = fork();
        if (pid == 0){
            reader();
        }else{
            int pid = fork();
            if (pid == 0){
                reader();
            }else{
                wait(NULL);
            }
            wait(NULL);
        }
        wait(NULL);
    }
    return 0;
}