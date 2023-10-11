#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Return file name
char* fname(char* path) {
  static char buf[DIRSIZ+1];
  memset(buf, '\0', DIRSIZ+1);
  char* p;

  uint path_len = strlen(path);
  // Find first character after last slash.
  for(p=path+path_len; p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)   // Length from p to end exceeds DIRSIZ
    return p;
  memmove(buf, p, strlen(p));
  return buf;
}

void find(char* path, char* name) {
  // buffer store the path of file
  char buf[512], *filename, *p;
  char current_dir[] = ".";
  char parent_dir[] = "..";
  int fd;
  struct stat st;   // File metadata
  struct dirent de; //  Directory entry

  if((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_DEVICE:
    case T_FILE:
      filename = fname(path);
      if(strcmp(filename, name) == 0) {
        printf("%s\n", filename);
      }
      break;
    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("ls: path too long\n");
        break;
      }
      // Copy path into buffer
      strcpy(buf, path);
      p = buf + strlen(buf);
      *(p++) = '/';
      // Iterate through directory
      while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if(stat(buf, &st) < 0) {
          printf("find: cannot stat %s\n", buf);
          continue;
        }

        switch (st.type) {
          case T_DEVICE:
            break;
          case T_FILE:
            filename = fname(buf);
            if(strcmp(filename, name) == 0) {
              printf("%s\n", buf);
            }
            break;
          case T_DIR:
            filename = fname(buf);
            // Avoid recursion '.' and '..'
            if(strcmp(filename, current_dir) == 0 || strcmp(filename, parent_dir) == 0)
              break;
            // Recursive find
            find(buf, name);
            break;
        }
      }
      break;
  }
  close(fd);
}

int main(int argc, char* argv[]) {
  if(argc < 3) {
    fprintf(2, "Usage: find directory name ...\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}