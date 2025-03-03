#include "include/builtins.h"
#include "include/utils.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void create_test_file(const char *filename, const char *content) {
  FILE *fp = fopen(filename, "w");
  if (fp == NULL) {
    perror("failed to create test file");
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "%s\n", content);
  fclose(fp);
}

void test_parse_simple_command() {
  Command *cmd = parse_command("ls -l");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->argc == 2);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file == NULL);
  assert(cmd->append == 0);
  assert(cmd->next == NULL);
  free_command(cmd);
  printf("test_parse_simple_command: PASSED\n");
}

void test_parse_input_redirection() {
  Command *cmd = parse_command("< input.txt ls -l");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->input_file != NULL);
  assert(strcmp(cmd->input_file, "input.txt") == 0);
  assert(cmd->output_file == NULL);
  free_command(cmd);
  printf("test_parse_input_redirection: PASSED\n");
}

void test_parse_output_redirection() {
  Command *cmd = parse_command("ls -l > output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file != NULL);
  assert(strcmp(cmd->output_file, "output.txt") == 0);
  assert(cmd->append == 0);
  free_command(cmd);
  printf("test_parse_output_redirection: PASSED\n");
}

void test_parse_append_redirection() {
  Command *cmd = parse_command("ls -l >> output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file != NULL);
  assert(strcmp(cmd->output_file, "output.txt") == 0);
  assert(cmd->append == 1);
  free_command(cmd);
  printf("test_parse_append_redirection: PASSED\n");
}

void test_parse_pipe() {
  Command *cmd = parse_command("ls -l | wc -l");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file == NULL);
  assert(cmd->next != NULL);

  Command *next_cmd = cmd->next;
  assert(strcmp(next_cmd->args[0], "wc") == 0);
  assert(strcmp(next_cmd->args[1], "-l") == 0);
  assert(next_cmd->args[2] == NULL);
  assert(next_cmd->input_file == NULL);
  assert(next_cmd->output_file == NULL);
  assert(next_cmd->next == NULL);

  free_command(cmd);
  printf("test_parse_pipe: PASSED\n");
}

void test_parse_multiple_pipes() {
  Command *cmd = parse_command("cat input.txt | grep hello | wc -l");
  assert(cmd != NULL);

  assert(strcmp(cmd->args[0], "cat") == 0);
  assert(strcmp(cmd->args[1], "input.txt") == 0);
  assert(cmd->next != NULL);

  cmd = cmd->next;
  assert(strcmp(cmd->args[0], "grep") == 0);
  assert(strcmp(cmd->args[1], "hello") == 0);
  assert(cmd->next != NULL);

  cmd = cmd->next;
  assert(strcmp(cmd->args[0], "wc") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->next == NULL);

  free_command(cmd);
  printf("test_parse_multiple_pipes: PASSED\n");
}

void test_parse_redirection_and_pipe() {
  Command *cmd = parse_command("< input.txt cat | wc -l > output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "cat") == 0);
  assert(cmd->args[1] == NULL);
  assert(cmd->input_file != NULL);
  assert(strcmp(cmd->input_file, "input.txt") == 0);
  assert(cmd->output_file == NULL);
  assert(cmd->next != NULL);

  Command *next_cmd = cmd->next;
  assert(strcmp(next_cmd->args[0], "wc") == 0);
  assert(strcmp(next_cmd->args[1], "-l") == 0);
  assert(next_cmd->args[2] == NULL);
  assert(next_cmd->input_file == NULL);
  assert(next_cmd->output_file != NULL);
  assert(strcmp(next_cmd->output_file, "output.txt") == 0);
  assert(next_cmd->append == 0);
  assert(next_cmd->next == NULL);

  free_command(cmd);
  printf("test_parse_redirection_and_pipe: PASSED\n");
}

void test_parse_empty_command() {
  Command *cmd = parse_command("");
  assert(cmd == NULL);
  printf("test_parse_empty_command: PASSED\n");
}

void test_parse_command_with_extra_spaces() {
  Command *cmd = parse_command("  ls    -l   ");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->args[2] == NULL);
  assert(cmd->argc == 2);
  free_command(cmd);
  printf("test_parse_command_with_extra_spaces: PASSED\n");
}

void test_parse_invalid_redirection() {
  Command *cmd = parse_command("ls -l >");
  assert(cmd == NULL);

  cmd = parse_command("ls -l <");
  assert(cmd == NULL);

  cmd = parse_command("ls -l >>");
  assert(cmd == NULL);

  printf("test_parse_invalid_redirection: PASSED\n");
}

// --- Test Cases for built-in commands ---
void test_builtin_cd() {
  // Save current directory
  char original_dir[1024];
  getcwd(original_dir, sizeof(original_dir));

  // Test changing to a different directory
  char *args1[] = {"cd", "/tmp", NULL};
  int result1 = builtin_cd(args1);
  assert(result1 == 0);
  char current_dir1[1024];
  getcwd(current_dir1, sizeof(current_dir1));
  assert(strcmp(current_dir1, "/tmp") == 0);

  // Test changing back to the original directory
  char *args2[] = {"cd", original_dir, NULL};
  int result2 = builtin_cd(args2);
  assert(result2 == 0);
  char current_dir2[1024];
  getcwd(current_dir2, sizeof(current_dir2));
  assert(strcmp(current_dir2, original_dir) == 0);

  // Test cd with no arguments (should go to HOME)
  char *home_dir = getenv("HOME");
  if (home_dir) { // Only run test if HOME is set.
    char *args3[] = {"cd", NULL};
    int result3 = builtin_cd(args3);
    assert(result3 == 0);
    char current_dir3[1024];
    getcwd(current_dir3, sizeof(current_dir3));
    assert(strcmp(current_dir3, home_dir) == 0);

    // Restore the original directory.
    chdir(original_dir);
  }

  // Test cd with invalid directory
  char *args4[] = {"cd", "/nonexistentdirectory", NULL};
  int result4 = builtin_cd(args4);
  assert(result4 != 0); // Should return an error.

  printf("test_builtin_cd: PASSED\n");
}

void test_builtin_exit() {
  // This test won't actually cause the test suite to exit. It only
  //  checks if the function is available.
  char *args[] = {"exit", NULL};
  int result = builtin_exit(args); // No good way to test exit() directly
  assert(
      result ==
      0); // We are assuming exit always returns 0, just for testing purposes.
  printf("test_builtin_exit: PASSED (but doesn't fully test exit)\n");
}

void test_builtin_help() {
  char *args[] = {"help", NULL};
  int result = builtin_help(args);
  assert(result == 0); // Should return 0
  printf("test_builtin_help: PASSED\n");
}

// --- Test Cases for execute_builtin ---
void test_execute_builtin() {
  char *args_cd[] = {"cd", "/tmp", NULL};
  char *args_exit[] = {"exit", NULL};
  char *args_help[] = {"help", NULL};
  char *args_unknown[] = {"unknown_command", NULL};

  // Save current directory
  char original_dir[1024];
  getcwd(original_dir, sizeof(original_dir));

  int result_cd = executable_builtin(args_cd, 2);
  assert(result_cd == 0);
  char current_dir[1024];
  getcwd(current_dir, sizeof(current_dir));
  assert(strcmp(current_dir, "/tmp") == 0);

  chdir(original_dir);

  int result_exit = executable_builtin(args_exit, 1);
  assert(result_exit == 0);

  int result_help = executable_builtin(args_help, 1);
  assert(result_help == 0);

  int result_unknown = executable_builtin(args_unknown, 1);
  assert(result_unknown == -1);

  printf("test_execute_builtin: PASSED\n");
}

int main() {
  // Parsing tests
  test_parse_simple_command();
  test_parse_input_redirection();
  test_parse_output_redirection();
  test_parse_append_redirection();
  test_parse_pipe();
  test_parse_multiple_pipes();
  test_parse_redirection_and_pipe();
  test_parse_empty_command();
  test_parse_command_with_extra_spaces();
  test_parse_invalid_redirection();

  // Built-in tests
  test_builtin_cd();
  test_builtin_exit(); // Limited test
  test_builtin_help();

  // execute_builtin tests
  test_execute_builtin();

  printf("All tests finished.\n");
  return 0;
}
