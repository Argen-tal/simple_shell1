#include <stdio.h>
#include "shell.h"

int execute_builtin_exit() {
    return SHELL_EXIT;
}

int execute_builtin_env() {
        int i;
    extern char** environ;
    for (i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}
