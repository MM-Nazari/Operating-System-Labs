#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>

int main() {
    int fds1[2], fds2[2];
    char pipe1[50];
    char pipe2[50];
    char readmessage[50];
    int pid ;
	if(pipe(fds1)==-1){
		exit(EXIT_FAILURE);
	}
	if(pipe(fds2)==-1){
		exit(EXIT_FAILURE);
	}
    pid = fork() ;
    // parent process
    if(pid != 0){
        close(fds1[0]);
        close(fds2[1]);
        gets(pipe1);
        printf(" writing %s to pipe1 in parent process \n" , pipe1);
        write(fds1[1], pipe1, sizeof(pipe1));
        read(fds2[0], readmessage, sizeof(readmessage));
        printf(" reading %s from pipe2 in parent process \n ", readmessage);
    }
    // child process 
    else {
        close(fds1[1]);
        close(fds2[0]);
        read(fds1[0], readmessage, sizeof(readmessage));
        printf(" reading %s form pipe1 in chld process \n", readmessage);
        memcpy(pipe2, readmessage, sizeof(readmessage));
        for(int i=0; i<50; i++){
            if(pipe2[i] >= 65 && pipe2[i] <= 90)
                pipe2[i] = pipe2[i] + 32;
            else if(pipe2[i] >= 65+32 && pipe2[i] <= 90+32)
                pipe2[i] = pipe2[i] - 32;
        }
        printf(" writing %s to pipe2 in child process \n", pipe2);
        write(fds2[1], pipe2, sizeof(pipe2));
    }
    return 0;
}
