cmake_minimum_required(VERSION 3.28)
project(number_1)

set(CMAKE_CXX_STANDARD 20)

# Включение опции для покрытия кода
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Настройка GoogleTest
add_subdirectory(googletest)

# Основной проект
add_executable(number_2
        include/functions.hpp
        src/main.cpp
        include/container.hpp
)

include_directories(include)

# Тестовый проект
add_executable(tests
        tests/tests.cpp
        include/functions.hpp
        include/container.hpp
)

target_link_libraries(tests PRIVATE
        gtest
        gtest_main
)

# Настройка покрытия
if(ENABLE_COVERAGE)
    # Поиск необходимых утилит
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    find_program(GCOV_PATH gcov)

    if(LCOV_PATH AND GENHTML_PATH AND GCOV_PATH)
        # Добавляем флаги компилятора
        target_compile_options(tests PRIVATE
                -fprofile-arcs
                -ftest-coverage
                -fPIC
        )
        target_link_libraries(tests PRIVATE gcov)

        # Создаем кастомные цели
        add_custom_target(coverage
                # Запуск тестов
                COMMAND ../tests

                # Сбор данных
                COMMAND lcov --capture --directory . --output-file coverage.info

                # Удаление системных файлов
                COMMAND lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info

                # Генерация HTML отчета
                COMMAND genhtml coverage.info --output-directory coverage_report

                # Очистка
                COMMAND lcov --directory . --zerocounters

                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS tests
                COMMENT "Generating code coverage report"
        )
    else()
        message(WARNING "Code coverage tools not found!")
    endif()
endif()

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(cppcheck
            COMMAND ${CPPCHECK}
            --enable=all
            --inconclusive
            --quiet
            --suppress=missingIncludeSystem
            -I include
            src
            tests
            COMMENT "Запуск cppcheck..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )
else()
    message(WARNING "cppcheck не найден, цель 'cppcheck' недоступна.")
endif()