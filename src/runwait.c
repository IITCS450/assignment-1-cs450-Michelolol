#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}

static double d(struct timespec a, struct timespec b){
 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}

int main(int c,char**v){
    if (c < 2) 
        usage(v[0]);
    struct timespec start, end;
    pid_t pid;
    int status;
    clock_gettime(CLOCK_MONOTONIC, &start); //Monotonic is time that only goes forward
                                            // ie it can't be repeated like a clock (pm-am)

    pid = fork();
    
    if (pid < 0){ //checks for fork failures
        perror("Fork Failed");
        return 1;
    }
    else if (pid == 0){
        if(execvp(v[1], &v[1]) == -1) { //checks for execvp failures
            perror("execvp failed");
            exit(1);
        }
    }
    else{
        waitpid(pid, &status, 0);           //waits for child to finish
        clock_gettime(CLOCK_MONOTONIC, &end);
        printf("pid=%d elapsed=%.3f exit=%d\n", 
            pid, 
            d(start, end), 
            WIFEXITED(status) ? WEXITSTATUS(status) : -1);
    }
    
    return 0;
}
