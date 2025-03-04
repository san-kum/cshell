#include "include/builtins.h"
#include "include/utils.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024

char history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
int history_count = 0;

void sigint_handler(int signo) {
  (void)signo;
  printf("\n");
  fflush(stdout);
}

int main() {
  char input[MAX_INPUT_SIZE];
  Command *cmd;
  int status;

  // --- Command History ---
  int current_history_index = 0;

  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    perror("signal failed");
    exit(EXIT_FAILURE);
  }

  while (1) {
    printf("cshell> ");
    fflush(stdout);
    if (get_input(input, history, &history_count, &current_history_index) ==
            0 &&
        feof(stdin)) {
      printf("\n");
      break;
    }
    if (strlen(input) > 0) {
      add_to_history(input, history, &history_count, &current_history_index);
    }

    cmd = parse_command(input);
    if (!cmd)
      continue;

    Command *current = cmd;
    int input_fd = 0;

    while (current != NULL) {
      int pipefd[2];
      if (current->next != NULL) {
        if (pipe(pipefd) == -1) {
          perror("pipe failed");
          exit(EXIT_FAILURE);
        }
      }

      if (current == cmd &&
          executable_builtin(current->args, current->argc) != -1) {
        current = current->next;
        continue;
      }
      pid_t pid = fork();

      if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
      } else if (pid == 0) {

        if (current->input_file) {
          int fd = open(current->input_file, O_RDONLY);
          if (fd == -1) {
            perror("open failed");
            exit(EXIT_FAILURE);
          }
          dup2(fd, STDIN_FILENO);
          close(fd);
        } else if (input_fd != 0) { // If not the first command
          dup2(input_fd, STDIN_FILENO);
        }

        if (current->output_file) {
          int flags = O_WRONLY | O_CREAT;
          flags |= (current->append) ? O_APPEND : O_TRUNC;
          int fd = open(current->output_file, flags, 0644);
          if (fd == -1) {
            perror("open failed");
            exit(EXIT_FAILURE);
          }
          dup2(fd, STDOUT_FILENO);
          close(fd);
        } else if (current->next != NULL) {
          dup2(pipefd[1], STDOUT_FILENO);
        }

        // Close all pipe ends in the child
        if (current->next != NULL) {
          close(pipefd[0]);
          close(pipefd[1]);
        }
        if (input_fd != 0) {
          close(input_fd);
        }

        if (execvp(current->args[0], current->args) == -1) {
          perror("execvp failed");
          exit(EXIT_FAILURE);
        }

      } else {
        if (input_fd != 0) {
          close(input_fd);
        }

        if (current->next != NULL) {
          close(pipefd[1]);
          input_fd = pipefd[0];
        }
        current = current->next;
      }
    }

    while (wait(NULL) > 0)
      ;

    free_command(cmd); // Free the entire command list
  }

  return 0;
}
