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
        int size = 0; //表示从标准输入中读取的字节数目
        //从标准输入中一个字节一个字节读取
        while (read(0, &buf[size], 1) != 0) {
            //遇见/n 跳出
            if (buf[size] == '\n') break;
            ++size;
        } 
        //如果一个字节都没读取到，说明标准输入内容已经读取完了
        if (size == 0) break;
        buf[size] = 0; //buf结尾置为0
        //将从标准输入读取的一行内容放入到argvs末尾中
        argvs[argc - 1] = buf;
        //创建一个子进程，利用exec执行
        if (fork() == 0) {
            exec(argv[1], argvs);
            exit(0);
        } 
        wait(0); //等待父进程结束
        memset(buf, 0, 1024); //将buf清空
    }
    exit(0);    
}