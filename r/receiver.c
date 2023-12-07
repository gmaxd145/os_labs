#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define BUF_SIZE 256

int shmid;
char* segptr;

void signalfunction()
{
    if (shmdt(segptr) == -1)
    {
        perror("shmdt");
        exit(1);
    }
    
    exit(0);
}

int main()
{
    signal(SIGINT, signalfunction);

    key_t key;
    if ((key = ftok(".", 'S')) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, BUF_SIZE, 0)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((segptr = shmat(shmid, 0, 0)) == (void*) -1)
    {
        perror("shmat");
        exit(1);
    }

    char buffer[BUF_SIZE];
    while(1)
    {
        sleep(1);

        time_t t;
        time(&t);

        strcpy(buffer, segptr);

        printf("Sender: %s\n", buffer);
        printf("Receiver time: %s pid: %d\n", asctime(localtime(&t)), getpid());
    }
}