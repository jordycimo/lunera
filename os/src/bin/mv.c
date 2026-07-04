#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    int force = 0;
    int verbose = 0;

    const char* short_opts = "fv";

    int opt;

    while((opt = getopt(argc, argv, short_opts)) != -1) {
        switch (opt) {
            case 'f':
                force = 1;
            break;
            case 'v':
                verbose = 1;
            break;
            optind++;
        }
    }

    if ()

    return 0;
}
