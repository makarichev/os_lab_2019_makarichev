#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

bool timed_out = false;

void sigcheck(int signo) { //
    timed_out = true;
    printf("Timeout\n");
}

int kill_chd(pid_t* pids, const int size){ //
    for(int i = 0; i < size; ++i) {
        kill(pids[i], 9);
    }
    printf("Process kill");
    return 1;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int by_files=0;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            if (seed<=0){
              printf("Seed must be a positive number\n");
              return 1;
            }
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size<=0){
              printf("Array size must be a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if (pnum<=0){
              printf("Process number must be a positive number\n");
              return 1;
            }            
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        printf("check with_files status");
        break;

      case '?':
        printf("Unknown error");
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  by_files=1;
  printf("%d",by_files);
  if (by_files == 1) {
    with_files=true;
  }
  printf("%d",with_files);

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

 int step=array_size/pnum;

    FILE* fl;
    int** pipefd;
    printf("%d",with_files);
    if (with_files) {
        fl=fopen("test.txt","wb+");
        printf("success");
    }
    else {
        pipefd=(int**)malloc(sizeof(int*) * pnum);
        printf("%d",with_files);
      
    }


    pid_t child_proc[pnum];
    signal(SIGALRM, sigcheck);
    alarm(timed_out);

  for (int i = 0; i < pnum; i++) {
    if (!with_files){
             printf("error");
            pipefd[i]=(int*)malloc(sizeof(int)*2);
            if (pipe(pipefd[i])==-1) {
                printf("pipe failed");
                return 1;
            }


    }

    pid_t child_pid = fork();
    if (child_pid >= 0) {      
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {

        // child process
        printf("Pipe was created\n");
        struct  MinMax min_max_i;
      
        // parallel somehow
        if (i!=pnum-1){
            min_max_i=GetMinMax(array,i*step,(i+1)*step-1) ;
        }
        else{
            min_max_i=GetMinMax(array,i*step,array_size) ;
        }     
        

        if (with_files) {
          // use files here
          printf("success");
          fwrite(&min_max_i.min,sizeof(int),1,fl);
          fwrite(&min_max_i.max,sizeof(int),1,fl);
      
         } 
         else {
          // use pipe here
          write(pipefd[i][1],&min_max_i.min,sizeof(int));
          write(pipefd[i][1],&min_max_i.max,sizeof(int));

          close(pipefd[i][1]);
        }
        return 0;
      }

     } else {
      printf("fork failed\n");
      return 1;
    }
  }
  
    if(with_files){ 
      fclose(fl);
      fl=fopen("test.txt","r");
    }


    int stat_pid;

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    printf("finish-start time%d\n",finish_time.tv_usec - start_time.tv_usec);

    while (active_child_processes > 0) {
        if(timed_out) {
            kill_chd(child_proc, pnum); 
            return 2;
        }

        if (waitpid(-1, &stat_pid, WNOHANG) > 0) {
            active_child_processes -= 1;
        }

    }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
        fread(&min,sizeof(int),1,fl);
        fread(&max,sizeof(int),1,fl);

    } else {
      // read from pipes

     read(pipefd[i][0],&min,sizeof(int));
     read(pipefd[i][0],&max,sizeof(int));

     close(pipefd[i][0]);

     free(pipefd[i]);

    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  

    if (with_files) {
        fclose(fl);
    }
    else {
        free(pipefd);
    }

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}