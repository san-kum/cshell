cmake_minimum_required(VERSION 3.16)

project(cshell C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Build the shell executable
add_executable(cshell
    src/shell.c
    src/builtins.c
    src/utils.c
    src/history.c
)

target_include_directories(cshell
    PRIVATE
        src/include
)

set_target_properties(cshell PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Build the test executable
add_executable(cshell_tests
    src/tests.c
    src/builtins.c
    src/utils.c
    src/history.c
)

target_include_directories(cshell_tests
    PRIVATE
        src/include
)

set_target_properties(cshell_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Add tests
add_test(NAME cshell_tests COMMAND ${CMAKE_BINARY_DIR}/build/cshell_tests)

enable_testing() # Enable ctest command

