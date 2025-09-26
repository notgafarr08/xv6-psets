#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char *strrchr(char *s, char c) {
  char *last = 0;
  for (; *s; s++) {
    if (*s == c) last = s;
  }
  return last;
}

void run_exec(char *cmd[], char *file) {
  char *argv[MAXARG];
  int i = 0;
  // copy cmd args
  while(cmd[i] && i < MAXARG-1) {
    argv[i] = cmd[i];
    i++;
  }
  // add the file at the end
  argv[i++] = file;
  argv[i] = 0;

  if(fork() == 0){
    exec(argv[0], argv);
    fprintf(2, "exec %s failed\n", argv[0]);
    exit(1);
  }
  wait(0);
}

void find(char *path, char *name, int has_exec, char *cmd[]) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    close(fd);
    return;
  }

  if(st.type == T_FILE){
    char *base = strrchr(path, '/');
    base = base ? base+1 : path;
    if(strcmp(base, name) == 0){
      if(has_exec)
        run_exec(cmd, path);
      else
        printf("%s\n", path);
    }
  }

  if(st.type == T_DIR){
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name, has_exec, cmd);
    }
  }
  close(fd);
}

int main(int argc, char *argv[]){
  if(argc < 3){
    fprintf(2, "usage: find path filename [-exec cmd]\n");
    exit(1);
  }
  if(argc > 3 && strcmp(argv[3], "-exec") == 0){
    find(argv[1], argv[2], 1, &argv[4]);
  } else {
    find(argv[1], argv[2], 0, 0);
  }
  exit(0);
}

