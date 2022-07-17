//user/pingpong.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define WRITE 0
#define READ 1

int main(int argc, char *argv[]) {
    if (argc >= 2) {
      printf("Usage: pingpong\n");
      exit(1);
    }
    char buf[2];        //一个缓冲区原来读写
    int p[2];           //文件描述符
    pipe(p);            //创建一个管道
    int pid = fork();   //创建一个子进程
    if (pid == 0) {
      read(p[READ], buf, 1);//从管道读取一个字节
      close(p[READ]);       //关闭管道读端
      printf("%d: received ping\n", getpid());

      write(p[WRITE], buf, 1);//向管道写入一个字节
      close(p[WRITE]);        //关闭管道写端
    } else {
      write(p[WRITE], buf, 1);//向管道写入一个字节
      close(p[WRITE]);        //关闭管道写端

      wait(0); //这里是关键，要到达子进程结束后父进程才继续执行下去

      read(p[READ], buf, 1);  //从管道读取一个字节
      printf("%d: received pong\n", getpid());
      close(p[READ]);         //关闭管道写端
    }
    exit(0);
}