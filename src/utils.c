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
};

Command *parse_command(char *input) {

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

  char *token;
  int i = 0;

  token = strtok(input, DELIMITERS);
  while (token != NULL) {
    if (strcmp(token, ">") == 0) {
      token = strtok(NULL, DELIMITERS);
      if (token == NULL) {
        print_error("Syntax error: Expected filename after >");
        free_command(cmd); // Important: Free allocated memory
        return NULL;       // Indicate an error
      }
      cmd->output_file = strdup(token);
      if (!cmd->output_file) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
      }
      break; // Stop parsing after redirection
    } else if (strcmp(token, "<") == 0) {
      token = strtok(NULL, DELIMITERS);
      if (token == NULL) {
        print_error("Syntax error: Expected filename after <");
        free_command(cmd); // Important: Free allocated memory
        return NULL;
      }
      cmd->input_file = strdup(token);
      if (!cmd->input_file) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
      }
      token = strtok(NULL, DELIMITERS); // Continue parsing arguments
    } else if (strcmp(token, ">>") == 0) {
      token = strtok(NULL, DELIMITERS);
      if (token == NULL) {
        print_error("Syntax error: Expected filename after >>");
        free_command(cmd); // Important: free allocated memory
        return NULL;
      }
      cmd->output_file = strdup(token);
      if (!cmd->output_file) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
      }
      cmd->append = 1;
      break; // Stop parsing after redirection

    } else {
      if (i < MAX_ARGS - 1) {
        cmd->args[i++] = strdup(token);
        if (!cmd->args[i - 1]) {
          perror("strdup failed");
          exit(EXIT_FAILURE);
        }
      } else {
        print_error("Too many arguments");
        free_command(cmd);
        return NULL;
      }
      token = strtok(NULL, DELIMITERS);
    }
  }
  cmd->args[i] = NULL; // Null-terminate
  cmd->argc = i;
  return cmd;
}

void free_command(Command *cmd) {
  if (cmd) {
    free_args(cmd->args);
    if (cmd->input_file)
      free(cmd->input_file);
    if (cmd->output_file)
      free(cmd->output_file);
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
  fprintf(stderr, "myshell: %s\n", message);
}
