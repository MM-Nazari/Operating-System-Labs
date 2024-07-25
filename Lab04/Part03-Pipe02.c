#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){

    int fd;
    char * myfifo = "/tmp/myfifo";
    char buf[1024];

    fd = open(myfifo, O_RDONLY);
    read(fd, buf, 1024);
    printf("Received: %s\n", buf);
    for (int i = 0; i < 1024; ++i){
        if(buf[i] >96 && buf[i]<123){
            // a - z
            buf[i] = buf[i]-32;
        }else if (buf[i] >64 && buf[i]<91){
            //A - Z
            buf[i] = buf[i]+32;
        }
    }
    close(fd);
    printf("new String : %s\n", buf);

    fd = open(myfifo, O_WRONLY);
    write(fd, buf, 1024);
    close(fd);

    return 0;
}