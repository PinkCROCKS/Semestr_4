#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#include "../task 1/errors.h"

typedef struct {
    int women_count;
    int men_count;
} BathroomState;

typedef struct {
    BathroomState* bathroom;
    int semid;
    int max_people;
} ThreadArgs;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

enum Semaphores {
    SEM_MUTEX,
    SEM_SPACES
};

void sem_op(int semid, int sem_num, int op) {
    struct sembuf buf = {sem_num, op, 0};
    if (semop(semid, &buf, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void woman_wants_to_enter(BathroomState* bathroom, int semid, int max_people) {
    while (1) {
        sem_op(semid, SEM_MUTEX, -1);

        if (bathroom->men_count == 0) {
            int available;
            if ((available = semctl(semid, SEM_SPACES, GETVAL)) > 0) {
                sem_op(semid, SEM_SPACES, -1);
                bathroom->women_count++;
                printf("Женщина вошла. Состояние: %s [%d/%d]\n",
                       (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
                       bathroom->men_count > 0 ? "Мужчины" : "Женщины",
                       bathroom->women_count + bathroom->men_count, max_people);
                sem_op(semid, SEM_MUTEX, 1);
                break;
            }
        }

        sem_op(semid, SEM_MUTEX, 1);
        usleep(100000);
    }
}

void man_wants_to_enter(BathroomState* bathroom, int semid, int max_people) {
    while (1) {
        sem_op(semid, SEM_MUTEX, -1);

        if (bathroom->women_count == 0) {
            int available;
            if ((available = semctl(semid, SEM_SPACES, GETVAL)) > 0) {
                sem_op(semid, SEM_SPACES, -1);
                bathroom->men_count++;
                printf("Мужчина вошел. Состояние: %s [%d/%d]\n",
                       (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
                       bathroom->women_count > 0 ? "Женщины" : "Мужчины",
                       bathroom->women_count + bathroom->men_count, max_people);
                sem_op(semid, SEM_MUTEX, 1);
                break;
            }
        }

        sem_op(semid, SEM_MUTEX, 1);
        usleep(100000);
    }
}

void woman_leaves(BathroomState* bathroom, int semid, int max_people) {
    sem_op(semid, SEM_MUTEX, -1);
    if (bathroom->women_count > 0){
        bathroom->women_count--;
        sem_op(semid, SEM_SPACES, 1);
        printf("Женщина вышла. Состояние: %s [%d/%d]\n",
               (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
               bathroom->men_count > 0 ? "Мужчины" : "Женщины",
               bathroom->women_count + bathroom->men_count, max_people);
    } else {
        printf("В туалете нет женщин ");
        printf("Состояние: %s [%d/%d]\n",
               (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
               bathroom->men_count > 0 ? "Мужчины" : "Женщины",
               bathroom->women_count + bathroom->men_count, max_people);
    }
    sem_op(semid, SEM_MUTEX, 1);
}

void man_leaves(BathroomState* bathroom, int semid, int max_people) {
    sem_op(semid, SEM_MUTEX, -1);
    if (bathroom->men_count > 0) {
        bathroom->men_count--;
        sem_op(semid, SEM_SPACES, 1);
        printf("Мужчина вышел. Состояние: %s [%d/%d]\n",
               (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
               bathroom->women_count > 0 ? "Женщины" : "Мужчины",
               bathroom->women_count + bathroom->men_count, max_people);
    } else {
        printf("В туалете нет мужчин ");
        printf("Состояние: %s [%d/%d]\n",
               (bathroom->women_count + bathroom->men_count) == 0 ? "Пусто" :
               bathroom->women_count > 0 ? "Женщины" : "Мужчины",
               bathroom->women_count + bathroom->men_count, max_people);
    }
    sem_op(semid, SEM_MUTEX, 1);
}

void* woman_thread(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    woman_wants_to_enter(targs->bathroom, targs->semid, targs->max_people);
    return NULL;
}

void* man_thread(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    man_wants_to_enter(targs->bathroom, targs->semid, targs->max_people);
    return NULL;
}

void * man_thread2(void* args){
    ThreadArgs* targs = (ThreadArgs*)args;
    man_leaves(targs->bathroom, targs->semid, targs->max_people);
    return NULL;
}

void * woman_thread2(void* args){
    ThreadArgs* targs = (ThreadArgs*)args;
    woman_leaves(targs->bathroom, targs->semid, targs->max_people);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return print_error(amount_of_arguments_error);
    }

    const int max_people = atoi(argv[1]);
    key_t key = ftok("/tmp", 'B');

    int shmid = shmget(key, sizeof(BathroomState), IPC_CREAT | 0666);
    if (shmid == -1){
        return memory_error;
    }
    BathroomState* bathroom = shmat(shmid, NULL, 0);
    bathroom->women_count = 0;
    bathroom->men_count = 0;

    int semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == 0) {
        shmdt(bathroom);
        shmctl(shmid, IPC_RMID, NULL);
        return memory_error;
    }
    union semun arg;
    arg.val = 1;
    semctl(semid, SEM_MUTEX, SETVAL, arg);
    arg.val = max_people;
    semctl(semid, SEM_SPACES, SETVAL, arg);

    ThreadArgs targs = {bathroom, semid, max_people};

    pthread_t threads[10];
    char * input = NULL;
    long int len;
    getline(&input, &len, stdin);
    int i = 0;
    int j = 5;
    int m = 4;
    int w = -1;
    while (strcmp(input, "0\n")){
        if (!strcmp(input, "1\n")) {
            pthread_create(&threads[(j++) % 5 + 5], NULL, man_thread, &targs);
            if (m + 1 < 9) m++;
        } else if (!strcmp(input, "2\n")) {
            pthread_create(&threads[(j++) % 5 + 5], NULL, man_thread2, &targs);
        } else if (!strcmp(input, "3\n")) {
            pthread_create(&threads[i++ % 5], NULL, woman_thread, &targs);
            if (w + 1 < 5) w++;
        } else if (!strcmp(input, "4\n")) {
            pthread_create(&threads[i++ % 5], NULL, woman_thread2, &targs);
        }
        getline(&input, &len, stdin);
    }

    free(input);


//    for (int k = 0; k < max_people; ++k) {
//        pthread_join(threads[k], NULL);
//    }
    for (; w > -1; w--) {
        pthread_join(threads[w], NULL);
    }
    for (; m > 4; m--) {
        pthread_join(threads[m], NULL);
    }

    shmdt(bathroom);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return EXIT_SUCCESS;
}