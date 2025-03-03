#ifndef BUILTINS_H
#define BUILTINS_H

int builtin_cd(char **args);
int builtin_exit(char **args);
int builtin_help(char **args);
int executable_builtin(char **args, int argc);

#endif // !BUILTINS_H
