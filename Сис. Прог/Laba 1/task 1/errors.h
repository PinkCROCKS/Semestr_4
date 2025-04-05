#ifndef LABA_1_ERRORS_H
#define LABA_1_ERRORS_H

#include <stdio.h>

typedef enum errors {
    OK = 0,
    file_error,
    same_files_error,
    amount_of_arguments_error,
    sign_error,
    input_error,
    memory_error,
    ptr_error,
    login_error,
    password_error,
    authorisation_error,
    end,
    time_error,
    flag_error,
    sanctions_limit_error,
    token_error,
    thread_create_error,
    sem_create_error,
    message_queue_error,
    socket_error,
    fork_error
} errors;

errors print_error(errors);

#endif //LABA_1_ERRORS_H
