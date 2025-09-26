#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *name)
{
  char buf[512];
  char *p;
  int fd;
  struct stat st;
  struct dirent de;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type == T_FILE){
    char *base = path + strlen(path);
    while(base > path && *(base-1) != '/')
      base--;
    if(strcmp(base, name) == 0){
      printf("%s\n", path);
    }
    close(fd);
    return;
  }

  if(st.type == T_DIR){
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;

      if(strcmp(p, name) == 0){
        printf("%s\n", buf);
      }

      if(stat(buf, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }

      if(st.type == T_DIR){
        find(buf, name);
      }
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: find <start-path> <name>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}

