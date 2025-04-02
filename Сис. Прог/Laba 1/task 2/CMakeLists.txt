cmake_minimum_required(VERSION 3.28)
project(task_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(client client.c client.h "../task 1/errors.c")

add_executable(server server.c server.h "../task 1/errors.c")
