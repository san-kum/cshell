#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 64
#define DELIMITERS " \t\r\n\a" // Whitespace delimeters

char **parse_command(char *input, int *argc) {
  char **args = malloc(MAX_ARGS * sizeof(char *));
  if (!args) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  char *token;
  int i = 0;

  token = strtok(input, DELIMITERS);
  while (token != NULL && i < MAX_ARGS - 1) {
    args[i++] = strdup(token);
    if (!args[i - 1]) {
      perror("strdup failed.");
      exit(EXIT_FAILURE);
    }
    token = strtok(NULL, DELIMITERS);
  }
  args[i] = NULL;
  *argc = i;
  return args;
}

void free_args(char **args) {
  if (args) {
    for (int i = 0; args[i] != NULL; i++)
      free(args[i]);
  }
}

void print_error(const char *message) {
  fprintf(stderr, "myshell: %s\n", message);
}
