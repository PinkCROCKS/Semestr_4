#include "work.h"

void sem_lock(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};
    semop(semid, &op, 1);
}

void sem_unlock(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    semop(semid, &op, 1);
}

void init_sems(int semid) {
    union semun {
        int val;
        struct semid_ds *buf;
        ushort *array;
    } arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    arg.val = MAX_PEOPLE;
    semctl(semid, 1, SETVAL, arg);
}

void woman_wants_to_enter(Bathroom *bathroom, int semid){
    sem_lock(semid, 0);
    while (bathroom->men_count > 0) {
        printf("Занято\n");
        sem_unlock(semid, 0);
        sem_lock(semid, 0);
    }
    sem_lock(semid, 1);
    bathroom->women_count++;
    printf("Женщина вошла. Женщин: %d, Мужчин: %d\n", bathroom->women_count, bathroom->men_count);
    sem_unlock(semid, 0);
    sem_unlock(semid, 1);
}

void woman_leaves(Bathroom *bathroom, int semid) {
    sem_lock(semid, 0);
    bathroom->women_count--;
    sem_unlock(semid, 1);
    printf("Женщина вышла. Женщин: %d, Мужчин: %d\n", bathroom->women_count, bathroom->men_count);
    sem_unlock(semid, 0);
}

void man_wants_to_enter(Bathroom *bathroom, int semid) {
    sem_lock(semid, 0);

    while (bathroom->women_count > 0) {
        printf("Занято\n");
        sem_unlock(semid, 0);
        sem_lock(semid, 0);
    }

    sem_lock(semid, 1);
    bathroom->men_count++;
    printf("Мужчина вошел. Женщин: %d, Мужчин: %d\n",
           bathroom->women_count, bathroom->men_count);
    sem_unlock(semid, 1);
    sem_unlock(semid, 0);
}

void man_leaves(Bathroom *bathroom, int semid) {
    sem_lock(semid, 0);
    bathroom->men_count--;
    sem_unlock(semid, 1);
    printf("Мужчина вышел. Женщин: %d, Мужчин: %d\n", bathroom->women_count, bathroom->men_count);
    sem_unlock(semid, 0);
}