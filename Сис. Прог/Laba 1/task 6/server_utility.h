#ifndef TASK_6_SERVER_UTILITY_H
#define TASK_6_SERVER_UTILITY_H

#pragma once

#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../task 1/errors.h"

typedef struct strings {
    char ** content;
    int size;
    int capacity;
} Strings;

typedef struct {
    char dir[FILENAME_MAX];
    Strings* files;
} DirEntry;

errors start_resourses(int* msg_id_1, int* msg_id_2);

errors processing_paths(Strings * input, Strings ** output);
errors free_strings(Strings * strings);
errors append_str(Strings * strings, char * str);
void print_strings(Strings * strings);

#endif //TASK_6_SERVER_UTILITY_H
