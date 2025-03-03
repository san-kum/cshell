#include "include/builtins.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int builtin_cd(char **args) {
  if (args[1] == NULL) {
    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
      print_error("HOME environment variable not set.");
      return 1;
    }
    if (chdir(args[1]) != 0) {
      perror("chdir failed");
      return 1;
    }
  }
  return 0;
}

int builtin_exit(char **args) { exit(0); }

int executable_builtin(char **args, int argc) {
  if (strcmp(args[0], "cd") == 0)
    return builtin_cd(args);
  else if (strcmp(args[0], "exit") == 0)
    return builtin_cd(args);
  return -1;
}
