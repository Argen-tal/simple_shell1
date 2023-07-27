#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

typedef struct ParsedCommand {
    int type;
    char** args;
} ParsedCommand;

#define BUILTIN_EXIT 0
#define BUILTIN_ENV  1
#define EXTERNAL_COMMAND 2

#define SHELL_ERROR -1
#define SHELL_EXIT 1

void handle_signals(int signal);
int shell_loop();
char* read_line();
char** parse_command(char* line);
char* expand_env_variables(char* token);
int execute_builtin_exit();
int execute_builtin_env();
int execute_external_command(ParsedCommand* command);
void free_args(char** args);

#endif /* SHELL_H */
