#include "utility.h"

errors start_resourses(int *msg_id_1, int *msg_id_2) {
    if (!msg_id_1 || !msg_id_2) {
        return ptr_error;
    }
    key_t tmp_key1 = ftok("/tmp", 4);
    if (tmp_key1 == -1) {
        return token_error;
    }

    int tmp_msg_1 = msgget(tmp_key1, 0666);
    if (tmp_msg_1 == -1) {
        return token_error;
    }

    key_t tmp_key2 = ftok("/tmp", 7);
    if (tmp_key2 == -1) {
        return token_error;
    }

    int tmp_msg_2 = msgget(tmp_key2, 0666);
    if (tmp_msg_2 == -1) {
        msgctl(tmp_msg_1, IPC_RMID, 0);
        return token_error;
    }

    *msg_id_1 = tmp_msg_1;
    *msg_id_2 = tmp_msg_2;
    return OK;
}