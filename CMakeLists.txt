cmake_minimum_required(VERSION 3.16)

project(cshell C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

find_program(VALGRIND_EXECUTABLE valgrind)
if(NOT VALGRIND_EXECUTABLE)
    message(WARNING "Valgrind not found. Memory check will not be performed.")
endif()


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

if(VALGRIND_EXECUTABLE)
    add_test(NAME cshell_tests COMMAND ${CMAKE_BINARY_DIR}/build/cshell_tests)
    set_tests_properties(cshell_tests PROPERTIES
        ENVIRONMENT "LD_PRELOAD=${VALGRIND_EXECUTABLE}"  # Sometimes needed
        RUN_SERIAL TRUE  # Run tests one at a time (important for Valgrind)
    )
    set(CTEST_MEMORYCHECK_COMMAND "${VALGRIND_EXECUTABLE}")  # Explicitly set memory checker.
    set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all --track-origins=yes")
endif()

enable_testing()
