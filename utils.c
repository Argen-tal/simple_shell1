#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

char* read_line() {
    char* line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        /*Handling EOF (Ctrl+D)*/
        exit(SHELL_EXIT);
    }
    return line;
}

void free_args(char** args) {
    size_t i;
    for (i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}
