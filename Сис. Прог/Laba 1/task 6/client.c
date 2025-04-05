#include "utility.h"
#include "client.h"

struct Message_Data {
    long type;
    char name[FILENAME_MAX];
    int quit;
} Message_Data;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return amount_of_arguments_error;
    }

    int message1_queue_id;
    int message2_queue_id;

    errors error = start_resourses(&message1_queue_id, &message2_queue_id);
    if (error) {
        return print_error(error);
    }
    struct Message_Data message = {.type = getpid()};

//    if(strcmp(argv[1], "STOP") == 0 && argc == 3 ) {
//        strcpy(message.name, "STOP");
//        int pass = atoi(argv[2]);
//        message.quit = pass;
//        int er = messagesnd(message1_queue_id, &message, sizeof(message.name) + sizeof(message.quit), 0);
//        if (er == -1) {
//            return ptr_error;
//        }
//        return 0;
//    }
//
//    FILE* file = fopen(argv[1], "r");
//    if (!file) {
//        return input_error;
//    }

    for (int i = 1; i < argc; i++) {
        strcpy(message.name, argv[i]);
        message.quit = 0;
        if (i == argc - 1) {
            message.quit = 1;
        }
        int er = msgsnd(message1_queue_id, &message, sizeof(message.name) + sizeof(message.quit), 0);
        if (er == -1) {
            return ptr_error;
        }
    }

    message.quit = 0;
    while (!message.quit) {
        int er = msgrcv(message2_queue_id, &message, sizeof(message.name) + sizeof(message.quit), 0, 0);
        if (er == -1) {
            return token_error;
        }
        if(message.name[0] != '/') {
            printf("\t");
        }
        printf("%s\n", message.name);
    }
    return 0;
}
