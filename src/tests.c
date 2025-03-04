#include "include/builtins.h"
#include "include/history.h"
#include "include/utils.h"
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// --- Helper functions for tests ----

// --- Test Cases for parse_command ---

void test_parse_simple_command() {
  Command *cmd = parse_command("ls");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(cmd->argc == 1);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file == NULL);
  assert(cmd->append == 0);
  assert(cmd->next == NULL);
  printf("test_parse_simple_command: Passed\n");
}

void test_parse_command_with_args() {
  Command *cmd = parse_command("ls -l -a");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(strcmp(cmd->args[2], "-a") == 0);
  assert(cmd->argc == 3);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file == NULL);
  assert(cmd->next == NULL);
  printf("test_parse_command_with_args: Passed\n");
}

void test_parse_input_redirection() {
  Command *cmd = parse_command("sort < input.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "sort") == 0);
  assert(cmd->argc == 1);
  assert(strcmp(cmd->input_file, "input.txt") == 0);
  assert(cmd->output_file == NULL);
  assert(cmd->next == NULL);
  printf("test_parse_input_redirection: Passed\n");
}

void test_parse_output_redirection() {
  Command *cmd = parse_command("ls > output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(cmd->argc == 1);
  assert(cmd->input_file == NULL);
  assert(strcmp(cmd->output_file, "output.txt") == 0);
  assert(cmd->append == 0);
  assert(cmd->next == NULL);
  printf("test_parse_output_redirection: Passed\n");
}

void test_parse_append_redirection() {
  Command *cmd = parse_command("echo hello >> output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "echo") == 0);
  assert(strcmp(cmd->args[1], "hello") == 0);
  assert(cmd->argc == 2);
  assert(cmd->input_file == NULL);
  assert(strcmp(cmd->output_file, "output.txt") == 0);
  assert(cmd->append == 1);
  assert(cmd->next == NULL);
  printf("test_parse_append_redirection: Passed\n");
}

void test_parse_pipe() {
  Command *cmd = parse_command("ls -l | grep foo");
  assert(cmd != NULL);

  assert(strcmp(cmd->args[0], "ls") == 0);
  assert(strcmp(cmd->args[1], "-l") == 0);
  assert(cmd->argc == 2);
  assert(cmd->input_file == NULL);
  assert(cmd->output_file == NULL);
  assert(cmd->next != NULL);

  Command *next_cmd = cmd->next;
  assert(strcmp(next_cmd->args[0], "grep") == 0);
  assert(strcmp(next_cmd->args[1], "foo") == 0);
  assert(next_cmd->argc == 2);
  assert(next_cmd->input_file == NULL);
  assert(next_cmd->output_file == NULL);
  assert(next_cmd->next == NULL);

  printf("test_parse_pipe: Passed\n");
}

void test_parse_empty_command() {
  Command *cmd = parse_command("");
  assert(cmd == NULL);

  cmd = parse_command("   "); // Only whitespace
  assert(cmd == NULL);
  printf("test_parse_empty_command: Passed\n");
}

void test_parse_multiple_pipes() {
  Command *cmd = parse_command("cat file.txt | grep error | wc -l");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "cat") == 0);
  assert(cmd->argc == 2);
  assert(cmd->next != NULL);

  Command *cmd2 = cmd->next;
  assert(strcmp(cmd2->args[0], "grep") == 0);
  assert(cmd2->argc == 2);
  assert(cmd2->next != NULL);

  Command *cmd3 = cmd2->next;
  assert(strcmp(cmd3->args[0], "wc") == 0);
  assert(cmd3->argc == 2);
  assert(cmd3->next == NULL);
  printf("test_parse_multiple_pipes: Passed\n");
}

void test_parse_combined_redirection_and_pipe() {
  Command *cmd = parse_command("cat < input.txt | grep error > output.txt");
  assert(cmd != NULL);
  assert(strcmp(cmd->args[0], "cat") == 0);
  assert(strcmp(cmd->input_file, "input.txt") == 0);
  assert(cmd->next != NULL);

  Command *cmd2 = cmd->next;
  assert(strcmp(cmd2->args[0], "grep") == 0);
  assert(strcmp(cmd2->args[1], "error") == 0);
  assert(strcmp(cmd2->output_file, "output.txt") == 0);
  assert(cmd2->next == NULL);

  printf("test_parse_combined_redirection_and_pipe: Passed\n");
}

void test_parse_error_handling() {
  // missing argument
  Command *cmd = parse_command(">\n");
  assert(cmd == NULL);

  cmd = parse_command("<\n");
  assert(cmd == NULL);
  assert(cmd == NULL);
  printf("test_parse_error_handling: Passed\n");
}
// --- Test Cases for add_to_history and get_history_entry ---

void test_history_add_and_get() {
  char test_history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
  int test_history_count = 0;
  int current_index = 0;

  add_to_history("command1", test_history, &test_history_count, &current_index);
  add_to_history("command2", test_history, &test_history_count, &current_index);

  assert(strcmp(get_history_entry(test_history, test_history_count, 0),
                "command1") == 0);
  assert(strcmp(get_history_entry(test_history, test_history_count, 1),
                "command2") == 0);
  assert(get_history_entry(test_history, test_history_count, 2) ==
         NULL); // Out of bounds
  printf("test_history_add_and_get: Passed\n");
}

void test_history_circular_buffer() {
  char test_history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
  int test_history_count = 0;
  int current_index = 0;

  // Fill the history
  for (int i = 0; i < MAX_HISTORY_SIZE + 5; i++) {
    char command[20];
    sprintf(command, "command%d", i);
    add_to_history(command, test_history, &test_history_count, &current_index);
  }
  // Check that the oldest entries are overwritten
  assert(strcmp(get_history_entry(test_history, test_history_count,
                                  test_history_count - MAX_HISTORY_SIZE),
                "command5") == 0);
  assert(strcmp(get_history_entry(test_history, test_history_count,
                                  test_history_count - 1),
                "command104") == 0);
  printf("test_history_circular_buffer: Passed\n");
}

// --- Test Cases for get_input (Basic) ---
// Full terminal input testing is difficult in a unit test.

void test_get_input_basic() {
  char buffer[MAX_INPUT_SIZE];
  char test_history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE]; // Dummy history.
  int test_history_count = 0;
  int current_history_index = 0;

  // Simulate typing "hello\n" into stdin
  FILE *input_stream = fmemopen("hello\n", 6, "r");
  stdin = input_stream; // Redirect stdin

  int input_length = get_input(buffer, test_history, &test_history_count,
                               &current_history_index);
  assert(strcmp(buffer, "hello\n") == 0);
  assert(input_length == 6);

  fclose(input_stream);

  // Restore
  freopen("/dev/tty", "r", stdin);
  printf("test_get_input_basic: Passed\n");
}

void test_builtin_cd() {
  char *original_dir = getcwd(NULL, 0); // Get the current working directory
  assert(original_dir != NULL);

  // Test changing to a valid directory
  char *test_dir_name = "test_dir";
  mkdtemp(test_dir_name); // create a temporal directory.

  char *args1[] = {"cd", test_dir_name, NULL};
  assert(builtin_cd(args1) == 0);

  char *new_dir = getcwd(NULL, 0);
  assert(new_dir != NULL);

  char full_test_dir_path[PATH_MAX];

  // Resolve the real path, to ensure testing when using relative paths.
  realpath(test_dir_name, full_test_dir_path);
  assert(strcmp(new_dir, full_test_dir_path) == 0);

  // Restore the previous directory.
  char *args_restore[] = {"cd", original_dir, NULL};
  assert(builtin_cd(args_restore) == 0);

  // Clean up: remove the test directory
  rmdir(test_dir_name);

  free(original_dir);
  free(new_dir);
  printf("test_builtin_cd: Passed\n");
}
void test_builtin_exit() {
  // Test exiting (we can't directly assert this in a simple way,
  // as it would terminate the test process.  We'll rely on
  // manual observation and the fact that the shell exits cleanly).
  // We'll just *call* it and, if the test program continues, that's a problem.
  // char* args[] = {"exit", NULL};
  // builtin_exit(args);
  // If we reach this point, exit() didn't work!
  // assert(0 && "builtin_exit failed to exit!"); // This will always fail.
  printf("test_builtin_exit: Check Manually\n");
}

void test_builtin_history() {

  char test_history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
  int test_history_count = 0;
  int current_index = 0;

  add_to_history("command1", test_history, &test_history_count, &current_index);
  add_to_history("command2", test_history, &test_history_count, &current_index);
  // Redirect stdout to a buffer.
  int stdout_copy = dup(STDOUT_FILENO); // Save stdout
  char buffer[1024];
  int fd =
      open("test_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // temp file.
  dup2(fd, STDOUT_FILENO); // Redirect stdout to a file

  char *args[] = {"history", NULL};
  builtin_history(args);

  // restore stdout
  dup2(stdout_copy, STDOUT_FILENO);
  close(stdout_copy);
  close(fd);

  // Read content
  FILE *file = fopen("test_output.txt", "r");
  assert(file != NULL);
  size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
  buffer[bytes_read] = '\0';
  fclose(file);

  // Verify content
  assert(strstr(buffer, "command1") != NULL);
  assert(strstr(buffer, "command2") != NULL);

  // Remove file
  remove("test_output.txt");
  printf("test_builtin_history: Passed\n");
}

void test_execute_builtin() {

  // Test valid built-in
  char *args_cd[] = {"cd", "..", NULL};
  int ret_cd = executable_builtin(args_cd, 2);
  assert(ret_cd == 0); // cd returns 0 on success.

  // test invalid built in
  char *args_invalid[] = {"invalid_command", NULL};
  int ret_inv = executable_builtin(args_invalid, 1);
  assert(ret_inv == -1); // not a built-in.

  printf("test_execute_builtin: Passed\n");
}

void test_expand_wildcards_no_match() {
  char **expanded = expand_wildcards("nonexistent_file_*.txt");
  assert(expanded != NULL);
  assert(strcmp(expanded[0], "nonexistent_file_*.txt") ==
         0); // Should return original string
  assert(expanded[1] == NULL);
  free(expanded[0]); // Free the duplicated string
  free(expanded);
  printf("test_expand_wildcards_no_match: Passed\n");
}

void test_expand_wildcards_single_match() {
  // Create a temporary file for testing
  FILE *temp_file = fopen("test_file.txt", "w");
  fclose(temp_file);

  char **expanded = expand_wildcards("test_file.txt");
  assert(expanded != NULL);
  assert(strcmp(expanded[0], "test_file.txt") == 0);
  assert(expanded[1] == NULL);
  free(expanded[0]);
  free(expanded);

  // Clean up: remove the test file
  remove("test_file.txt");
  printf("test_expand_wildcards_single_match: Passed\n");
}

void test_expand_wildcards_multiple_matches() {
  // Create a few temporary files
  FILE *temp_file1 = fopen("file1.txt", "w");
  fclose(temp_file1);
  FILE *temp_file2 = fopen("file2.txt", "w");
  fclose(temp_file2);

  char **expanded = expand_wildcards("file?.txt"); // Use ? wildcard
  assert(expanded != NULL);
  assert(strcmp(expanded[0], "file1.txt") == 0 ||
         strcmp(expanded[0], "file2.txt") == 0);
  assert(strcmp(expanded[1], "file1.txt") == 0 ||
         strcmp(expanded[1], "file2.txt") == 0);
  assert(expanded[2] == NULL);

  // Important:  Free *each* string, and then the array itself
  for (int i = 0; expanded[i] != NULL; ++i) {
    free(expanded[i]);
  }
  free(expanded);

  // Clean up
  remove("file1.txt");
  remove("file2.txt");
  printf("test_expand_wildcards_multiple_matches: Passed\n");
}

void test_expand_wildcards_star() {
  // Create temporary files for testing the '*' wildcard
  FILE *temp_file1 = fopen("test_file_a.txt", "w");
  fclose(temp_file1);
  FILE *temp_file2 = fopen("test_file_b.log", "w");
  fclose(temp_file2);

  char **expanded = expand_wildcards("test_file_*");
  assert(expanded != NULL);
  // Check if the files are contained. The order is not guaranteed.
  int found_a = 0;
  int found_b = 0;
  for (int i = 0; expanded[i] != NULL; ++i) {
    if (strcmp(expanded[i], "test_file_a.txt") == 0)
      found_a = 1;
    if (strcmp(expanded[i], "test_file_b.log") == 0)
      found_b = 1;
  }
  assert(found_a);
  assert(found_b);

  for (int i = 0; expanded[i] != NULL; ++i) {
    free(expanded[i]);
  }
  free(expanded);

  remove("test_file_a.txt");
  remove("test_file_b.log");
  printf("test_expand_wildcards_star: Passed\n");
}

int main() {
  // Run all test cases
  test_parse_simple_command();
  test_parse_command_with_args();
  test_parse_input_redirection();
  test_parse_output_redirection();
  test_parse_append_redirection();
  test_parse_pipe();
  test_parse_empty_command();
  test_parse_multiple_pipes();
  test_parse_combined_redirection_and_pipe();
  test_parse_error_handling();
  test_history_add_and_get();
  test_history_circular_buffer();
  test_get_input_basic();
  test_builtin_cd();
  test_builtin_exit();
  test_builtin_history();
  test_execute_builtin();
  test_expand_wildcards_no_match();
  test_expand_wildcards_single_match();
  test_expand_wildcards_multiple_matches();
  test_expand_wildcards_star();

  printf("All tests completed.\n");

  return 0;
}
