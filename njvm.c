#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i],"--version") == 0) printf("version 1.1\n");
        else if (strcmp(argv[i],"--help") == 0) printf("look at the source code :)\n");
        else printf("unknown command line argument, %s, try --help\n", argv[i]);
    }
    printf("Ninja Virtual Machine started\n");
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}
