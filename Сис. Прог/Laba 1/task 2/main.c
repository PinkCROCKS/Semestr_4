#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <ctype.h>
#include <limits.h>

#include "../task 1/errors.h"

int is_command(char * input){
    return (!strncmp(input, "xor", 3) || !strncmp(input, "mask", 4) ||
    !strncmp(input, "copy", 4) || !strncmp(input, "find", 4));
}

int validate_args(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file1> [file2...] <command>\n", argv[0]);
        return 0;
    }
    return 1;
}

int xorN(char **files, int amount_of_files, int n, size_t *result) {
    *result = 0;

    if (n < 2 || n > 6) {
        fprintf(stderr, "Error: n must be between 2 and 6\n");
        return input_error;
    }

    size_t block_bits = 1UL << n;
    size_t block_bytes = block_bits / 8;
    int process_half_byte = (n == 2);

    for (int i = 0; i < amount_of_files; ++i) {
        FILE *file = fopen(files[i], "rb");
        if (!file) {
            print_error(file_error);
            continue;
        }

        if (process_half_byte) {
            int byte;
            while ((byte = fgetc(file)) != EOF) {
                unsigned char blocks[2] = {
                        (unsigned char)((byte >> 4) & 0x0F),
                        (unsigned char)(byte & 0x0F)
                };

                for (int j = 0; j < 2; ++j) {
                    *result ^= blocks[j];
                }
            }
        } else {
            unsigned char *block = malloc(block_bytes);
            if (!block) {
                fclose(file);
                return print_error(file_error);
            }

            while (1) {
                size_t bytes_read = fread(block, 1, block_bytes, file);
                if (bytes_read == 0) break;
                if (bytes_read < block_bytes) {
                    memset(block + bytes_read, 0, block_bytes - bytes_read);
                }
                size_t value = 0;
                for (size_t j = 0; j < block_bytes; ++j) {
                    value |= (size_t)block[j] << (j * 8);
                }
                *result ^= value;
            }
            free(block);
        }
        fclose(file);
    }
    return OK;
}

errors is_valid_hex_mask(const char *mask_str) {
    if (mask_str == NULL || strlen(mask_str) == 0 || strlen(mask_str) > 8) {
        return input_error;
    }

    for (size_t i = 0; i < strlen(mask_str); i++) {
        if (!isxdigit(mask_str[i])) return input_error;
    }
    return OK;
}

int process_mask(const char *file, const char *mask_str, int *result) {
    if (!file || !mask_str || !result) {
        return ptr_error;
    }

    if (!is_valid_hex_mask(mask_str)){
        return input_error;
    }

    uint32_t mask_value = strtoul(mask_str, NULL, 16);
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        return file_error;
    }

    uint32_t number;
    uint8_t buffer[4];
    size_t bytes_read;
    int count = 0;

    while ((bytes_read = fread(buffer, 1, 4, fp)) > 0) {
        if (bytes_read < 4) memset(buffer + bytes_read, 0, 4 - bytes_read);
        number = (uint32_t)buffer[0] |
                 ((uint32_t)buffer[1] << 8) |
                 ((uint32_t)buffer[2] << 16) |
                 ((uint32_t)buffer[3] << 24);
        if ((number & mask_value) == mask_value) {
            count++;
        }
    }

    fclose(fp);
    *result = count;
    return OK;
}

int copyN(char** files, int amount_of_files, int n) {
    if (files == NULL) {
        return ptr_error;
    }

    if (amount_of_files <= 0 || n <= 0){
        return input_error;
    }

    int global_error = 0;

    for (int i = 0; i < amount_of_files; ++i) {
        const char* filename = files[i];

        if (filename == NULL || strlen(filename) == 0) {
            printf("File %d: Incorrect File", i);
            global_error = 1;
            continue;
        }

        for (int copy_num = 1; copy_num <= n; ++copy_num) {
            pid_t pid = fork();

            if (pid == -1) {
                print_error(fork_error);
                global_error = 1;
                continue;
            }
            else if (pid == 0) {
                char new_name[PATH_MAX];
                snprintf(new_name, sizeof(new_name), "%s_%d", filename, copy_num);

                execlp("cp", "cp", filename, new_name, NULL);
                return file_error;
            }
            else {
                int status;
                waitpid(pid, &status, 0);

                if (WIFEXITED(status)) {
                    if (WEXITSTATUS(status) != 0) {
                        printf("Copy %d for %s failed\n", copy_num, filename);
                        global_error = 1;
                    }
                } else {
                    printf("Copy %d for %s terminated abnormally\n", copy_num, filename);
                    global_error = 1;
                }
            }
        }
    }
    if (global_error == 0) {
        return OK;
    }
    return global_error;
}

int shift_table(const char *template, int n, int *result) {
    if (n == 0) return print_error(input_error);
    result[0] = 0;
    int i = 1, j = 0;

    while (i < n) {
        if (template[i] == template[j]) result[i++] = ++j;
        else {
            if (j != 0) j = result[j - 1];
            else result[i++] = 0;
        }
    }
    return 0;
}

int substr(const char* str, int n, const char* key, int m) {
    int *table = malloc(m * sizeof(int));
    if (!table) return -1;

    shift_table(key, m, table);
    int i = 0, j = 0;

    while (i < n) {
        if (str[i] == key[j]) {
            i++;
            j++;
            if (j == m) {
                free(table);
                return i - j;
            }
        } else {
            if (j > 0) j = table[j - 1];
            else i++;
        }
    }

    free(table);
    return -1;
}

int line_counter(const char *str, int size) {
    int count = 1;
    for (int i = 0; i < size; ++i) if (str[i] == '\n') count++;
    return count;
}

int pos_in_line(const char *str, int size) {
    for (int i = size - 1; i >= 0; --i) if (str[i] == '\n') return size - i - 1;
    return size;
}

int file_handler(const char *key, int m, const char *filename) {
    FILE* stream = fopen(filename, "rb");
    if (!stream) return print_error(file_error);

    fseek(stream, 0, SEEK_END);
    long size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    char *content = malloc(size + 1);
    if (!content) {
        fclose(stream);
        return print_error(memory_error);
    }

    fread(content, 1, size, stream);
    content[size] = '\0';
    fclose(stream);

    int *table = malloc(m * sizeof(int));
    if (!table) {
        free(content);
        return print_error(memory_error);
    }

    shift_table(key, m, table);

    int ind = 0, found = 0;
    while (ind < size) {
        int pos = substr(content + ind, size - ind, key, m);
        if (pos == -1) break;
        ind += pos;
        printf("Found in %s: line %d, position %d\n", filename,
               line_counter(content, ind), pos_in_line(content, ind));
        ind += 1;
        found = 1;
    }

    free(content);
    free(table);
    return found ? 0 : 1;
}

int find_substr_in_files(const char *substr, int m, int file_count, char **filenames) {
    for (int i = 0; i < file_count; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            int err = file_handler(substr, m, filenames[i]);
            exit(err);
        } else if (pid < 0) print_error(fork_error);
    }

    int status, found_any = 0;
    while (wait(&status) > 0) {
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) found_any = 1;
    }

    if (!found_any) printf("No Files with this str\n");
    return 0;
}

errors process_find(char **input_files, int input_files_count, const char *search_str) {
    int m = strlen(search_str);
    if (m == 0) return print_error(input_error);
    return find_substr_in_files(search_str, m, input_files_count, input_files);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        return print_error(amount_of_arguments_error);
    }

    if (!is_command(argv[argc - 1])) {
        return print_error(input_error);
    }
    char *command = argv[argc - 1];
    char ** files = &argv[1];
    if (strncmp(command, "xor", 3) == 0) {
        size_t result = 0;
        int n = atoi(command + 3);
        if (n < 2 || n > 6) {
            return print_error(input_error);
        }
        errors error =  xorN(files, argc - 2, n, &result);
        if (error) {
            return print_error(error);
        }
        printf("Result of XOR 2^%d = %zu", n, result);
    } else if (!strncmp(command, "mask", 4)) {
        char * input;
        int n = sscanf(command, "mask<%s>", input);
        if (n != 1){
            return input_error;
        }
        n = 0;
        for (int i = 0; i < argc - 2; i++) {
            errors error = process_mask(files[i], input, &n);
            if (error == file_error) {
                printf("File %d: No such file or Directory\n", i + 1);
            }
            printf("File %d: %d\n", i + 1, n);
        }
    } else if (strncmp(command, "copy", 4) == 0) {
        int n = atoi(command + 4);
        if (n <= 0) {
            return print_error(input_error);
        }
        errors error = copyN(files, argc - 2, n);
        if (error) {
            return print_error(error);
        }
    }
    else if (strncmp(command, "find", 4) == 0) {
        char * input = (char *) malloc(strlen(command));
        int n = sscanf(command, "find<%s>", input);
        if (n != 1){
            free(input);
            return print_error(input_error);
        }
        errors error = process_find(files, argc - 2, input);
        if (error){
            free(input);
            return error;
        }
        free(input);
    }
    else {
        printf("Incorrect command\n");
    }
    return OK;
}