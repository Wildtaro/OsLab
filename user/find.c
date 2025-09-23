#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user.h"

void find(char *path, char *target);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <name>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}

// 获取路径中的文件名部分
char* get_filename(char *path) {
  char *p = path + strlen(path);
  while (p >= path && *p != '/') p--;
  return p + 1;
}

void find(char *path, char *target) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      // 比较文件名是否匹配目标
      if (strcmp(get_filename(path), target) == 0) {
        printf("%s\n", path);
      }
      break;

    case T_DIR:
      // 检查当前目录名是否匹配
      if (strcmp(get_filename(path), target) == 0) {
        printf("%s\n", path);
      }

      // 检查路径长度是否合法
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        break;
      }

      // 构建新路径并递归搜索
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';

      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;  // 跳过空目录项
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, target);
      }
      break;
  }

  close(fd);
}