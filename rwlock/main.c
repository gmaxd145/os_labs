//Аналогично №10, но синхронизация доступа должна происходить посредством блокировок чтения-записи (rwlock).

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define WRITERS_N 10
#define STR_LEN 64

char str[STR_LEN];
int counter = 0;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

void *writer(void *arg)
{
    while (1) {
        pthread_rwlock_wrlock(&rwlock);
        sprintf(str, "write counter = %d", counter);
        ++counter;
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

void *reader(void *arg)
{
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("tid: %lx, array: %s\n", pthread_self(), str);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

int main()
{
    pthread_t tids[WRITERS_N + 1];
    pthread_rwlock_init(&rwlock, NULL);
    pthread_create(&tids[0], NULL, writer, NULL);
    for (int i = 1; i <= WRITERS_N; i++) {
        pthread_create(&tids[i], NULL, reader, NULL);
    }
    for (int i = 0; i <= WRITERS_N; i++) {
        pthread_join(tids[i], NULL);
    }
    pthread_rwlock_destroy(&rwlock);
    return 0;
}