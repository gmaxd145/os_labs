#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUF_SIZE 256

int shmid;
char* segptr;
char* semName = "test";
sem_t* sem;

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
    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        exit(1);
    }

    if (sem_unlink(semName) == -1)
    {
        perror("sem_unlink");
        exit(1);
    }

    exit(0);
}

int main()
{
    signal(SIGINT, signalfunction);

    if ((sem = sem_open(semName, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    key_t key;
    if ((key = ftok("shmem", 6)) == -1)
    {
        perror("ftok");
        exit(1);
    }

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
        sleep(3);

        time_t t;
        time(&t);

        sprintf(buffer, "pid: %d time: %s\n", getpid(), asctime(localtime(&t)));

        sem_wait(sem);
        strcpy(segptr, buffer);
        sem_post(sem);
    }
}