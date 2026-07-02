#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int mode = 0; // default mode 0: relative pathing; 1 is exact pathing
    int force = 0; // default force 0: dont overwrite; 1 to force writing

    if (argv[1] == NULL || argv[2] == NULL) {
        printf("mv: expected 2 arguments, \"mv [source] [dest]\"");
        return 1;
    }

    if (strcmp(args[1], "-f") == 0) {
        force = 1;
    }

    if (mode == 0) {
        if (access(arg[2], F_OK)) {
            perror("destination exists! -f to force");
            return 1;
        }

        if (rename(argv[1], argv[2])) {
            perror("rename");

        } else {
            printf("%s -> ", argv[1]);
            printf("%s\n", argv[2]);
        }
    }
}
