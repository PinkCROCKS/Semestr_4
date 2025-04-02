#include "server_utility.h"

errors init(int *msg_id_1, int *msg_id_2) {
    if (!msg_id_1 || !msg_id_2) {
        return input_error;
    }
    key_t tmp_key1 = ftok("/tmp", 4);
    if (tmp_key1 == -1) {
        return token_error;
    }

    int tmp_msg_1 = msgget(tmp_key1, IPC_CREAT | 0666);
    if (tmp_msg_1 == -1) {
        return token_error;
    }

    key_t tmp_key2 = ftok("/tmp", 7);
    if (tmp_key2 == -1) {
        msgctl(tmp_msg_1, IPC_RMID, 0);
        return token_error;
    }

    int tmp_msg_2 = msgget(tmp_key2, IPC_CREAT | 0666);
    if (tmp_msg_2 == -1) {
        msgctl(tmp_msg_1, IPC_RMID, 0);
        return token_error;
    }

    *msg_id_1 = tmp_msg_1;
    *msg_id_2 = tmp_msg_2;
    return OK;
}

Strings * create_strings(int k){
    Strings * temp = (Strings *) malloc(k * sizeof(Strings));
    if (temp == NULL){
        return NULL;
    }
    temp->capacity = 0;
    temp->size = 0;
    return temp;
}

errors free_strings(Strings * strings){
    if (strings->capacity == 0){
        return OK;
    }
    if (strings->size == 0){
        free(strings->content);
        return OK;
    }
    for (;strings->size > 0;){
        free(strings->content[--strings->size]);
    }
    free(strings->content);
    return OK;
}

errors append_str(Strings * strings,char * str){
    if (strings->capacity == 0){
        strings->size = 0;
        char** temp = (char**) malloc(sizeof(char *) * 2);
        if (!temp){
            return ptr_error;
        }
        strings->content = temp;
        strings->capacity = 2;
    } else if (strings->size == strings->capacity){
        char** temp = (char**) realloc(strings->content, strings->capacity * 2 * sizeof (char*));
        if (!temp){
            free_strings(strings);
            return ptr_error;
        }
        strings->content = temp;
        strings->capacity *= 2;
    }
    char * temp = (char *) malloc(strlen(str) + 1);
    if (temp == NULL){
        free_strings(strings);
        return ptr_error;
    }
    strcpy(temp, str);
    strings->content[strings->size++] = temp;
    return OK;
}

errors str_copy(char * input, char * output, int start, int end){
    int i = 0;
    int end_for = end - start;
    if (start > 0) end_for++;
    for (; i < end_for && i + start < strlen(input); i++){
        output[i] = input[i + start];
    }
    output[i] = '\0';
}

void print_strings(Strings * strings){
    for (int i = 0; i < strings->size; ++i) {
        printf("%d: %s\n", i + 1, strings->content[i]);
    }
}

errors processing_paths(Strings *input, Strings **output_t) {

    if (!input || !output_t) {
        return input_error;
    }

    Strings * output;
    DirEntry *dirs = malloc(input->size * sizeof(DirEntry));
    if (!dirs) {
        return memory_error;
    }
    size_t dir_count = 0;
    errors errorMsg;
    for (size_t i = 0; i < input->size; i++) {

        char dir[FILENAME_MAX];
        memset(dir, '\0', FILENAME_MAX);


        char file[FILENAME_MAX];
        memset(file, '\0', FILENAME_MAX);

        int index = 0;
        for(int k = 0; input->content[i][k]; ++k) {
            if(input->content[i][k] == '/') {
                index = k;
            }
        }

        str_copy(input->content[i], dir, 0, index);
        str_copy(input->content[i], file, index + 1, strlen(input->content[i]));


        size_t found = 0;
        for (size_t j = 0; j < dir_count; j++) {
            if (strcmp(dirs[j].dir, dir) == 0) {
                errorMsg = append_str(dirs[j].files, file);
                if (errorMsg) {
                    for (int k = 0; k < dir_count; ++k) {
                        free_strings(dirs[k].files);
                    }
                    free(dirs);
                    return errorMsg;
                }
                found = 1;
                break;
            }
        }
        if (!found) {
            strncpy(dirs[dir_count].dir, dir, FILENAME_MAX);
            dirs[dir_count].files = create_strings(1);
            if (!dirs[dir_count].files) {
                for (int k = 0; k < dir_count; ++k) {
                    free_strings(dirs[k].files);
                }
                free(dirs);
                return memory_error;
            }
            errorMsg = append_str((dirs[dir_count].files), file);
            if (errorMsg) {
                for (int k = 0; k < dir_count; ++k) {
                    free_strings(dirs[k].files);
                }
                free(dirs);
                return errorMsg;
            }
            dir_count++;
        }
    }

    output = create_strings(1);
    if (!output) {
        for (int k = 0; k < dir_count; ++k) {
            free_strings(dirs[k].files);
        }
        free(dirs);
        return memory_error;
    }

    for (size_t i = 0; i < dir_count; i++) {
        errorMsg = append_str(output, dirs[i].dir);
        if (errorMsg) {
            for (int k = 0; k < dir_count; ++k) {
                free_strings(dirs[k].files);
            }
            free_strings(output);
            free(dirs);
            return errorMsg;
        }

        for (size_t j = 0; j < dirs[i].files->size; j++) {
            errorMsg = append_str(output, dirs[i].files->content[j]);
            if (errorMsg) {
                for (int k = 0; k < dir_count; ++k) {
                    free_strings(dirs[k].files);
                }
                free(dirs);
                free_strings(output);
                return errorMsg;
            }
        }
    }
    for (int k = 0; k < dir_count; ++k) {
        free_strings(dirs[k].files);
    }
    free(dirs);
    *output_t = output;
    return OK;
}
