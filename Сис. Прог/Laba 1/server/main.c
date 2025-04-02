#include "../task 6/common.h"

int msgq_id, shm_id, sem_id;
struct shm_data *shm_ptr;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void init_server_resources() {
    key_t shm_key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) { perror("shmget"); exit(1); }

    shm_ptr = (struct shm_data*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) { perror("shmat"); exit(1); }

    key_t msgq_key = ftok(MSG_Q_KEY_PATH, MSG_Q_KEY_ID);
    msgq_id = msgget(msgq_key, IPC_CREAT | 0666);
    if (msgq_id == -1) { perror("msgget"); exit(1); }

    key_t sem_key = ftok(SEM_KEY_PATH, SEM_KEY_ID);
    sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) { perror("semget"); exit(1); }

    union semun arg; arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) { perror("semctl"); exit(1); }
}

void cleanup_server_resources() {
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    msgctl(msgq_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, 0);
}

void lock_sem() {
    struct sembuf op = {0, -1, SEM_UNDO};
    if (semop(sem_id, &op, 1) == -1) { perror("semop lock"); exit(1); }
}

void unlock_sem() {
    struct sembuf op = {0, 1, SEM_UNDO};
    if (semop(sem_id, &op, 1) == -1) { perror("semop unlock"); exit(1); }
}

void process_paths() {
    char *input = shm_ptr->data;
    char output[SHM_SIZE - sizeof(size_t)] = {0};
    char *out_ptr = output;

    char dirs[100][PATH_MAX];
    char files[100][100][PATH_MAX];
    int dir_count = 0;

    char *path = strtok(input, "\0");
    while (path && *path) {
        char dir_path[PATH_MAX], file_name[PATH_MAX];
        strcpy(dir_path, path);
        dirname(dir_path);
        strcpy(file_name, path);
        basename(file_name);

        int found = 0;
        for (int i = 0; i < dir_count; i++) {
            if (strcmp(dirs[i], dir_path) == 0) {
                strcpy(files[i][files[i][0][0]++], file_name);
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(dirs[dir_count], dir_path);
            files[dir_count][0][0] = 1;
            strcpy(files[dir_count][1], file_name);
            dir_count++;
        }
        path = strtok(NULL, "\0");
    }

    for (int i = 0; i < dir_count; i++) {
        strcpy(out_ptr, dirs[i]);
        out_ptr += strlen(dirs[i]) + 1;
        for (int j = 1; j <= files[i][0][0]; j++) {
            strcpy(out_ptr, files[i][j]);
            out_ptr += strlen(files[i][j]) + 1;
        }
        *out_ptr++ = '\0';
    }
    *out_ptr = '\0';
    shm_ptr->data_size = out_ptr - output + 1;
    memcpy(shm_ptr->data, output, shm_ptr->data_size);
}

int main() {
    init_server_resources();
    printf("Server started\n");

    struct msg_buffer msg;
    while (1) {
        if (msgrcv(msgq_id, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv"); break;
        }
        lock_sem();
        process_paths();
        struct msg_buffer resp = {.mtype = msg.pid};
        if (msgsnd(msgq_id, &resp, sizeof(resp) - sizeof(long), 0) == -1)
            perror("msgsnd");
        unlock_sem();
    }

    cleanup_server_resources();
    return 0;
}