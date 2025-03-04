#include "include/history.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

char history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
int history_count = 0;

void add_to_history(char *command, char history[][MAX_INPUT_SIZE],
                    int *history_count, int *current_history_index) {
  if (strlen(command) > 0 && strcmp(command, "\n") != 0) {
    command[strcspn(command, "\n")] = 0;
    strcpy(history[(*history_count) % MAX_HISTORY_SIZE], command);

    (*history_count)++;
    *current_history_index = *history_count;
  }
}

void print_history(char history[][MAX_INPUT_SIZE], int history_count) {
  int start =
      (history_count > MAX_HISTORY_SIZE) ? history_count - MAX_HISTORY_SIZE : 0;
  for (int i = start; i < history_count; i++) {
    printf("%d  %s\n", i + 1, history[i % MAX_HISTORY_SIZE]);
  }
}

char *get_history_entry(char history[][MAX_INPUT_SIZE], int history_count,
                        int index) {
  if (history_count == 0) {
    return NULL; // No history yet
  }

  // Adjust index for circular buffer and check bounds
  int real_index;
  if (history_count <= MAX_HISTORY_SIZE) {
    if (index < 0 || index >= history_count) {
      return NULL; // Out of range
    }
    real_index = index;
  } else {
    // Handle the circular buffer case
    if (index < history_count - MAX_HISTORY_SIZE || index >= history_count) {
      return NULL; // Out of range
    }
    real_index = (index + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE;
  }

  return history[real_index];
}

// Get input with history support and basic line editing
int get_input(char *buffer, char history[][MAX_INPUT_SIZE], int *history_count,
              int *current_history_index) {

  static struct termios old_termios,
      new_termios; // static to keep values between function calls

  // Get current terminal settings
  tcgetattr(STDIN_FILENO, &old_termios);
  new_termios = old_termios;

  // Disable canonical mode (line buffering) and echo
  new_termios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

  int i = 0; // Current position in the buffer
  int ch;
  *current_history_index = *history_count; // Start at the end of history.
  while (1) {
    ch = getchar();

    if (ch == EOF || ch == '\n') {
      buffer[i] = '\0';
      putchar('\n');
      break;
    } else if (ch == 127 || ch == 8) { //  Backspace or delete key
      if (i > 0) {
        printf("\b \b"); // Erase character from screen
        fflush(stdout);
        i--;
      }
    } else if (ch == 27) {         // Escape sequence (likely arrow key)
      if (getchar() == 91) {       // Check for '['
        int arrow_key = getchar(); // Get the actual arrow key code
        if (arrow_key == 65) {     // Up arrow
          if (*current_history_index > 0 && *history_count > 0) {
            (*current_history_index)--;

            // Clear the current line
            printf("\033[2K\r"); //\033[2K -> erase entire line, \r move cursor
                                 // to the begining
            printf("cshell> ");
            char *history_entry = get_history_entry(history, *history_count,
                                                    *current_history_index);

            if (history_entry) {
              strcpy(buffer, history_entry);
              i = strlen(buffer); // Update cursor position
              printf("%s", buffer);
              fflush(stdout);
            } else {
              (*current_history_index)++; // prevent index errors
            }
          }
        } else if (arrow_key == 66) { // Down arrow
          if (*current_history_index < *history_count) {
            (*current_history_index)++;
            printf("\033[2K\r");
            printf("cshell> ");

            if (*current_history_index == *history_count) {
              // Clear the buffer if we're at the "new" command
              buffer[0] = '\0';
              i = 0;
              printf("%s", buffer);
              fflush(stdout);
            } else {
              char *history_entry = get_history_entry(history, *history_count,
                                                      *current_history_index);

              if (history_entry) {
                strcpy(buffer, history_entry);
                i = strlen(buffer); // Update cursor position
                printf("%s", buffer);
                fflush(stdout);
              } else {
                (*current_history_index)--;
              }
            }
          }
        }
      }
    } else if (isprint(ch)) { // Check if is a printable character
      // Regular character, add to buffer
      buffer[i++] = ch;
      putchar(ch); // Echo character to screen
      fflush(stdout);

      if (i >= MAX_INPUT_SIZE - 1) {
        print_error("Maximum line length exceeded.");
        buffer[0] = '\0';
        i = 0;
        break;
      }
    }
    // Ignore other control characters.
  }

  // *** ADD THIS: Append a newline character ***
  buffer[i++] = '\n'; // Add the newline
  buffer[i] = '\0';   // Null-terminate *after* the newline

  // Restore original terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
  return i; // Return the length of the input
}
