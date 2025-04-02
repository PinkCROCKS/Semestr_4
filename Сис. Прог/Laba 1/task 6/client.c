#include "utility.h"
#include "client.h"

struct MsgBuffer {
    long mtype;
    char filename[FILENAME_MAX];
    int is_end;
} MsgBuffer;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return amount_of_arguments_error;
    }

    int msg_id_1;
    int msg_id_2;

    errors errorMsg = init(&msg_id_1, &msg_id_2);
    if (errorMsg) {
        return print_error(errorMsg);
    }
    // отправляем данные
    struct MsgBuffer msg = {.mtype = getpid()};

    if(strcmp(argv[1], "STOP") == 0 && argc == 3 ) {
        strcpy(msg.filename, "STOP");
        int pass = atoi(argv[2]);
        msg.is_end = pass;
        int er = msgsnd(msg_id_1, &msg, sizeof(msg.filename) + sizeof(msg.is_end), 0);
        if (er == -1) {
            return ptr_error;
        }
        return 0;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        return input_error;
    }

    while (!feof(file)) {
        fscanf(file, "%4096s", msg.filename);
        msg.is_end = 0;
        if (feof(file)) {
            msg.is_end = 1;
        }
        int er = msgsnd(msg_id_1, &msg, sizeof(msg.filename) + sizeof(msg.is_end), 0);
        if (er == -1) {
            fclose(file);
            return ptr_error;
        }
    }


    // получаем данные
    msg.is_end = 0;
    while (!msg.is_end) {
        int er = msgrcv(msg_id_2, &msg, sizeof(msg.filename) + sizeof(msg.is_end), 0, 0);
        if (er == -1) {
            fclose(file);
            return token_error;
        }
        if(msg.filename[0] != '/') {
            printf("\t");
        }
        printf("%s\n", msg.filename);
    }
    fclose(file);
    return 0;
}
