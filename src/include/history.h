#ifndef HISTORY_H
#define HISTORY_H
#include "utils.h"

extern char history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
extern int history_count;

void add_to_history(char *command, char history[][MAX_INPUT_SIZE],
                    int *history_count, int *current_history_index);
void print_history(char history[][MAX_INPUT_SIZE], int history_count);
char *get_history_entry(char history[][MAX_INPUT_SIZE], int history_count,
                        int index);
int get_input(char *buffer, char history[][MAX_INPUT_SIZE], int *history_count,
              int *current_history_index);
#endif // !HISTORY_H
