#include "include/scripting.h"
#include "include/builtins.h"
#include "include/utils.h"
#include <stdlib.h>
#include <string.h>

void init_script_context(ScriptContext *context) {
  context->variables = malloc(sizeof(char *) * 10);
  context->values = malloc(sizeof(char *) * 10);
  context->var_count = 0;
  context->max_var_capacity = 10;
}

void add_variable(ScriptContext *context, const char *name, const char *value) {
  if (context->var_count >= context->max_var_capacity) {
    context->max_var_capacity *= 2;
    context->variables =
        realloc(context->values, sizeof(char *) * context->max_var_capacity);
  }

  for (int i = 0; i < context->var_count; i++) {
    if (strcmp(context->variables[i], name) == 0) {
      free(context->values[i]);
      context->values[i] = strdup(value);
      return;
    }
  }

  context->variables[context->var_count] = strdup(name);
  context->values[context->var_count] = strdup(value);
  context->var_count++;
}

char *get_variable(ScriptContext *context, const char *name) {
  for (int i = 0; i < context->var_count; i++) {
    if (strcmp(context->variables[i], name) == 0) {
      return context->values[i];
    }
  }
  return NULL;
}

void free_script_context(ScriptContext *context) {
  for (int i = 0; i < context->var_count; i++) {
    free(context->variables[i]);
    free(context->values[i]);
  }
  free(context->variables);
  free(context->values);
}

ScriptElement *parse_script(const char *script_text) {
  ScriptElement *head = NULL;
  ScriptElement *current = NULL;
  char *token;
  char *script_copy = strdup(script_text);

  token = strtok(script_copy, "\n");

  while (token != NULL) {
    ScriptElement *element = malloc(sizeof(ScriptElement));
    memset(element, 0, sizeof(ScriptElement));

    while (*token == ' ' || *token == '\t')
      token++;

    if (strncmp(token, "if ", 3) == 0) {
      element->type = SCRIPT_IF;
      element->content = strdup(token + 3);
    } else if (strncmp(token, "else", 4) == 0) {
      element->type = SCRIPT_ELSE;
    } else if (strncmp(token, "while ", 6) == 0) {
      element->type = SCRIPT_WHILE;
      element->content = strdup(token + 6);
    } else if (strchr(token, '=')) {
      element->type = SCRIPT_VARIABLE;
      element->content = strdup(token);
    } else {
      element->type = SCRIPT_COMMAND;
      element->content = strdup(token);
      element->cmd = parse_command(token);
    }

    if (head == NULL) {
      head = element;
      current = element;
    } else {
      current->next = element;
      current = element;
    }
    token = strtok(NULL, "\n");
  }

  free(script_copy);
  return head;
}

int evaluate_condition(const char *condition) {
  Command *cmd = parse_command(condition);
  if (!cmd)
    return 0;
  int result = executable_builtin(cmd->args, cmd->argc);
  free_command(cmd);
  return result == 0;
}

int execute_script(ScriptElement *script) {
  ScriptContext context;
  init_script_context(&context);

  ScriptElement *current = script;

  while (current != NULL) {
    switch (current->type) {
    case SCRIPT_COMMAND:
      if (current->cmd) {
        executable_builtin(current->cmd->args, current->cmd->argc);
      }
      break;
    case SCRIPT_VARIABLE: {
      char *eq_pos = strchr(current->content, '=');
      if (eq_pos) {
        *eq_pos = '\0';
        char *name = current->content;
        char *value = eq_pos + 1;

        while (*name == ' ')
          name++;
        while (*value == ' ')
          value++;

        add_variable(&context, name, value);
      }
      break;
    }
    case SCRIPT_IF: {
      if (evaluate_condition(current->content)) {
        if (current->body) {
          execute_script(current->body);
        }
      } else if (current->next && current->next->type == SCRIPT_ELSE) {
        if (current->next->body) {
          execute_script(current->next->body);
        }
      }
      break;
    }
    case SCRIPT_WHILE: {
      while (evaluate_condition(current->content)) {
        if (current->body) {
          execute_script(current->body);
        }
      }
      break;
    }
    default:
      break;
    }
    current = current->next;
  }
  free_script_context(&context);
  return 0;
}

void free_script_element(ScriptElement *element) {
  if (element == NULL)
    return;

  if (element->next)
    free_script_element(element->next);

  if (element->content)
    free(element->content);

  if (element->cmd)
    free_command(element->cmd);

  if (element->body)
    free_script_element(element->body);

  free(element);
}
