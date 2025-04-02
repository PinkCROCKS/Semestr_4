cmake_minimum_required(VERSION 3.28)
project(task_6 C)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c server_utility.c server_utility.h "../task 1/errors.c")


add_executable(client client.c client.h
        utility.c
        utility.h "../task 1/errors.c")


