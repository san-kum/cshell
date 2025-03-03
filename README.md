# cshell - A Simple C Shell

`cshell` is a simple command-line shell written in C. It demonstrates fundamental concepts of operating systems, including process creation, input/output redirection, and basic command parsing. It is intended as an educational project and is not a full-featured replacement for shells like `bash` or `zsh`.

## Features

- **Execution of External Commands:** `cshell` can execute external programs found in the system's `PATH`.
- **Built-in Commands:**
  - `cd <directory>`: Changes the current working directory. If no directory is specified, it changes to the home directory (`$HOME`).
  - `exit`: Terminates the shell.
  - `help`: Displays a brief help message listing the available built-in commands.
- **Input/Output Redirection:**
  - `command > file`: Redirects the standard output of `command` to `file`, overwriting it if it exists or creating it if it doesn't.
  - `command < file`: Redirects the standard input of `command` to come from `file`.
  - `command >> file`: Redirects the standard output of `command` to `file`, appending to it if it exists or creating it if it doesn't.
- **Basic Error Handling:** The shell provides basic error messages for common issues like invalid commands, failed `fork`, `execvp`, `open`, `chdir` and file operations.
- **Empty input handling**: Shell handles empty lines gracefully.

## Usage

1.  **Compilation:**

    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

    This will create an executable named `cshell` inside a `build` directory.

2.  **Running the Shell:**

    ```bash
    ./build/cshell
    ```

3.  **Entering Commands:**

    At the `cshell>` prompt, type a command and its arguments, then press Enter.

    **Examples:**

    ```bash
    cshell> ls -l
    cshell> pwd
    cshell> cd /tmp
    cshell> echo "Hello, world!" > greeting.txt
    cshell> cat < greeting.txt
    cshell> date >> log.txt
    cshell> help
    cshell> exit
    ```

## Project Structure

The project is organized into the following files and directories:

- **`src/`:** Contains the source code.
  - `myshell.c`: The main shell program, including the main loop, command execution logic, and process management.
  - `builtins.c`: Implementation of built-in commands (`cd`, `exit`, `help`).
  - `utils.c`: Utility functions for parsing commands and handling errors.
  - `include/`: Contains header files.
    - `builtins.h`: Declarations for built-in functions.
    - `utils.h`: Declarations for utility functions.
- **`build/`:** Directory for storing compiled object files and the final executable (created by `make`).
- **`CMakeLists.txt`:** CMake configuration file for building the project.
- **`README.md`:** This documentation file.
- **`LICENSE`**: License file.

## Design

### 1. Main Loop (`shell.c`)

The shell operates in a continuous loop:

1.  **Display Prompt:** Prints `cshell> ` to the console.
2.  **Read Input:** Reads a line of input from the user using `fgets()`.
3.  **Parse Input:** The `parse_command()` function (in `utils.c`) splits the input line into a command and its arguments. It also handles redirection (`>`, `<`, `>>`) by identifying the redirection operators and associated filenames. The result is a `Command` struct.
4.  **Built-in Command Check:** The `execute_builtin()` function (in `builtins.c`) checks if the command is a built-in command (`cd`, `exit`, or `help`). If so, the corresponding function is executed directly within the shell process.
5.  **External Command Execution:** If the command is _not_ a built-in:

    - **Fork:** The shell creates a child process using `fork()`.
    - **Redirection (Child Process):**
      - If input redirection (`<`) is specified, the child process opens the input file using `open()` and uses `dup2()` to redirect standard input (file descriptor 0) to the opened file.
      - If output redirection (`>` or `>>`) is specified, the child process opens the output file (with appropriate flags for overwriting or appending) and uses `dup2()` to redirect standard output (file descriptor 1) to the opened file.
      - The opened file descriptors from `open()` are closed using `close()` _after_ being duplicated.
    - **Exec:** The child process uses `execvp()` to replace its process image with the specified external command.
    - **Wait (Parent Process):** The parent process (the shell) waits for the child process to complete using `waitpid()`.

6.  **Free Memory:** The memory allocated for parsing is released with the call to `free_command`.

### 2. Built-in Commands (`builtins.c`)

Built-in commands are implemented as functions that directly modify the shell's state (e.g., `cd` changes the current working directory using `chdir()`). They are executed within the main shell process, _not_ in a child process.

### 3. Utility Functions (`utils.c`)

- **`parse_command(char* input)`:** Parses the input string, handles redirection, and returns a `Command` structure containing the command, arguments, and redirection information.
- **`free_command(Command* cmd)`:** Deallocates all the memory allocated for `Command` struct and its members.
- **`free_args(char** args)`: Frees the memory allocated for an array of strings. This function is reused by the `free_command`.
- **`print_error(const char* message)`:** Prints an error message to standard error.

### 4. Data Structures

- **`Command` (in `utils.c`):**
  ```c
  typedef struct {
      char** args;      // Array of arguments
      int argc;        // Number of arguments
      char* input_file;  // Input redirection file (NULL if none)
      char* output_file; // Output redirection file (NULL if none)
      int append;      // 1 if output should be appended (>>), 0 otherwise
  } Command;
  ```
  This structure encapsulates all information about a single command.

