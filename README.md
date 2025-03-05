# CShell: A Custom Unix-Like Shell Implementation

## Project Overview

CShell is a custom Unix-like shell implemented in C, designed to provide a robust and feature-rich command-line interface. This project demonstrates a deep understanding of system programming, process management, and shell functionality by implementing core shell behaviors from scratch.

## Features

### Command Execution

- Execute external commands using `execvp()`
- Support for complex command pipelines
- Input and output redirection
  - `>` for output redirection
  - `<` for input redirection
  - `>>` for append output redirection

### Built-in Commands

- `cd`: Change current working directory
- `exit`: Terminate the shell
- `help`: Display available commands and help information
- `history`: View command history

### Advanced Capabilities

- Command history with navigation (up/down arrow keys)
- Signal handling for `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z)
- Wildcard expansion using `glob()`
- Basic scripting support with control structures

## Technical Architecture

### Key Components

1. **Command Parsing** (`utils.c`)

   - Tokenizes input into command structures
   - Handles pipes, redirections, and argument parsing
   - Supports wildcard expansion

2. **History Management** (`history.c`)

   - Circular buffer for storing command history
   - Advanced input handling with history navigation
   - Supports retrieving and displaying past commands

3. **Built-in Commands** (`builtins.c`)

   - Implements shell-specific commands
   - Provides core shell functionality

4. **Scripting Support** (`scripting.c`)
   - Basic script parsing and execution
   - Supports control structures like `if`, `while`
   - Variable management within scripts

### Signal Handling

- `SIGINT`: Interrupt current foreground process
- `SIGCHLD`: Manage child process termination
- `SIGTSTP`: Stop foreground process

## Compilation and Running

### Prerequisites

- GCC or Clang compiler
- UNIX-like operating system (Linux, macOS)
- Make utility

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/san-kum/cshell.git
cd cshell

mkdir build && cd build

# Compile the project
cmake ..
make

# Run the shell
./build/build/cshell
```

## Testing

The project includes a comprehensive test suite (`tests.c`) covering:

- Command parsing
- Built-in command functionality
- History management
- Wildcard expansion
- Error handling

Run tests using:

```bash
./build/build/cshell_tests

# or use ctest
ctest
ctest -V # for verbose output
```

## Limitations and Future Improvements

- Enhanced scripting capabilities
- More robust error handling
- Support for environment variable expansion
- Advanced tab completion
- Multi-line command support

## Learning Objectives

- System programming in C
- Process management
- Unix shell internals
- Input/output redirection mechanisms
- Signal handling techniques

## Author

[https://github.com/san-kum]
