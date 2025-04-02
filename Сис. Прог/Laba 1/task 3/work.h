#ifndef TASK_3_WORK_H
#define TASK_3_WORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "../task 1/errors.h"

#define MAX_PEOPLE 5
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

typedef struct {
    int women_count;
    int men_count;
} Bathroom;

void woman_wants_to_enter(Bathroom *bathroom, int semid);
void woman_leaves(Bathroom *bathroom, int semid);
void man_wants_to_enter(Bathroom *bathroom, int semid);
void man_leaves(Bathroom *bathroom, int semid);
#endif //TASK_3_WORK_H
