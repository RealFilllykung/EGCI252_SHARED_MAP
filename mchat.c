#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define FILE_LENGTH 2000

static void sig_end(){
    for(int i = 0; i < 500; i++);
    exit(EXIT_SUCCESS);
}

struct mm_st{
    int written_0;
    char data_0[BUFSIZ];
    int written_1;
    char data_1[BUFSIZ];
}

int main(int argc, char const *argv[]){
    int fd, n = 0;
    pid_t child;
    void* file_memory;
    struct mm_st* mm_area;
    char* buffer = NULL;
    
    if (argc > 2){
        fprintf(stderr,"Usage: %s <[1, 2]>\n", *argv);
        exit(EXIT_FAILURE);
    }
    
    fd = open("chat_log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek(fd, FILE_LENGTH + 1, SEEK_SET);
    write(fd,"",1);
    lseek(fd,0,SEEK_SET);
    
    argv++;
    if (strcmp(*argv, "1") == 0){
        //fork() child process
        child = fork();
    }
    else if(strcmp(*argv, "2") == 0){
        //fork() child process
        child = fork();
    }
    
    if (child) kill(child, SIGTERM);
    else kill(getppid(), SIGTERM);
    
    exit(EXIT_SUCCESS);
}
