#include "include/builtins.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int builtin_history(char **args) {
  (void)args;
  print_history(history, history_count);
  return 0;
}

int builtin_cd(char **args) {
  if (args[1] == NULL) {
    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
      print_error("HOME environment variable not set.");
      return 1;
    }
    if (chdir(home_dir) != 0) {
      perror("chdir faile");
      return 1;
    }
  } else {
    if (chdir(args[1]) != 0) {
      perror("chdir failed");
      return 1;
    }
  }
  return 0;
}

int builtin_exit(char **args) {
  (void)args;
  exit(0);
}

int builtin_help(char **args) {
  printf("cshell - A simple shell written in C\n");
  printf("Built-in commands:\n");
  printf("  cd <directory>   - Change the current working directory.\n");
  printf("  exit             - Exit the shell.\n");
  printf("  help             - Display this help message.\n");
  printf("  history          - Display command history.\n");
  printf("Other commands are executed as external programs.\n");
  return 1;
}

int executable_builtin(char **args, int argc) {
  (void)argc;
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  else if (strcmp(args[0], "exit") == 0)
    return builtin_exit(args);
  else if (strcmp(args[0], "help") == 0)
    return builtin_help(args);
  else if (strcmp(args[0], "history") == 0)
    return builtin_history(args);
  return -1;
}
