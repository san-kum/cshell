#include "include/builtins.h"
#include "include/utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024

int main() {
  char input[MAX_INPUT_SIZE];
  Command *cmd;
  int status;

  while (1) {
    printf("cshell> ");
    fflush(stdout);

    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
      if (feof(stdin)) {
        printf("\n");
        break;
      } else {
        perror("fgets failed");
        exit(EXIT_FAILURE);
      }
    }
    cmd = parse_command(input);
    if (cmd->argc > 0) {
      int builtin_result = executable_builtin(cmd->args, cmd->argc);
      if (builtin_result == -1) {
        pid_t pid = fork();

        if (pid == -1) {
          perror("fork failed");
          exit(EXIT_FAILURE);
        } else if (pid == 0) {
          if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd == -1) {
              perror("open failed");
              exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
          }
          if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT;
            flags |= (cmd->append) ? O_APPEND : O_TRUNC;
            int fd = open(cmd->output_file, flags, 0644);
            if (fd == -1) {
              perror("open failed");
              exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
          }
          if (execvp(cmd->args[0], cmd->args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
          }
        } else {
          if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
          }
        }
      }
    }
    free_command(cmd);
  }
  return 0;
}
