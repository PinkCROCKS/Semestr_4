#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "../task 1/errors.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#define N 5
#define MEALS_LIMIT 1
#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) < (b) ? (b) : (a))


typedef struct PhilosopherData {
    int id;
    int forks_semid;
    int table_semid;
} PhilosopherData;

void P(int semid, int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void V(int semid, int semnum) {
    struct sembuf op = {semnum, +1, 0};
    semop(semid, &op, 1);
}

void* philosopher(void* arg) {
    PhilosopherData* data = (PhilosopherData*)arg;
    int id = data->id;
    int forks_semid = data->forks_semid;
    int table_semid = data->table_semid;

    int left = min(id, (id + 1) % N);
    int right = max(id, (id + 1) % N);
    int meals = 0;

    while (meals < MEALS_LIMIT) {
        P(table_semid, 0);
        P(forks_semid, left);
        printf("Philosoph %d take left fork %d\n", id, left);

        P(forks_semid, right);
        printf("Philosoph %d take right fork %d\n", id, right);

        printf("Philosoph %d is eating\n", id);
        meals++;

        V(forks_semid, right);
        printf("Philosoph %d placed right fork %d\n", id, right);

        V(forks_semid, left);
        printf("Philosoph %d placed left fork %d\n", id, left);

        V(table_semid, 0);
    }

    printf("Philosoph %d SUCESS\n", id);
    free(data);
    return NULL;
}

int main() {
    pthread_t phils[N];
    int forks_semid, table_semid;
    union semun arg;

    if ((forks_semid = semget(IPC_PRIVATE, N, IPC_CREAT | 0666)) < 0) {
        return print_error(sem_create_error);
    }

    unsigned short values[N];
    for (int i = 0; i < N; i++) values[i] = 1;
    arg.array = values;
    if (semctl(forks_semid, 0, SETALL, arg) < 0) {
        return print_error(sem_create_error);
    }

    if ((table_semid = semget(IPC_PRIVATE, N-1, IPC_CREAT | 0666)) < 0) {
        semctl(forks_semid, 0, IPC_RMID);
        return print_error(sem_create_error);
    }

    arg.val = N - 1;
    if (semctl(table_semid, 0, SETVAL, arg) < 0) {
        semctl(forks_semid, 0, IPC_RMID);
        return print_error(sem_create_error);
    }

    for (int i = 0; i < N; i++) {
        PhilosopherData* data = (PhilosopherData *) malloc(sizeof(PhilosopherData));
        data->id = i;
        data->forks_semid = forks_semid;
        data->table_semid = table_semid;

        if (pthread_create(&phils[i], NULL, philosopher, data)) {
            semctl(forks_semid, 0, IPC_RMID);
            semctl(table_semid, 0, IPC_RMID);
            for (int j = 0; j < N; j++) {
                pthread_join(phils[j], NULL);
            }
            return print_error(thread_create_error);
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    semctl(forks_semid, 0, IPC_RMID);
    semctl(table_semid, 0, IPC_RMID);

    printf("Yra vse syty\n");
    return 0;
}