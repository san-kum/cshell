#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "utils.h"

typedef enum {
  SCRIPT_COMMAND,
  SCRIPT_IF,
  SCRIPT_ELSE,
  SCRIPT_WHILE,
  SCRIPT_FUNCTION,
  SCRIPT_RETURN,
  SCRIPT_VARIABLE
} ScriptElementType;

typedef struct ScriptElement {
  ScriptElementType type;
  char *content;
  Command *cmd;
  struct ScriptElement *condition;
  struct ScriptElement *body;
  struct ScriptElement *next;
} ScriptElement;

typedef struct {
  char **variables;
  char **values;
  int var_count;
  int max_var_capacity;
} ScriptContext;

ScriptElement *parse_script(const char *script_text);
int execute_script(ScriptElement *script);
void free_script_element(ScriptElement *element);
void init_script_context(ScriptContext *context);
void add_variable(ScriptContext *context, const char *name, const char *value);
char *get_variable(ScriptContext *context, const char *name);
void free_script_context(ScriptContext *context);

#endif // !SCRIPTING_H
