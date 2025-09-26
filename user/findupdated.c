#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

int 
matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

int 
matchstar(int c, char *re, char *text)
{
  do{
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void
find(char *path, char *pattern)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_FILE:
    if(match(pattern, fmtname(path))){
      printf("%s\n", path);
    }
    break;
    
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
        
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      
      if(match(pattern, de.name)){
        printf("%s\n", buf);
      }
      
      if(st.type == T_DIR){
        find(buf, pattern);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: find <path> [pattern]\n");
    fprintf(2, "  If no pattern is given, lists all files\n");
    fprintf(2, "  Pattern supports regex: . (any char), * (zero or more), ^ (start), $ (end)\n");
    exit(1);
  }
  
  if(argc == 2){
    find(argv[1], ".*");
  } else {
    find(argv[1], argv[2]);
  }
  
  exit(0);
}

