#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "shell.h"

char* get_command_path(char* command) {
    char* path = getenv("PATH");
    char* path_copy = strdup(path);
    char* dir = strtok(path_copy, ":");

    while (dir != NULL) {
        char* full_path = (char*)malloc(strlen(dir) + strlen(command) + 2);
        if (full_path == NULL) {
            perror("Memory allocation error");
            exit(SHELL_ERROR);
        }
        sprintf(full_path, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }
        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

void handle_signals(int signal) {
    printf("\n");
    printf("simple_shell> ");
    fflush(stdout);
    /*No need to use the signal parameter, so it remains unused.*/
    (void)signal;
}

int execute_external_command(ParsedCommand* command) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Fork error");
        return SHELL_ERROR;
    } else if (pid == 0) {
        /* Child process */
        char* command_path = get_command_path(command->args[0]);
        if (command_path == NULL) {
            fprintf(stderr, "Command not found: %s\n", command->args[0]);
            exit(SHELL_ERROR);
        }

        if (execv(command_path, command->args) == -1) {
            perror("Execution error");
free(command_path);
            exit(SHELL_ERROR);
        }
    } else {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : SHELL_ERROR;
    }

    return 0;
}

int shell_loop()
{
        char* line;
    ParsedCommand* command;
    int status = 0;

    signal(SIGINT, handle_signals);

    while (status != SHELL_EXIT) {
        printf("simple_shell> ");

        line = read_line();

        if (line == NULL || line[0] == '\0' || strcmp(line, "\n") == 0) {
            free(line);
            continue;
        }

        command = (ParsedCommand*)malloc(sizeof(ParsedCommand));
        if (command == NULL) {
            perror("Memory allocation error");
            exit(SHELL_ERROR);
        }

        command->args = parse_command(line);
        if (command->args == NULL) {
            free(command);
            free(line);
            continue;
        }

        if (strcmp(command->args[0], "exit") == 0) {
            command->type = BUILTIN_EXIT;
        } else if (strcmp(command->args[0], "env") == 0) {
            command->type = BUILTIN_ENV;
        } else {
            command->type = EXTERNAL_COMMAND;
        }

        if (command->type == BUILTIN_EXIT) {
            status = execute_builtin_exit();
        } else if (command->type == BUILTIN_ENV) {
            status = execute_builtin_env();
}

        free_args(command->args);
        free(command);
        free(line);
    }

    return status;
}
