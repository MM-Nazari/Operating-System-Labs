#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    int fd;
    char * myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);

    fd = open(myfifo, O_WRONLY);
    write(fd, "This is First Process", sizeof("This is First Process"));
    close(fd);

    char buf[1024];
    fd = open(myfifo, O_RDONLY);
    read(fd, buf, 1024);
    printf("Received: %s\n", buf);

    return 0;
}