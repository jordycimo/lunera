#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>

int main(int argc, char *argv[]){
    char cwd[PATH_MAX];
    int mode = 0; // default to relative path

    if(mode == 0) {
        // mode 0: relative pathing, filenames will have the cwd appended to the front of them.
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s",cwd);
        } else {
            perror("error in getcwd");
            return -1;
        }
    } else if (mode == 1) {
        // mode 1: exact pathing, filenames will be treated exactly and will not have cwd apended.
    }
    return 0;
}
