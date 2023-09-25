#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char*argv[]) {
  if(argc < 2) {
    fprintf(2, "Usage: pause for a user-specified number of ticks\n");
    exit(1);
  } else {
    int sleep_time = atoi(argv[1]);
    fprintf(1, "(nothing happens for a little while)\n");
    sleep(sleep_time);
  }
  exit(0);
}