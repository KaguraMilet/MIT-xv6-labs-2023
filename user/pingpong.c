#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int p[2];
    pipe(p);
    char receive;

    if(fork() == 0) {
        // Child process
        int len = read(p[0], (void*)&receive, 1);   // read from the pipe
        if(len == 1) {
            fprintf(1, "%d: received ping\n", getpid());
            write(p[1], (void*)&receive, 1);
            close(p[0]);
            close(p[1]);
            exit(0);
        } else {
            fprintf(1, "Error when received from the pipe\n");
            exit(1);
        }
    }

    // Parent process
    receive = 'p';
    write(p[1], (void*)&receive, 1);
    int len = read(p[0], (void*)&receive, 1);   // parent read from pipe
    if(len != 1) {
        fprintf(1, "Error when received from the pipe\n");
        exit(1);
    } else {
        fprintf(1, "%d: received pong\n", getpid());
        close(p[0]);
        close(p[1]);
    }
    
    exit(0);
}