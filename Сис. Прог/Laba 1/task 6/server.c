#include "server_utility.h"

struct MsgBuffer {
    long mtype;
    char filename[FILENAME_MAX];
    int is_end;
} MsgBuffer;

int main() {
    int msg_id_1;
    int msg_id_2;

    errors errorMsg = init(&msg_id_1, &msg_id_2);
    if (errorMsg) {
        return errorMsg;
    }

    long type_input_msg = 0;

    struct MsgBuffer input_msg;
    Strings*  strings = (Strings *) malloc(sizeof(Strings));
    if (!strings) {
        msgctl(msg_id_1, IPC_RMID, 0);
        msgctl(msg_id_2, IPC_RMID, 0);
        return memory_error;
    }

    while (1) {
        int er = msgrcv(msg_id_1, &input_msg, sizeof(input_msg.filename) + sizeof(input_msg.is_end), 0, 0);
        if (er == -1) {
            msgctl(msg_id_1, IPC_RMID, 0);
            msgctl(msg_id_2, IPC_RMID, 0);
            return ptr_error;
        }

        if (strcmp("STOP\n", input_msg.filename) == 0 && input_msg.is_end == 12345) {
            break;
        }

        // Начинаем складывать в массив пути от одного клиента
        if (type_input_msg == 0) {
            free_strings(strings);
            type_input_msg = input_msg.mtype;
        }

        // Добавляем в путь
        if (type_input_msg > 0) {
            if(access(input_msg.filename, F_OK) == 0) {
                errorMsg = append_str(strings, input_msg.filename);
                if (errorMsg) {
                    free_strings(strings);
                    msgctl(msg_id_1, IPC_RMID, 0);
                    msgctl(msg_id_2, IPC_RMID, 0);
                    return errorMsg;
                }
            }
        }

        // Конец запроса от одного клиента
        if (input_msg.is_end) {
            Strings *  output;
            errorMsg = processing_paths(strings, &output);
            if (errorMsg) {
                free_strings(strings);
                msgctl(msg_id_1, IPC_RMID, 0);
                msgctl(msg_id_2, IPC_RMID, 0);
                return errorMsg;
            }

            for (int j = 0; j < output->size; ++j) {
                struct MsgBuffer output_msg = {.mtype = type_input_msg};
                strncpy(output_msg.filename, output->content[j], FILENAME_MAX);
                if (j == output->size - 1) {
                    output_msg.is_end = 1;
                }
                er = msgsnd(msg_id_2, &output_msg, sizeof(output_msg.filename) + sizeof(output_msg.is_end), 0);
                if (er) {
                    free_strings(strings);
                    msgctl(msg_id_1, IPC_RMID, 0);
                    msgctl(msg_id_2, IPC_RMID, 0);
                    return memory_error;
                }
            }
            free_strings(output);
            type_input_msg = 0;
        }
    }

    free_strings(strings);
    msgctl(msg_id_1, IPC_RMID, 0);
    msgctl(msg_id_2, IPC_RMID, 0);
    return 0;
}