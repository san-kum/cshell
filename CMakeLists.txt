cmake_minimum_required(VERSION 3.16)

project(cshell C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)


add_executable(cshell
    src/shell.c
    src/builtins.c
    src/utils.c
)

target_include_directories(cshell
    PRIVATE
        src/include
)

set_target_properties(cshell PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
