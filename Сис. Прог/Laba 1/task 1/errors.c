#include "errors.h"

errors print_error(errors error){
    switch (error) {
        case file_error:
            printf("No such file or Directory or Another problem with file");
            return file_error;
        case same_files_error:
            printf("All files must be different");
            return same_files_error;
        case amount_of_arguments_error:
            printf("You have mistake with amount of arguments");
            return amount_of_arguments_error;
        case input_error:
            printf("Unknown symbol");
            return input_error;
        case memory_error:
            printf("Problems with declaration of memory");
            return memory_error;
        case thread_create_error:
            printf("Mistake with threade create");
            return thread_create_error;
        case sem_create_error:
            printf("Mistake with semaphor create");
            return thread_create_error;
        case message_queue_error:
            printf("Mistake with message queue create");
            return message_queue_error;
        case socket_error:
            printf("Socket_error");
            return socket_error;
        case fork_error:
            printf("fork_error");
            return socket_error;
    }
}



