cmake_minimum_required(VERSION 3.28)
project(task_2)

set(CMAKE_CXX_STANDARD 17)

add_executable(task_2 main.cpp
        generator.hpp
        ts_queue.hpp
        generator.cpp
        analyzer.cpp
        analyzer.h
        analyzer.h
        utils.h
        loger.cpp
        loger.h)
