#include "server_utility.h"

struct Message_Data {
    long type;
    char filename[FILENAME_MAX];
    int quit;
} Message_Data;

int main() {
    int message1_queue_id;
    int message2_queue_id;

    errors error = start_resourses(&message1_queue_id, &message2_queue_id);
    if (error) {
        return error;
    }

    long message_type = 0;

    struct Message_Data input_message;
    Strings*  strings = (Strings *) malloc(sizeof(Strings));
    if (!strings) {
        msgctl(message1_queue_id, IPC_RMID, 0);
        msgctl(message2_queue_id, IPC_RMID, 0);
        return memory_error;
    }

    while (1) {
        int er = msgrcv(message1_queue_id, &input_message, sizeof(input_message.filename) + sizeof(input_message.quit), 0, 0);
        if (er == -1) {
            msgctl(message1_queue_id, IPC_RMID, 0);
            msgctl(message2_queue_id, IPC_RMID, 0);
            return ptr_error;
        }

        if (strcmp("STOP\n", input_message.filename) == 0 && input_message.quit == 12345) {
            break;
        }
        if (message_type == 0) {
            free_strings(strings);
            message_type = input_message.type;
        }

        if (message_type > 0) {
            if(access(input_message.filename, F_OK) == 0) {
                error = append_str(strings, input_message.filename);
                if (error) {
                    free_strings(strings);
                    msgctl(message1_queue_id, IPC_RMID, 0);
                    msgctl(message2_queue_id, IPC_RMID, 0);
                    return error;
                }
            }
        }

        if (input_message.quit) {
            Strings *  output;
            error = processing_paths(strings, &output);
            if (error) {
                free_strings(strings);
                msgctl(message1_queue_id, IPC_RMID, 0);
                msgctl(message2_queue_id, IPC_RMID, 0);
                return error;
            }

            for (int j = 0; j < output->size; ++j) {
                struct Message_Data output_msg = {.type = message_type};
                strncpy(output_msg.filename, output->content[j], FILENAME_MAX);
                if (j == output->size - 1) {
                    output_msg.quit = 1;
                }
                er = msgsnd(message2_queue_id, &output_msg, sizeof(output_msg.filename) + sizeof(output_msg.quit), 0);
                if (er) {
                    free_strings(strings);
                    msgctl(message1_queue_id, IPC_RMID, 0);
                    msgctl(message2_queue_id, IPC_RMID, 0);
                    return memory_error;
                }
            }
            free_strings(output);
            message_type = 0;
        }
    }

    free_strings(strings);
    msgctl(message1_queue_id, IPC_RMID, 0);
    msgctl(message2_queue_id, IPC_RMID, 0);
    return 0;
}