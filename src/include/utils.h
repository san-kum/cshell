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

extern char history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
extern int history_count;

Command *parse_command(char *input);
void free_command(Command *cmd);
void free_args(char **args);
void print_error(const char *message);

// Command history
void add_to_history(char *command, char history[][MAX_INPUT_SIZE],
                    int *history_count, int *current_history_index);
void print_history(char history[][MAX_INPUT_SIZE], int history_count);
char *get_history_entry(char history[][MAX_INPUT_SIZE], int history_count,
                        int index);
int get_input(char *buffer, char history[][MAX_INPUT_SIZE], int *history_count,
              int *current_history_index);

#endif // !UTILS_H
