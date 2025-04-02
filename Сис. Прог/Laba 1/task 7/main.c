#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#include "../task 1/errors.h"

char letters[] = "dlcbps-";

typedef enum types{
    directiry,
    usual,
    link,
    simdev,
    blockdev,
    fifo,
    socket
} types;

struct file_info {
    char *name;
    long long int size;
    char permissions[11];
    char type;
    char time[20];
};

types get_file_type(mode_t mode) {
    if (S_ISDIR(mode))  return directiry;
    if (S_ISLNK(mode))  return link;
    if (S_ISCHR(mode))  return simdev;
    if (S_ISBLK(mode))  return blockdev;
    if (S_ISFIFO(mode)) return fifo;
    if (S_ISSOCK(mode)) return socket;
    return usual;
}

void format_time(time_t mtime, char *buf) {
    struct tm *tm_info = localtime(&mtime);
    strftime(buf, 20, "%b %e %H:%M", tm_info);
}

void get_permissions(mode_t mode, char *perm_str) {
    perm_str[0] =
                    S_ISDIR(mode)  ? 'd' :
                    S_ISCHR(mode)  ? 'c' :
                    S_ISBLK(mode)  ? 'b' :
                    S_ISFIFO(mode) ? 'p' :
                    S_ISLNK(mode)  ? 'l' :
                    S_ISSOCK(mode) ? 's' : '-';

    // Права пользователя
    perm_str[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm_str[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm_str[3] = (mode & S_IXUSR) ? 'x' : '-';

    // Права группы
    perm_str[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm_str[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm_str[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Права остальных
    perm_str[7] = (mode & S_IROTH) ? 'r' : '-';
    perm_str[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm_str[9] = (mode & S_IXOTH) ? 'x' : '-';

    // Специальные биты
    if (mode & S_ISUID)
        perm_str[3] = (perm_str[3] == 'x') ? 's' : 'S';
    if (mode & S_ISGID)
        perm_str[6] = (perm_str[6] == 'x') ? 's' : 'S';
    if (mode & S_ISVTX)
        perm_str[9] = (perm_str[9] == 'x') ? 't' : 'T';

    perm_str[10] = '\0';
}

int compare(const void *a, const void *b) {
    return strcmp(((struct file_info*)a)->name, ((struct file_info*)b)->name);
}

errors process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        return input_error;
    }
    int capacity = 1;
    struct file_info *files = (struct file_info *) malloc(sizeof (struct file_info));
    if (files == NULL){
        return memory_error;
    }
    size_t count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st)) {
            continue;
        }
        if (count == capacity){
            struct file_info *tmp = realloc(files, (capacity * 2) * sizeof(struct file_info));
            if (!tmp) {
                break;
            }
            files = tmp;
            capacity *= 2;
        }

        files[count].name = strdup(entry->d_name);
        files[count].size = st.st_size;
        get_permissions(st.st_mode, files[count].permissions);
        files[count].type = letters[get_file_type(st.st_mode)];
        format_time(st.st_mtime, files[count].time);
        count++;
    }

    closedir(dir);
    qsort(files, count, sizeof(struct file_info), compare);
    for (size_t i = 0; i < count; i++) {
        if (files[i].type == 'd'){
            printf("d%s %lld %s [%s] \n",
                   files[i].permissions,
                   files[i].size,
                   files[i].time,
                   files[i].name
            );
        } else {
            printf("%c%s %lld %s %s \n",
                   files[i].type,
                   files[i].permissions,
                   files[i].size,
                   files[i].time,
                   files[i].name
            );
        }
        free(files[i].name);
    }
    free(files);
    return OK;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        process_directory(".");
    } else {
        for (int i = 1; i < argc; i++) {
            if (argc > 2) {
                printf("%s:\n", argv[i]);
            }
            if (process_directory(argv[i]) == input_error) {
                printf("No such file or Directory\n");
            } else if (process_directory(argv[i]) == memory_error){
                printf("Memory error\n");
            } else {
                printf("\n");
            }
        }
    }
    return 0;
}