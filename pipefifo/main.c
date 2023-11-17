// Лабораторная работа №6

// 1. Создать pipe. Создать дочерний процесс. Передать строчку через pipe
// из родительского в дочерний процесс с текущим временем и pid передающего
// процесса. Дочерний процесс должен вывести на экран его текущее время и
// полученную через pipe строчку. Время из родительского и дочернего должно
// различаться как минимум на 5 секунд.
// 2. Аналогично, но использовать fifo

// Проверять работоспособность программы на сервере кафедры!!!

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    char buf;
    pid_t cpid;

    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) 
    {
        // Дочерний
        sleep(5);
        long int s_time;
        struct tm *m_time;
        s_time = time (NULL);
        m_time  = localtime (&s_time);
        char buffer[100] = {0};
        strcpy(buffer, asctime(m_time));
        printf("Current time: %s\n", buffer);

        close(pipefd[1]);   
        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } 
    else 
    {   
        // Родитель
        close(pipefd[0]);        
        long int s_time;
        struct tm *m_time;
        s_time = time (NULL);
        m_time  = localtime (&s_time);
        char buffer[100] = {0};
        sprintf(buffer, "%d; msg time: %s\n", getpid(), asctime(m_time));
        // printf("%ld\n", sizeof(buffer));
        // puts(buffer);
        write(pipefd[1], buffer, sizeof(buffer));
        close(pipefd[1]);
        int wstatus;
        wait(&wstatus);
        printf("Child exit status: %d\n",WEXITSTATUS(wstatus));
    }
    return 0;
}