#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t child_pid;
    child_pid = fork();
    if (child_pid > 0) {
        sleep(5);
        printf("Parent process out\n");
    } else {
        printf("Child process out\n");
        exit(0);
    }

    return 0;
}
