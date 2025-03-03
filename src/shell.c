#include "include/builtins.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024

int main() {
  char input[MAX_INPUT_SIZE];
  char **args;
  int argc;
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
    args = parse_command(input, &argc);

    if (argc > 0) {
      int builtin_result = executable_builtin(args, argc);
      if (builtin_result == -1) {
        pid_t pid = fork();

        if (pid == -1) {
          perror("fork failed");
          exit(EXIT_FAILURE);
        } else if (pid == 0) {
          if (execvp(args[0], args) == -1) {
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
    free_args(args);
  }
  return 0;
}
