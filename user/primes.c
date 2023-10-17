#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int i;
    int p[2];
    pipe(p);

    // Child process
    if(fork() == 0) {
        int number;
        int receive;
        int is_number = 1;
        int channel[2];
        // Child process doesn't need to write to the parent 
        close(p[1]);
        while(read(p[0], (void*)&receive, sizeof(int))) {
            if(is_number == 1) {
                number = receive;
                printf("prime %d\n", number);
                pipe(channel);
                // Create right neighbour
                if(fork() == 0) {
                    // No data to send to the left neighbour
                    close(channel[1]);
                    // Receive data from the left neighbour
                    p[0] = channel[0];
                    continue;
                }
                close(channel[0]);
                is_number = 0;
            }
            else {
                if(receive % number == 0) {
                    continue;
                }
                write(channel[1], (void*)&receive, sizeof(int));
            }
        }
        close(p[0]);
        close(channel[1]);
    }
    close(p[0]);
    for(i = 2; i <= 35; ++i) {
        write(p[1], (void*)&i, sizeof(int));
    }
    close(p[1]);
    wait(0);
    exit(0);
}