//
// Created by Ярослав on 09.03.2025.
//

#ifndef LABA_1_WORK_H
#define LABA_1_WORK_H
#pragma once

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "errors.h"

#define MAX_LOGIN_LEN 6
#define CORRECT_SYMBOL(i, k) ((i[k] > '9' || i[k] < '0') && (i[k] < 'a' || i[k] > 'z') && (i[k] < 'A' || i[k] > 'Z'))

typedef struct User {
    char login[MAX_LOGIN_LEN + 1];
    int password;
    int sanctions;
} User;

typedef struct Users {
    User* data;
    size_t capacity;
    size_t size;
} Users;

typedef struct Status{
    errors error;
    int exit;
} Status;

typedef enum Command { TIME_C, DATE_C, HOW_MUCH_C, LOGOUT_C, SANCTIONS_C, UNRECOGNIZED_C } Command;

errors get_time_string(char*);
errors get_date_string(char*);
errors check_login(const char* login);
void destroy_buffer();
errors start_window(Users*, char*, int*);
void print_users(Users users);
errors dialog(Users* users, char* login, int win);
errors update(const char* filename, Users* users);
errors load(const char* filename, Users* users);
errors free_users(Users* users);

#endif //LABA_1_WORK_H
