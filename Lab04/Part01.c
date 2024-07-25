#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int main (int argc, char *argv[]){

    struct shmid_ds shmbuffer;

    int segment_id = shmget(IPC_PRIVATE, 1024, S_IRUSR | S_IWUSR);

    char* writer = (char*) shmat (segment_id, NULL, 0);

    shmctl(segment_id, IPC_STAT, &shmbuffer);

    // add to memory
    sprintf(writer, "%s", argv[1]);
    printf("Write : %s\n\n", argv[1]);

    int pid = fork();

    if (pid == 0){

        // child process read from memory
        char *reader = (char*) shmat(segment_id,NULL,0);
        printf("Read : %s\n",reader);
        shmdt(reader);

    }else{
        shmdt(writer);
    }

    return 0;
}