#include "include/utils.h"
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_ARGS 64
#define DELIMITERS " \t\r\n\a" // Whitespace delimeters

void reset_input_line(char *buffer, int *i) {
  printf("\n");
  printf("cshell> ");
  fflush(stdout);
  buffer[0] = '\0';
  *i = 0;
}

Command *parse_command(char *input) {
  Command *head = NULL;
  Command *tail = NULL;
  char *token;
  char *input_copy = strdup(input);

  if (!input_copy) {
    perror("strdup failed");
    exit(EXIT_FAILURE);
  }

  token = strtok(input_copy, DELIMITERS);

  if (token == NULL) {
    free(input_copy);
    return NULL;
  }
  while (token != NULL) {
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
    }
    cmd->args = malloc(MAX_ARGS * sizeof(char *));
    if (!cmd->args) {
      perror("malloc failed");
      free(cmd);
      free_command(head);
      free(input_copy);
      exit(EXIT_FAILURE);
    }
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;
    cmd->argc = 0;
    cmd->next = NULL;

    while (token != NULL) {
      if (strcmp(token, "|") == 0) {
        token = strtok(NULL, DELIMITERS);
        break;
      } else if (strcmp(token, ">") == 0) {
        token = strtok(NULL, DELIMITERS);
        if (token == NULL) {
          print_error("Syntax Error: Expected file name after >");
          free_command(cmd);
          free_command(head);
          free(input_copy);
          return NULL;
        }
        cmd->output_file = strdup(token);
        if (!cmd->output_file) {
          perror("strdup failed");
          // Clean up and exit.
          free_command(cmd);
          free_command(head);
          free(input_copy);
          exit(EXIT_FAILURE);
        }
        token = strtok(NULL, DELIMITERS);
        break; // Stop parsing arguments
      } else if (strcmp(token, "<") == 0) {
        token = strtok(NULL, DELIMITERS);
        if (token == NULL) {
          print_error("Syntax Error: Expected file name after <");
          free_command(cmd);
          free_command(head);
          free(input_copy);
          return NULL;
        }
        cmd->input_file = strdup(token);

        if (!cmd->input_file) {
          perror("strdup failed");
          // Clean up and exit.
          free_command(cmd);
          free_command(head);
          free(input_copy);
          exit(EXIT_FAILURE);
        }

        token = strtok(NULL, DELIMITERS); // Get next

      } else if (strcmp(token, ">>") == 0) {

        token = strtok(NULL, DELIMITERS);
        if (token == NULL) {
          print_error("Syntax error: Expected filename after >>");
          free_command(cmd);
          free_command(head);
          free(input_copy);
          return NULL;
        }

        cmd->output_file = strdup(token);

        if (!cmd->output_file) {
          perror("strdup failed");
          // Clean up and exit.
          free_command(cmd);
          free_command(head);
          free(input_copy);
          exit(EXIT_FAILURE);
        }
        cmd->append = 1;
        token = strtok(NULL, DELIMITERS);
        break;

      } else {
        char **expanded_args = expand_wildcards(token);
        if (expanded_args) {
          for (int i = 0; expanded_args[i] != NULL; i++) {
            if (cmd->argc < MAX_ARGS - 1) {
              cmd->args[cmd->argc++] = expanded_args[i];
            } else {
              print_error("Too many arguments");
              free_command(cmd);
              free_command(head);
              free(input_copy);
              return NULL;
            }
          }
          free(expanded_args);
        } else {
          free_command(cmd);
          free_command(head);
          free(input_copy);
          return NULL;
        }
        token = strtok(NULL, DELIMITERS);
      }
    }
    cmd->args[cmd->argc] = NULL;

    if (head == NULL) {
      head = cmd;
      tail = cmd;
    } else {
      tail->next = cmd;
      tail = cmd;
    }
  }

  free(input_copy);
  return head;
}

char **expand_wildcards(const char *arg) {
  glob_t glob_result;
  int flags = GLOB_NOCHECK | GLOB_TILDE;
  int ret = glob(arg, flags, NULL, &glob_result);

  if (ret != 0) {
    if (ret == GLOB_NOMATCH) {
      char **result = malloc(2 * sizeof(char *));
      if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
      }
      result[0] = strdup(arg);
      if (!result[0]) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
      }
      result[1] = NULL;
      return result;
    } else if (ret == GLOB_NOSPACE) {
      perror("glob faild: Out of memory");
      exit(EXIT_FAILURE);
    } else {
      fprintf(stderr, "glob error: %d\n", ret);
      return NULL;
    }
  }
  char **expanded_args = malloc((glob_result.gl_pathc + 1) * sizeof(char *));
  if (!expanded_args) {
    perror("malloc failed");
    globfree(&glob_result);
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < glob_result.gl_pathc; i++) {
    expanded_args[i] = strdup(glob_result.gl_pathv[i]);
    if (!expanded_args[i]) {
      perror("strdup failed");
      for (size_t j = 0; j < i; j++) {
        free(expanded_args[j]);
      }
      free(expanded_args);
      globfree(&glob_result);
      exit(EXIT_FAILURE);
    }
  }
  expanded_args[glob_result.gl_pathc] = NULL;

  globfree(&glob_result);
  return expanded_args;
}

void free_command(Command *cmd) {
  if (cmd) {
    free_args(cmd->args);
    if (cmd->input_file)
      free(cmd->input_file);
    if (cmd->output_file)
      free(cmd->output_file);
    if (cmd->next)
      free_command(cmd->next);
    free(cmd);
  }
}

void free_args(char **args) {
  if (args) {
    for (int i = 0; args[i] != NULL; i++)
      free(args[i]);
    free(args);
  }
}

void print_error(const char *message) {
  fprintf(stderr, "cshell: %s\n", message);
}
