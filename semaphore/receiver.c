#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#define BUF_SIZE 256


int main()
{
    char* semName = "test";
    sem_t* sem;
    if ((sem = sem_open(semName, 0)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    key_t key;
    if ((key = ftok("tz", 5)) == -1)
    {
        perror("ftok");
        exit(1);
    }

    int shmid;
    if ((shmid = shmget(key, BUF_SIZE, 0)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    char* segptr;
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

        sem_wait(sem);
        strcpy(buffer, segptr);
        sem_post(sem);

        printf("Sender: %s\n", buffer);
        printf("Receiver time: %s pid: %d\n", asctime(localtime(&t)), getpid());
    }
}