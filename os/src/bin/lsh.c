#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT 1024
#define MAX_PATH 4096

// main loop : done!
// input parsing : done!

// builtins: cd, pwd, echo, setenv, unsetenv, which, exit
// external execution
// manage environment variables
// manage path
// error handling

// parse
char** parse(char* input) {
    size_t buffer_size = MAX_INPUT;

    char** tokens = malloc(buffer_size * sizeof(char*));
    char* token = NULL;
    size_t position = 0;
    size_t token_length = 0;

    if (!tokens) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; input[i]; i++) {
        while (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '\r' || input[i] == '\a') {
            i++;
        }

        if (input[i] == '\0') {
            break;
        }

        token = &input[i];

        // to me, this line is confusing for some reason. it is checking if input[i]
        // is NOT null AND also checking if input[i] is NOT a space.
        while (input[i] && input[i] != ' ' && input[i] != '\n' && input[i] != '\t' && input[i] != '\r' && input[i] != '\a') {
            token_length++;
            i++;
        }

        tokens[position] = malloc((token_length + 1) * sizeof(char));

        if (!tokens[position]) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        for (size_t j = 0; j < token_length; j++) {
            tokens[position][j] = token[j];
        }

        tokens[position][token_length] = '\0'; // null terminate token
        position++;
        token_length = 0; // reset token length counter
    }

    tokens[position] = NULL; // terminate array
    return tokens;
}

// oops! a memory leak!
// lets free allocated tokens..
void free_tokens(char** tokens) {
    if (tokens) {
        return;
    }
    for (size_t i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }

    free(tokens);
}

// search path for provided arg
char* find_in_path(const char* command, char** env) {
    char* path_env = NULL; // store PATH
    char* path = NULL; // duplicate of PATH, we mess with this one instead of path_env
    char* token = NULL; // tokenized dirs from path
    char full_path[MAX_PATH]; // buffer to construct path

    path_env = getenv(env);

    if (!path_env) {
        return NULL; // no path
    }

    path = strdup(path_env);

    printf("path: %s\n", path);
}

// shell builtins
// cd, pwd, echo, env, which, setenv, unsetenv, exit

// changes directory
int c_cd(char** args, char* initial_directory) {
    if (args[1] == NULL) {
        printf("cd: expected 1 argument, \"cd [path]\"\n");
    } else if (chdir(args[1]) == 0) {
        printf("\n");
    } else {
        perror("cd");
        return 1;
    }

    return 0;
}

// prints working directory
int c_pwd() {
    char* cwd = NULL;

    // dynamic allocation
    cwd = getcwd(NULL, 0);

    if (cwd != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
        return 1;
    }

    return 0;
}

// prints strings passed as arguments
int c_echo(char** args, char** env) {
    int newline = 1; // echo terminates with \n by default
    size_t i = 1;

    if (args[1] != NULL && strcmp(args[1], "-n") == 0) {
        newline = 0;
        i++; // skip first arg within for loop
    }

    for (; args[i]; i++) {
        if (args[i][0] == '$') { // handle env variable
            char* value = getenv(args[i] + 1); // skip $ and get variable
            if (value) {
                printf("%s", value);
            } else {
                printf("\n");
            }
        } else {
            printf("%s", args[i]);
        }

        if (args[i + 1] != NULL) {
            printf(" ");
        }
    }

    if (newline) {
        printf("\n");
    }

    return 0;
}

int c_env(char** env) {
    size_t i = 0;

    while (env[i]) {
        printf("%s\n", env[i]);
        i++;
    }
    return 0;
}

int c_which(char** args, char** env) {
    if (args[1] == NULL) {
        printf("which: expected one argument, \"which [command]\"\n");
        return 1;
    }

    // list of builtins
    const char* builtin_commands[] = {"cd","pwd","echo","env","setenv","unsetenv","which","exit"};
    for (size_t i = 0; builtin_commands[i]; i++) {
        if (strcmp(args[1], builtin_commands[i]) == 0) {
            printf("%s: builtin\n", args[1]);
            return 0;
        }
    }

    char* path = find_in_path(args[1], env);
}

char** c_setenv(char** args, char** env) {
    
}

char** c_unsetenv(char** args, char** env) {
    
}

// executor function
int execute() {

}

// builtins runner
int builtins(char** args, char** env, char* initial_directory) {
    //printf("args[0]: %s\n", args[0]);

    if (!strcmp(args[0], "cd")) {
        return c_cd(args, initial_directory);
    } else if (!strcmp(args[0], "pwd")) {
        return c_pwd();
    } else if (!strcmp(args[0], "echo")) {
        return c_echo(args, env);
    } else if (!strcmp(args[0], "env")) {
        return c_env(env);
    } else if (!strcmp(args[0], "which")) {
        return c_which(args, env);
    } else if (!strcmp(args[0], "exit")) {
        exit(EXIT_SUCCESS);
    } else {
        // not a builtin
        execute();
    }
}

// loop function
void loop(char** env) {
    char* input = NULL;
    size_t input_size = 0;

    char** args;

    char* initial_directory = getcwd(NULL, 0);

    while (1) {
        printf("lsh>");
        fflush(stdout);

        if (getline(&input, &input_size, stdin) == -1) { // EOF (ctrl+d) or error
            perror("getline");
            break;
        }

        args = parse(input);

        if (args[0]) {
            builtins(args, env, initial_directory);
        }

        free_tokens(args);
    }
}

//main
int main(int argc, char* argv[], char** env) {

    // remove these variables (we dont use them)
    (void)argc;
    (void)argv;

    loop(env);

    return 0;
}
