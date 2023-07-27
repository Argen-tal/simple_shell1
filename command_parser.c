#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

#define MAX_ARGS 64
#define MAX_ARG_LENGTH 100

char** parse_command(char* line) {
    char* token;
    char* end_of_token;
    size_t arg_count = 0;
    int quote_open = 0;
    size_t i, j;

    char** args = (char**)malloc(MAX_ARGS * sizeof(char*));
    if (args == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\n");
    while (token != NULL) {
        if (token[0] == '"' || token[0] == '\'' || token[0] == '`') {
            quote_open = 1;
            memmove(token, token + 1, strlen(token));
        }

        while (quote_open && (end_of_token = strrchr(token, token[0])) == NULL) {
            char* next_token = strtok(NULL, " \t\n");
            if (next_token == NULL) {
                fprintf(stderr, "Unmatched %c\n", token[0]);
                free_args(args);
                return NULL;
            }
            strcat(token, " ");
            strcat(token, next_token);
        }

        if (quote_open && end_of_token != token) {
            quote_open = 0;
            *end_of_token = '\0';
        }

        for (i = 0, j = 0; i < strlen(token); i++, j++) {
            if (token[i] == '\\') {
                i++;
                if (token[i] == '\0') {
                    fprintf(stderr, "Invalid escape sequence\n");
                    free_args(args);
                    return NULL;
                }
            }
            token[j] = token[i];
        }
        token[j] = '\0';
args[arg_count] = (char*)malloc((strlen(token) + 1) * sizeof(char));
        if (args[arg_count] == NULL) {
            perror("Memory allocation error");
            free_args(args);
            exit(EXIT_FAILURE);
        }

        strcpy(args[arg_count], token);
        arg_count++;

        if (arg_count >= MAX_ARGS) {
            fprintf(stderr, "Too many arguments\n");
            free_args(args);
            return NULL;
        }

        token = strtok(NULL, " \t\n");
    }

    if (quote_open) {
        fprintf(stderr, "Unmatched %c\n", line[strlen(line) - 1]);
        free_args(args);
        return NULL;
    }

    args[arg_count] = NULL;

    for (i = 0; i < arg_count; i++) {
        char* expanded_token = expand_env_variables(args[i]);
        if (expanded_token != NULL) {
            free(args[i]);
            args[i] = expanded_token;
        } else {
            free_args(args);
            return NULL;
        }
    }

    return args;
}
