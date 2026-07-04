#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {

    int force = 0;
    int verbose = 0;

    int opt;

    // just parse args here, set option variables
    while((opt = getopt(argc, argv, "fv")) != -1) {
        switch (opt) {
            case 'f':
                force = 1;
            break;
            case 'v':
                verbose = 1;
            break;
        }
    }

    // prints force status and all other command line arguments after a hypothetical -f and / or -v
    if (verbose) {
        printf("force overwrite: %d\n", force);

        for (int i = optind; i < argc; i++) {
            printf("%d: %s\n", i, argv[i]);
        }
    }


    // argc-optind = 2 if the correct amount of arguments were passed
    if (argc-optind != 2) {
        fprintf(stderr, "mv: expected at least 2 arguments, \"mv [source] [dest]\"");
    }

    // if were forcing, ask to confirm and then either write or cancel. also handle errors
    if (force) {
        printf("force overwrite? (y/N): \n");
        int chr = getc(stdin);

        if (chr == 'Y' || chr == 'y') {
            if (rename(argv[2], argv[3]) != 0) {
                perror("rename");
            }

        } else {
            fprintf(stderr, "mv: operation canceled");
        }
    }

    return 0;
}
