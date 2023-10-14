#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t pid = fork();

  if (pid == -1) {
    printf("Fork failed!\n");
    return 1;
  }

  if (pid == 0) {
    // child process
    char *args[] = {"./sequential_min_max", "4", "10000", "0", NULL};
    execvp(args[0], args);
    printf("Exec failed!\n");
    return 1;
  }

  // parent process
  wait(NULL);

  return 0;
}