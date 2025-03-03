#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

char **parse_command(char *input, int *argc);
void free_args(char **args);
void print_error(const char *message);

#endif // !UTILS_H
