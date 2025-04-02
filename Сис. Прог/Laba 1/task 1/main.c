#include "work.h"

int main(){
    Users* list = (Users *) malloc(sizeof(Users));
    if (list == NULL){
        return print_error(file_error);
    }
    list->capacity = 0;
    list->size = 0;
    errors error = load("base.bin", list);
    if (error) {
        free(list);
        return print_error(error);
    }
    char * login = (char *) malloc(MAX_LOGIN_LEN * sizeof(char ));
    error = dialog(list, login, 0);
    error = update("base.bin", list);
    if (error) {
        free(list);
        return print_error(error);
    }
    print_users(*list);
    free_users(list);
    free(login);
    return 0;
}