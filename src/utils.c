#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 64
#define DELIMITERS " \t\r\n\a" // Whitespace delimeters

struct Command {
  char **args;
  int argc;
  char *input_file;
  char *output_file;
  int append;
  int next;
};

Command *parse_command(char *input) {
  Command *head = NULL;
  Command *tail = NULL;
  char *token;
  char *input_copy = strdup(input);

  if (!input_copy) {
    perror("strdup failed");
    exit(EXIT_FAILURE);
  }

  Command *cmd = malloc(sizeof(Command));
  if (!cmd) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  cmd->args = malloc(MAX_ARGS * sizeof(char *));
  if (!cmd->args) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  token = strtok(input_copy, DELIMITERS);
  while (token != NULL) {
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
    }
    cmd->args = malloc(MAX_ARGS * sizeof(char *));
    if (!cmd->args) {
      perror("malloc failed");
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
          exit(EXIT_FAILURE);
        }
        token = strtok(NULL, DELIMITERS);
        break;
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
          exit(EXIT_FAILURE); // Or handle memory cleanup before exit
        }
        token = strtok(NULL, DELIMITERS);
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
          exit(EXIT_FAILURE); // Or handle memory cleanup before exit
        }
        cmd->append = 1;
        token = strtok(NULL, DELIMITERS);
        break;
      } else {
        if (cmd->argc < MAX_ARGS - 1) {
          cmd->args[cmd->argc++] = strdup(token);
          if (!cmd->args[cmd->argc - 1]) {
            perror("strdup failed");
            exit(EXIT_FAILURE);
          }
        } else {
          print_error("Too many arguments");
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
  return cmd;
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
