#include "client.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        return print_error(amount_of_arguments_error);
    }

    key_t token = ftok("./ss", 55);

    int message_queue = msgget(token, 0);
    if (message_queue < 0) {
        return print_error(message_queue_error);
    }

    Msg_Buffer message;

    struct sockaddr_in address;

    int id = socket(AF_INET, SOCK_STREAM, 0);
    if (id < 0) {
        return print_error(socket_error);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) < 0) {
        close(id);
        return print_error(input_error);
    }

    int status = connect(id, (struct sockaddr *) &address, sizeof(address));
    if (status < 0) {
        close(id);
        return print_error(socket_error);
    }

    message.type = 1;

    size_t current_read = msgrcv(message_queue, &message, sizeof(message) - sizeof(long), 1, 0);
    if (current_read < 1) {
        close(id);
        return print_error(message_queue_error);
    }


    FILE *file = fopen(argv[1], "r");
    if (!file) {
        close(id);
        return print_error(file_error);
    }

    char *command;

    int command_id, argument;

    while (!feof(file)) {
        command = get_command(file);
        if (!command) {
            close(id);
            return print_error(file_error);
        }

        if (parse_command(command, &command_id, &argument) < 0) {
            printf("Incorrect command!\n");
            free(command);
        } else {
            if (msgrcv(message_queue, &message, sizeof(message) - sizeof(long), 0, IPC_NOWAIT)) {
                if (message.type == 2) {
                    break;
                }
            }

            message.command = command_id;
            message.argument = argument;
            message.type = 1;

            free(command);

            if (msgsnd(message_queue, &message, sizeof(message) - sizeof(long), 0) < 0) {
                close(id);
                return print_error(message_queue_error);
            }

        }
    }

    message.type = 2;

    if (msgsnd(message_queue, &message, sizeof(message) - sizeof(long), 0) < 0) {
        close(id);
        return print_error(message_queue_error);
    }


    close(id);

    return 0;
}

char* get_command(FILE *file) {
    if (!file)
        return NULL;
    size_t size;
    char * result = (char *) calloc(MAX_LENGTH, sizeof(char));
    if (!result)
        return NULL;

    getline(&result, &size, file);

    return result;
}

int parse_command(char *str, int *cmd, int *argument) {
    char *command = strtok(str, " ");

    if (!command) {
        *cmd = -1;
        return -1;
    }

    if (strcmp(command, "take") == 0) {
        *cmd = 1;

        char *arg = strtok(NULL, " ");
        if (!arg) {
            *argument = -1;
            return -1;
        }
        if (!strcmp(arg, "wolf\n")) {
            *argument = 1;
        } else if (!strcmp(arg, "goat\n")) {
            *argument = 2;
        } else if (!strcmp(arg, "cabbage\n")) {
            *argument = 3;
        } else {
            *argument = -1;
        }
    } else if (!strcmp(command, "put\n") || !strcmp(command, "put")) {
        *cmd = 2;
    } else if (!strcmp(command, "move\n")) {
        *cmd = 3;
    } else {
        *cmd = -1;
        return -1;
    }

    return 0;
}