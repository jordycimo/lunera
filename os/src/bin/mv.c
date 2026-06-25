#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char cwd[PATH_MAX];
    int mode = 0; // default to relative path

    if(mode == 0) {
        // mode 0: relative pathing, filenames will have the cwd appended to the front of them.
        if(getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("error in getcwd");
            return -1;
        }

        char file[PATH_MAX] = ""; // file variable will hold the entire path to the file

        strcat(file, cwd);
        strcat(file, "/");
        strcat(file, argv[1]);
        printf("%s",file);

    } else if (mode == 1) {
        // mode 1: exact pathing, filenames will be treated exactly and will not have cwd apended.
    }
    return 0;
}
