#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

typedef struct {
  char **args;
  int argc;
  char *input_file;
  char *output_file;
  int append;
  char *next;
} Command;

Command *parse_command(char *input);
void free_command(Command *cmd);
void free_args(char **args);
void print_error(const char *message);

#endif // !UTILS_H
