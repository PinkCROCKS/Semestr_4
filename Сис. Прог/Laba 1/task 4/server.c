#include "server.h"

volatile sig_atomic_t keep_running = 1;

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);


    key_t token = ftok("./ss", 55);
    if (token < 0) {
        return print_error(file_error);
    }

    int msg_fd = msgget(token, IPC_CREAT | 0666);
    if (msg_fd < 0) {
        return print_error(message_queue_error);
    }

    struct msqid_ds info;
    msgctl(msg_fd, IPC_STAT, &info);
    info.msg_qnum = 0;
    msgctl(msg_fd, IPC_SET, &info);

    msg_buffer message;

    struct sockaddr_in address;

    socklen_t addrlen = sizeof(address);

    int opt = 1;
    int client_fd;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        msgctl(msg_fd, IPC_RMID, NULL);
        return print_error(socket_error);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        msgctl(msg_fd, IPC_RMID, NULL);
        close(msg_fd);
        return print_error(socket_error);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        close(server_fd);
        msgctl(msg_fd, IPC_RMID, NULL);
        return print_error(socket_error);
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        msgctl(msg_fd, IPC_RMID, NULL);
        return print_error(socket_error);
    }

    printf("Listening to commands!\n");
    size_t current_read;


    Game game = {0, EMPTY, {{1, 1, 1},
                            {0, 0, 0}}};


    while (keep_running) {
        client_fd = accept(server_fd, (struct sockaddr *) &address, &addrlen);
        if (client_fd < 0) {
            print_error(socket_error);
            continue;
        }

        printf("Client %d connected!\n", client_fd);

        message.client_fd = client_fd;
        message.msgtype = 1;

        if (msgsnd(msg_fd, &message, sizeof(message) - sizeof(long), 0) < 0) {
            print_error(message_queue_error);
            close(client_fd);
            continue;
        }

        current_read = msgrcv(msg_fd, &message, sizeof(message) - sizeof(long), 0, 0);
//        printf("%u %u %lu %d\n", message.command, message.argument, message.msgtype, message.client_fd);
        if (current_read < 1) {
            close(client_fd);

            continue;
        }

        if (message.client_fd != client_fd) {
            close(client_fd);
            continue;
        }

        int stop = 0;
        while (message.msgtype != 2) {  // While message type is not finishing
            if (!stop && process_command(message, &game)) {
                stop = 1;
            }

            current_read = msgrcv(msg_fd, &message, sizeof(message) - sizeof(long), 0, 0);
//            printf("%u %u %lu %d\n", message.command, message.argument, message.msgtype, message.client_fd);
            if (current_read < 1) {
                break;
            }

            if (message.client_fd != client_fd) {
                break;
            }
        }


        close(client_fd);
    }

    msgctl(msg_fd, IPC_RMID, NULL);
    close(server_fd);

    return 0;
}

void handle_signal() {
    keep_running = 0;
    printf("Server shut down!\n");
}

int check_conflicts(const int *inventory) {
    if (inventory[0] && inventory[1] && inventory[2]) {
        return 2;
    }
    if ((inventory[0] && inventory[1]) || (inventory[1] && inventory[2])){
        return 1;
    }

    return 0;
}

int check_stopping(int inventory[3][3]){
    if (inventory[1][0] && inventory[1][1] && inventory[1][2]){
        return 2;
    }
    for (int i = 0; i < 2; i++){
        if (inventory[i][0] && inventory[i][1]){
            return 1;
        }
        if (inventory[i][1] && inventory[i][2]){
            return 1;
        }
    }
    return 0;
}

void put_item(int *inventory, Item *item) {
    switch (*item) {
        case EMPTY:
            break;
        case WOLF:
            inventory[0] = 1;
            printf("Put down wolf!\n");
            break;
        case GOAT:
            inventory[1] = 1;
            printf("Put down goat!\n");
            break;
        case CABBAGE:
            inventory[2] = 1;
            printf("Put down cabbage!\n");
            break;
    }

    *item = EMPTY;
}

void print_status(int game_bank[3][3]){
    if (game_bank[0][0] && game_bank[0][1]){
        printf(" ");
        return;
    } if (game_bank[0][1] && game_bank[0][2]){
        printf(" ");
        return;
    } if (game_bank[1][0] && game_bank[1][1]){
        printf(" ");
        return;
    } if (game_bank[1][1] && game_bank[1][2]){
        printf(" ");
        return;
    }
}

int process_command(msg_buffer message, Game *game) {
    switch (message.command) {
        case TAKE:
            switch (message.argument) {
                case WOLF:
                    game->bank_inventories[game->bank][0] = 0;
                    game->current_item = WOLF;
                    //printf("Took wolf!\n");
                    break;
                case GOAT:
                    game->bank_inventories[game->bank][1] = 0;
                    game->current_item = GOAT;
                    //printf("Took goat!\n");
                    break;
                case CABBAGE:
                    game->bank_inventories[game->bank][2] = 0;
                    game->current_item = CABBAGE;
                    //printf("Took cabbage!\n");
                    break;
            }
            break;
        case PUT:
            put_item(game->bank_inventories[game->bank], &game->current_item);
            break;
        case MOVE:
            printf("\n");
            int state = check_stopping(game->bank_inventories);
            if (state) {
                if (state == 1) {
                    print_status(game->bank_inventories);
                    printf("Game is finished successfully!\n");
                } else {
                    printf("Game is finished successfully!\n");
                }
                reset_game(game);
                return 1;
            }
            game->bank = !game->bank;
            break;
        default:
            printf("Unknown command!\n");
            break;

    }
    return 0;
}

void reset_game(Game *game) {
    game->bank = 0;
    game->current_item = EMPTY;

    game->bank_inventories[0][0] = 1;
    game->bank_inventories[0][1] = 1;
    game->bank_inventories[0][2] = 1;

    game->bank_inventories[1][0] = 0;
    game->bank_inventories[1][1] = 0;
    game->bank_inventories[1][2] = 0;
}