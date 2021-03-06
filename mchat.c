#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define FILE_LENGTH 2000

static void sig_end(){
    for(int i = 0; i < 500; i++);
    exit(EXIT_SUCCESS);
}

//Create structure that will contain the chat buffer
struct mm_st{
    int written_0;
    char data_0[BUFSIZ];
    int written_1;
    char data_1[BUFSIZ];
};

int main(int argc, char const *argv[]){
    int fd, n = 0;
    pid_t child;
    void* file_memory;
    struct mm_st* mm_area;
    char* buffer = "";
    int running = 1;
    
    //If the user don't input enough argument
    if (argc > 2){
        fprintf(stderr,"Usage: %s <[1, 2]>\n", *argv);
        exit(EXIT_FAILURE);
    }
    
    //Open that chat log file, if it not exist, = create new one
    fd = open("chat_log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    //This just for checking that the file has enough space to contain the log or not
    lseek(fd, FILE_LENGTH + 1, SEEK_SET);
    write(fd,"",1);
    lseek(fd,0,SEEK_SET);
    
    argv++;
    
    //If the user 1 login
    if (strcmp(*argv, "1") == 0){
        //fork() child process
        child = fork();
        
        //If it is parent
        if (child){
            //Create the map
            file_memory = mmap(0,FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            
            //Create the shared map
            mm_area = (struct mm_st*) file_memory;
            buffer = mm_area -> data_0;
            
            //While the program is running
            while(running){
                while(mm_area -> written_0){
                    usleep(100);
                }
                
                //Receive the input from user
                fgets(buffer,BUFSIZ,stdin);
                mm_area -> written_0 = 1;
                usleep(200);
                
                //If the user type end chat
                if(strncmp(buffer,"end chat",8) == 0){
                    kill(child,SIGTERM);
                    running = 0;
                    munmap(file_memory, FILE_LENGTH);
                    exit(EXIT_SUCCESS);
                }
                
                //Print the chat text into the file
                sprintf((char*)file_memory,"%s\n",buffer);
            }
            return 0;
        }
        else{
            //Create the map
            file_memory = mmap(0, FILE_LENGTH, PROT_WRITE | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            
            //Create the shared map
            mm_area = (struct mm_st*) file_memory;
            buffer = mm_area -> data_1;
            mm_area -> written_1 = 0;
            while(running){
                if(mm_area -> written_1){
                    //Print the buffer receive from opposite parent
                    printf("%s", buffer);
                    mm_area -> written_1 = 0;
                    if (strncmp(buffer,"end chat",8) == 0){
                        kill(getppid(),SIGINT);
                        running = 0;
                        exit(EXIT_SUCCESS);
                    }
                    strcpy(buffer,"");
                }
            }
            munmap(file_memory, FILE_LENGTH);
            return 0;
        }
    }
    //If the user 2 login
    else if(strcmp(*argv, "2") == 0){
        //fork() child process
        child = fork();
        
        //If it is parent
        if (child){
            file_memory = mmap(0,FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            
            //Create the shared map
            mm_area = (struct mm_st*) file_memory;
            buffer = mm_area -> data_1;
            
            //While the program is running
            while(running){
                while(mm_area -> written_1){
                    usleep(100);
                }
                
                //Receive the input from user
                fgets(buffer,BUFSIZ,stdin);
                mm_area -> written_1 = 1;
                usleep(200);
                
                //If the user type end chat
                if(strncmp(buffer,"end chat",8) == 0){
                    kill(child,SIGINT);
                    running = 0;
                    exit(EXIT_SUCCESS);
                }
                
                //Print the chat text into the file
                sprintf((char*)file_memory,"%s\n",buffer);
            }
        }
        else{
            //Create the map
            file_memory = mmap(0, FILE_LENGTH, PROT_WRITE | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            
            //Create the shared map
            mm_area = (struct mm_st*) file_memory;
            buffer = mm_area -> data_0;
            mm_area -> written_0 = 0;
            while(running){
                if(mm_area -> written_0){
                    //Print the buffer receive from opposite parent
                    printf("%s", buffer);
                    mm_area -> written_0 = 0;
                    if (strncmp(buffer,"end chat",8) == 0){
                        kill(child,SIGINT);
                        running = 0;
                        munmap(file_memory, FILE_LENGTH);
                        exit(EXIT_SUCCESS);
                    }
                    strcpy(buffer,"");
                }
            }
            return 0;
        }
    }
    
    if (child) kill(child, SIGTERM);
    else kill(getppid(), SIGTERM);
    
    exit(EXIT_SUCCESS);
}
