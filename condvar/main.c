//Создать 10 читающих потоков и 1 пишущий. Потокам должен быть
//доступен один и тот же массив символов. Пишущий поток записывает в общий
//массив номер записи (монотонно возрастающий счетчик), а читающие потоки
//выводят на экран свой tid и текущее состояние общего массива. Доступ должен
//быть синхронизирован с помощью условной переменной (condvar).

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define WRITERS_N 10
#define STR_LEN 64

char str[STR_LEN];
int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void *writer(void *arg)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        sprintf(str, "write counter = %d", counter);
        ++counter;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void *reader(void *arg)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("tid: %lx, array: %s\n", pthread_self(), str);
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_t tid[WRITERS_N + 1];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&tid[0], NULL, writer, NULL);
    for (int i = 1; i <= WRITERS_N; i++) {
        pthread_create(&tid[i], NULL, reader, NULL);
    }
    for (int i = 0; i <= WRITERS_N; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}