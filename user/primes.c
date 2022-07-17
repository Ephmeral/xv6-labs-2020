//user/primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WRITE 1
#define READ 0

void primes(int p[]) {
    close(p[WRITE]);  //关闭原管道的写端

    int np[2]; 
    pipe(np);         //创建一个新的管道

    //两个数分别表示从管道读取的第一个数字，以及接下来的数字
    int first = 1, next = 0;
    //如果此时已经读取不到数字了，关闭管道并且退出
    if (read(p[READ], &first, 4) == 0) {
        close(p[READ]);
        exit(0);
    }
    //如果能读取到数字，打印输出
    printf("prime %d\n", first);
    
    int pid = fork();     //创建一个子进程
    if (pid == 0) {   
        close(np[WRITE]); //关闭新管道的写端
        primes(np);       //进入下一层递归调用
        close(np[READ]);  //关闭新管道的读端
    } else {
        close(np[READ]);  //关闭新管道的读端
        //从原管道中读取数字，read返回为0，表示已经读取完了
        while (read(p[READ], &next, 4) != 0) {
            //如果读取的数字不能整除读取第一个数字，写入下一层管道
            if (next % first != 0) { 
                write(np[WRITE], &next, 4);
            }
        }
        close(p[READ]);  //关闭原管道的读端
        close(np[WRITE]);//关闭新管道的写端
        wait((int*)0);   //等待子进程结束
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    //参数不符合情况时候返回报错
    if (argc >= 2) {
      printf("Usage: primes\n");
      exit(1);
    }
    
    int p[2];
    pipe(p);            //创建一个管道

    int pid = fork();   //创建一个子进程
    if (pid == 0) {
        close(p[WRITE]);//关闭管道的写端
        primes(p);      //进行递归调用
    } else {
        close(p[READ]);//关闭管道的读端
        //依次将2-35写入到管道中，由子进程进行进一步处理
        for (int i = 2; i <= 35; ++i) {
            write(p[WRITE], &i, 4);
        }
        close(p[WRITE]);//关闭管道的写端
        wait((int*)0);  //等待子进程结束
    }
    exit(0);
}
