// Лабораторная работа №3
// Реализовать программу, демонстрирующую работу системного вызова fork().
// Требования:
//     1. Вызов fork() и корректная обработка всех возвращаемых им значений.
//     2. Использование своего обработчика atexit() или on_exit().
//     3. Переопределение обработчика сигнала SIGINT при помощи системного вызова signal() и обработчика сигнала SIGTERM при помощи вызова sigaction(). (вывод описания полученного сигнала).
// Проверять работоспособность программы на сервере кафедры!!!

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>

void exitHandler() 
{
    printf("Exit handler. Curent: %d\n", getpid());
}

void sigintHandler() 
{
    printf("SIGINT handler. Curent: %d\n", getpid());

}

void sigtermHandler() 
{
    printf("SIGTERM handler. Curent: %d\n", getpid());
}

int main()
{
    atexit(exitHandler);    
    pid_t pid;

    signal(SIGINT, sigintHandler);


    struct sigaction act;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);

    switch (pid = fork())
    {
        case -1:
            puts("Child process is not created");\
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
        {
            puts("Child process is created");
            sleep(5);
            exit(EXIT_SUCCESS);
        }
        default:
            printf("Parent: %d. Child: %jd\n", getpid(), (intmax_t) pid);
            puts("Parent process. Waiting for a child process to finish");
            int wstatus;
            wait(&wstatus);
            printf("Child exit status: %d\n",WEXITSTATUS(wstatus));
    }
    return 0;
}