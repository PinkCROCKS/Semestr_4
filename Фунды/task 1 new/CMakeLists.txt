cmake_minimum_required(VERSION 3.28)
project(task_1_new)

set(CMAKE_CXX_STANDARD 23)
set(COMMON_FLAGS "-Wall -Wextra -Wpedantic -Werror -fsanitize=address -fsanitize=leak")

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with Cppcheck" OFF)

# Добавление Cppcheck
if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        add_custom_target(cppcheck ALL
                COMMAND ${CPPCHECK}
                --enable=all
                --suppress=missingInclude
                --std=c++23
                --inline-suppr
                -I ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_SOURCE_DIR}/src/*.cpp
                ${CMAKE_SOURCE_DIR}/tests/*.cpp
                COMMENT "Running Cppcheck..."
        )
    else()
        message(WARNING "Cppcheck not found!")
    endif()
endif()

add_subdirectory(googletest)

add_executable(task_1_new src/main.cpp
        include/functions.h
        src/functions.cpp
)

include_directories(include)

add_executable(tests include/functions.h src/functions.cpp tests/tests.cpp)

target_link_libraries(tests gtest)

if(ENABLE_COVERAGE)
    # Поиск необходимых утилит
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        # Добавляем флаги компилятора
        target_compile_options(tests PRIVATE --coverage -O0 -g)
        target_link_libraries(tests PRIVATE --coverage)

        # Пользовательские цели для coverage
        add_custom_target(coverage
                COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
                COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage-report
                COMMAND xdg-open coverage-report/index.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS tests
                COMMENT "Generating coverage report..."
        )
    else()
        message(WARNING "lcov/genhtml not found! Coverage targets disabled")
    endif()
endif()