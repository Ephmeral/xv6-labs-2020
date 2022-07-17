//user/find.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  return p;
}

void
find(char *path, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  //打开文件，如果失败直接报错
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  //查看文件的信息，失败则报错
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  //st.type为此时文件的类型
  switch(st.type){
  //当fd指向的是文件类型，进行比较
  case T_FILE:
	//如果比较发现是要查找的文件名，打印输出
    if (strcmp(fmtname(path), filename) == 0) {
      printf("%s\n", path);
    }
    break;
  //如果fd指向的是目录类型，需要进一步处理
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    } 
    strcpy(buf, path);  //将此时路径拷贝到buf上
    p = buf+strlen(buf);//p指向buf结尾的位置
	*p++ = '/';         //在buf结尾加上/，并且p指向下一个位置，先*p = '/'，再p++
	//读取目录中的每一个文件
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      //de.inum是所有文件的根目录的情况，.是当前目录本身，..是当前目录的父目录，都需要特判
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      //将此时文件名拷贝到p指向的位置，也就是buf后面接上文件名
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0; //字符串加上末端的0
      //打开此时的绝对路径，如果失败则跳过
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      //将此时绝对路径buf，进入下一层递归查找
      find(buf, filename);
      // printf("%s\n", fmtname(buf));
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc == 2) {
		find(".", argv[1]);
	} else if (argc == 3) {
		find(argv[1], argv[2]);
	} else {
    fprintf(2, "Usage: find path file\n");
    exit(1);
	}
  exit(0);
}