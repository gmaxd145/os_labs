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

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(1);
    }

    exit(0);
}

int main()
{
    signal(SIGINT, signalfunction);

    key_t key;
    key = ftok(".", 'S');

    if ((shmid = shmget(key, BUF_SIZE, IPC_CREAT | IPC_EXCL | 0666)) == -1)
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
        time_t t;
        time(&t);

        sprintf(buffer, "pid: %d time: %s\n", getpid(), asctime(localtime(&t)));
        strcpy(segptr, buffer);
    }
}