//
// Created by Ярослав on 02.04.2025.
//

#ifndef TASK_2_CLIENT_H
#define TASK_2_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../task 1/errors.h"

#define PORT 28015
#define MAX_LENGTH 16

typedef struct msg_buffer {
    long type;
    u_char command;
    u_char argument;
    int id;
} Msg_Buffer;

char* get_command(FILE *file);
int parse_command(char *str, int *cmd, int *argument);

#endif //TASK_2_CLIENT_H