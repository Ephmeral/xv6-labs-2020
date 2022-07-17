//user/xargs.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
		fprintf(2, "Usage: xargs cmd...\n");
		exit(1);
	}

    char buf[1024];
    char *argvs[MAXARG];
    
    // memcpy(&argvs[0], &argv[1], sizeof(argv[0]) * (argc - 1));
    for (int i = 1; i < argc; ++i) {
		argvs[i - 1] = argv[i];
	}
    while (1) {
        int size = 0;
        while (read(0, &buf[size], 1) != 0) {
            if (buf[size] == '\n') break;
            ++size;
        } 
        if (size == 0) break;
        buf[size] = 0;
        argvs[argc - 1] = buf;
        if (fork() == 0) {
            exec(argv[1], argvs);
            exit(0);
        }
        wait(0);
        memset(buf, 0, 1024);
    }
    exit(0);    
}