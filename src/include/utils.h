#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define MAX_HISTORY_SIZE 100
#define MAX_INPUT_SIZE 1024

typedef struct Command Command;

struct Command {
  char **args;
  int argc;
  char *input_file;
  char *output_file;
  int append;
  Command *next;
};

Command *parse_command(char *input);
void free_command(Command *cmd);
void free_args(char **args);
void print_error(const char *message);
char **expand_wildcards(const char *arg);

#endif // !UTILS_H
