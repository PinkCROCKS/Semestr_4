#include "work.h"

errors check_login(const char* login) {
    if (login == NULL) {
        return ptr_error;
    }
    for (int i = 0; login[i]; i++) {
        if (i >= 6 || CORRECT_SYMBOL(login, i)){
            return input_error;
        }
    }
    return OK;
}

void delete_enter(char * str){
    unsigned long int size = strlen(str);
    str[size - 1] = '\0';
}

errors free_users(Users* users){
    if (users == NULL) {
        return OK;
    }

    if (users->capacity > 0){
        free(users->data);
    }
    free(users);
}

errors register_user(Users* users, const char* login, int password, int sanctions) {
    if (users == NULL || login == NULL) {
        return ptr_error;
    }

    if (users->capacity == 0) {
        User * temp = (User  *) malloc(sizeof (User) * 2);
        if (!temp) {
            return memory_error;
        }
        users->data = temp;
        users->capacity = 2;
        users->size = 0;
    }

    if (password < 0 || password > 100000 || check_login(login)) {
        return input_error;
    }

    for(int i = 0; i < users->size; i++){
        if(strcmp(login, users->data[i].login) == 0) {
            return login_error;
        }
    }

    if (users->capacity == users->size) {
        User* tmp = (User*)realloc(users->data, users->capacity * 2 * sizeof(User));
        if (tmp == NULL) {
            return memory_error;
        }
        users->capacity *= 2 ;
        users->data = tmp;
    }
    strncpy(users->data[users->size].login, login, strlen(login));
    users->data[users->size].login[strlen(login)] = '\0';
    users->data[users->size].password = password;
    users->data[users->size].sanctions = sanctions;
    (users->size)++;
    return OK;
}

errors authorisation_user(Users* users, const char* login, int password, int sanctions) {
    if (users == NULL || login == NULL) {
        return ptr_error;
    }
    for (size_t i = 0; i < users->size; i++) {
        if (strcmp(users->data[i].login, login) == 0) {
            if (password == users->data[i].password) {
                return OK;
            } else {
                return authorisation_error;
            }
        }
    }
    return input_error;
}

errors find_user(Users* users, const char* login, int * id) {
    if (users == NULL || login == NULL) {
        return ptr_error;
    }
    for (size_t i = 0; i < users->size; i++) {
        if (strcmp(users->data[i].login, login) == 0) {
            *id = (int) i;
            return OK;
        }
    }
    return login_error;
}

void print_user(User user){
    printf("Login: %s\nPassword: %d\nSanctions: %d\n", user.login, user.password, user.sanctions);
}

void print_users(Users users){
    for (int i = 0; i < users.size; i++){
        print_user(users.data[i]);
    }
}

errors start_window(Users* users, char* login, int* password) {
    if (login == NULL || password == NULL || users == NULL) {
        return ptr_error;
    }
    errors (*commands[2])(Users* users, const char* login, int password, int sanctions) = {
        authorisation_user,
        register_user
    };
    while (1) {
        printf("Commands:\n1 - Authorisation\n2 - Registration\n3 - Exit\n");
        int com;
        char com_str[10];
        char * c = fgets(com_str, 10, stdin);
        if(c == 0) {
            return OK;
        }
        if (strcmp("Authorisation\n", com_str) == 0 || strcmp("1\n", com_str) == 0) {
            com = 1;
        } else if (strcmp("Registration\n", com_str) == 0 || strcmp("2\n", com_str) == 0) {
            com = 2;
        } else if (strcmp("Exit\n", com_str) == 0 || strcmp("3\n", com_str) == 0) {
            return end;
        }
        else {
            printf("Unrecognized command.\n");
            continue;
        }

        printf("Write login: ");
        int k = 0;
        while (scanf("%s", login) == 0 || check_login(login) || (k = ((com == 2) && (!find_user(users, login, &k))))) {
            if (k) {
                printf("This login is using bu another person, try another login:");
                continue;
            }
            printf("Incorrect login.\n");
            printf("Write login: ");
        }

        printf("Write password: ");
        while (scanf("%d", password) == 0 || *password < 0 || *password > 100000) {
            printf("Incorrect password.\n");
            destroy_buffer();
            printf("Write password: ");
        }

        errors error = commands[com - 1](users, login, *password, -1);
        switch (error) {
            case ptr_error:
                printf("Incorrect options\n");
                getc(stdin);
                destroy_buffer();
                break;
            case input_error:
                printf("Incorrect options\n");
                getc(stdin);
                break;
            case OK:
                return OK;
            case authorisation_error:
                printf("Incorrect password\n");
                getc(stdin);
                break;
            default:
                return OK;
        }
    }
}

errors sanctions(Users* users, char * login, int sanc) {
    if (login == NULL || users == NULL) {
        return ptr_error;
    }
    char * password = NULL;
    size_t len;
    printf("if you want to change selection you should write special code: ");
    getline(&password, &len, stdin);
    for (int i = 0; (i < 2) && (strcmp(password, "12345\n")); i++){
        printf("Incorrect password. Try again:");
        getline(&password, &len, stdin);
    }
    if (!strcmp(password, "12345\n")){
        int i = -1;
        for (int j = 0; j < users->size; j++) {
            if (!strcmp(users->data[j].login, login)) {
                i = j;
            }
        }
        if (i == -1) {
            printf("Incorrect Login\n");
            free(password);
            return login_error;
        }
        users->data[i].sanctions = sanc;
    } else {
        printf("You have mistake with password for 3 times");
        free(password);
        return password_error;
    }
    free(password);
    return OK;
}

void destroy_buffer() {
    char c;
    while ((c = getc(stdin)) != EOF && c != '\n');
}

errors get_time_string(char* output) {
    if (output == NULL) {
        return ptr_error;
    }
    time_t Time;
    struct tm* result_time;
    time(&Time);
    result_time = localtime(&Time);
    strftime(output, 9, "%H:%M:%S", result_time);
    return OK;
}

errors sanctions_limit_check(Users * users, char *login, int * amount_of_operations){
    int id;
    errors error = find_user(users, login, &id);
    if (error) {
        return error;
    }
    if ((users->data)[id].sanctions == -1){
        return OK;
    } else if (users->data[id].sanctions == *amount_of_operations){
        return sanctions_limit_error;
    } else {
        (*amount_of_operations)++;
        return OK;
    }
}

errors get_date_string(char* output) {
    if (output == NULL) {
        return ptr_error;
    }
    time_t Time;
    struct tm* result_time;
    time(&Time);
    result_time = localtime(&Time);
    strftime(output, 11, "%d/%m/%Y", result_time);
    return OK;
}

errors how_much_time(const char* input, const char* flag, double* res) {
    struct tm date = {0};
    time_t input_time;
    time_t current_time;
    double range;
    double result = -1;
    if (input == NULL || flag == NULL) {
        return input_error;
    }
    if (sscanf(input, "%d.%d.%d", &date.tm_mday, &date.tm_mon, &date.tm_year) != 3 ||
        (date.tm_mon > 12 || date.tm_mon < 0) || date.tm_year < 0 || date.tm_mday < 0 || date.tm_mday > 30) {
        return input_error;
    }
    date.tm_mon -= 1;
    date.tm_year -= 1900;
    if ((input_time = mktime(&date)) == -1) {
        return time_error;
    }
    time(&current_time);
    range = difftime(current_time, input_time);
    if (range < 0) {
        return time_error;
    }
    if (strcmp(flag, "-s\n") == 0) {
        result = range;
    } else if (strcmp(flag, "-m\n") == 0) {
        result = range / 60;
    } else if (strcmp(flag, "-h\n") == 0) {
        result = range / 3600;
    } else if (strcmp(flag, "-d\n") == 0) {
        result = range / 3600 / 24;
    } else if (strcmp(flag, "-y\n") == 0) {
        result = range / 60 / 60 / 24 / 365;
    } else {
        return flag_error;
    }
    *res = result;
    return OK;
}

void print_info(){
    printf("\nCommands\n1 - Time\n2 - Date\n3 - How much Time\n4 - Logout\n5 - Sanctions\n");
}

errors user_window (Users* users, char * login){
    char* command = NULL;
    char * timein = NULL;
    char * input_login = NULL;
    size_t len;
    int amount_of_operations = 0;
    while (1){
        getline(&command, &len, stdin);
        if (!strcmp(command, "\n")){
            print_info();
        } else if (!strcmp(command, "1\n")){
            if (sanctions_limit_check(users, login, &amount_of_operations)){
                printf("SANCTIONS. Limit of operations is imployed");
                print_info();
                continue;
            }
            char time[11];
            get_time_string(time);
            printf("Time now is: %s", time);
            print_info();
        } else if (!strcmp(command, "2\n")) {
            if (sanctions_limit_check(users, login, &amount_of_operations)){
                printf("SANCTIONS. Limit of operations is imployed");
                print_info();
                continue;
            }
            char time[11];
            get_date_string(time);
            printf("Today is: %s", time);
            print_info();
        } else if (!strcmp(command, "3\n")) {
            if (sanctions_limit_check(users, login, &amount_of_operations)){
                printf("SANCTIONS. Limit of operations is imployed");
                print_info();
                continue;
            }
            double res;
            printf("Write in Format <Day.Month.Year> Flag:\n");
            while (1){
                getline(&timein,&len, stdin);
                char delim[2] = " ";
                char * temp = strtok(timein, delim);
                char * flag = strtok(NULL, delim);
                errors error = how_much_time(temp, flag, &res);
                int p = 0;
                switch (error) {
                    case input_error:
                        printf("Incorrect format\nWrite in Format <Day.Month.Year> Flag:");
                        break;
                    case time_error:
                        printf("Incorrect time\nTry again:");
                        break;
                    case flag_error:
                        printf("Incorrect flag\nTry again (-s seconds, -m minutes, -h hours, -y years):");
                        break;
                    case OK:
                        p = 1;
                        break;
                }
                if (p){
                    printf("%f", res);
                    print_info();
                    break;
                }
            }
        } else if (!strcmp(command, "5\n")){
            if (sanctions_limit_check(users, login, &amount_of_operations)){
                printf("SANCTIONS. Limit of operations is imployed");
                print_info();
                continue;
            }
            printf("Write login of user sanctions:");
            getline(&input_login, &len, stdin);
            delete_enter(input_login);
            int id = 0;
            while (find_user(users, input_login, &id)){
                printf("No such user. Try another login:");
                getline(&input_login, &len, stdin);
                delete_enter(input_login);
            }
            int amount;
            printf("Write amount of limit:");
            getline(&timein, &len, stdin);
            while ((sscanf(timein, "%d\n", &amount) != 1) || (amount < 0)){
                printf("Write amount of limit:");
                getline(&timein, &len, stdin);
            }
            sanctions(users, input_login, amount);
            print_info();
        } else if (!strcmp(command, "4\n")){
            free(command);
            free(timein);
            free(input_login);
            return end;
        }
    }
};

errors dialog(Users* users, char* login, int win){
    while (1){
        if (win == 0) {
            int password = 123;
            errors error = start_window(users, login, &password);
            if (error){
                return error;
            } else {
                win = 1;
            }
        } else {
            if (user_window(users, login) == end){
                win = 0;
            }
            else {
                return ptr_error;
            }
        }
    }
}

errors load(const char* filename, Users* users) {
    if (filename == NULL || users == NULL) {
        return ptr_error;
    }
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        return file_error;
    }
    User user;
    errors error;
    while (fread(&user, 1, sizeof(User), f)) {
        error = register_user(users, user.login, user.password, user.sanctions);
        if (error) {
            fclose(f);
            return error;
        }
    }
    fclose(f);
    return OK;
}

//errors update(const char* filename, Users* users) {
//    if (!filename || !users) return ptr_error;
//    FILE* fin = fopen(filename, "wb");
//    if (!fin) return file_error;
//
//    for (size_t i = 0; i < users->size; ++i) {
//        fwrite(&users->data[i], users->size, sizeof(User), fin);
//    }
//    fclose(fin);
//    return 0;
//}

errors update(const char* filename, Users* users) {
    if (filename == NULL || users == NULL) {
        return ptr_error;
    }
    FILE* f = fopen(filename, "wb");
    if (f == NULL) {
        return file_error;
    }
    fwrite(users->data, users->size, sizeof(User), f);
    fclose(f);
    return OK;
}