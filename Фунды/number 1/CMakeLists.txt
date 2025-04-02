cmake_minimum_required(VERSION 3.28)
project(number_1)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(googletest)

add_executable(number_1 src/main.cpp
        src/functions.cpp
        include/functions.h)

include_directories(include)

add_executable(tests
        include/functions.h
        src/functions.cpp
        tests/tests.cpp
)

target_link_libraries(tests gtest)