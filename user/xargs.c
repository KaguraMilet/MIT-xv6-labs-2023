#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// Xargs should accept one argument one by one instead of 
// feed more than argument to the command at a time.
int main(int argc, char* argv[]) {
  // argn: the number of arguments
  int i, pos = 0;
  // Buffer input from pipeline
  char c;
  char buf[MAXARG];
  char* args[MAXARG];
  // Get args from xargs right side
  for(i = 0; i < argc - 1; i++) {
    args[i] = argv[i + 1];
  }
  int argn = argc - 1;

  // Handle one argument per time
  while(read(0, &c, 1) > 0) {
    if(c != '\n') {
      buf[pos] = c;
      pos++; 
    } 
    else {
      // Read one argument from stdin
      buf[pos] = '\0';
      args[argn] = (char*)malloc(pos*sizeof(char));
      strcpy(args[argn], buf);  pos = 0;
      memset(buf, '\0', MAXARG);
      argn++;
    }
  }
  // Child process
  if(fork() == 0) {
    // Close stdin for child process
    close(0);
    exec(args[0], args);
  }
  wait(0);
  
  exit(0);
}