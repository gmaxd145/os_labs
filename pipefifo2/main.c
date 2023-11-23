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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define FIFO_NAME "myfifo"

int main() {
    int fd;
    pid_t child_pid;
    char buf[128];

    mkfifo(FIFO_NAME, 0666);
    
    child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) 
    {
        fd = open(FIFO_NAME, O_RDONLY);

        if (fd < 0) 
        {
            perror("open");
            exit(1);
        }
        read(fd, buf, sizeof(buf));
        close(fd);
        puts(buf);

        sleep(5);
        long int s_time = time(NULL);
        struct tm* m_time  = localtime (&s_time);
        strcpy(buf, asctime(m_time));
        printf("Current time: %s", buf);

        exit(0);
    }
    else 
    {
        fd = open(FIFO_NAME, O_WRONLY);
        if (fd < 0) 
        {
            perror("open");
            exit(1);
        }
        long int s_time = time(NULL);
        struct tm* m_time  = localtime (&s_time);
        snprintf(buf, sizeof(buf), "PID: %d Msg time: %s", getpid(), asctime(m_time));
        write(fd, buf, strlen(buf));
        close(fd);

        int wstatus;
        wait(&wstatus);
        printf("Child exit status: %d\n", WEXITSTATUS(wstatus));
    }
    unlink(FIFO_NAME);

    return 0;
}
