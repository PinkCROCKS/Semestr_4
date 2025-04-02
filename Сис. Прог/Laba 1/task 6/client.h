#ifndef TASK_6_CLIENT_H
#define TASK_6_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <unistd.h>
#include <limits.h>

#define SHM_KEY_PATH "/tmp"
#define SHM_KEY_ID 1234
#define MSG_Q_KEY_PATH "/tmp"
#define MSG_Q_KEY_ID 5678
#define SEM_KEY_PATH "/tmp"
#define SEM_KEY_ID 91011
#define SHM_SIZE (64 * 1024)

struct msg_buffer {
    long mtype;
    pid_t pid;
    char mtext[1];
};

struct shm_data {
    size_t data_size;
    char data[SHM_SIZE - sizeof(size_t)];
};

#endif
