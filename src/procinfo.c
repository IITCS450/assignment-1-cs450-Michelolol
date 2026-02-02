#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
    if(c!=2||!isnum(v[1])) usage(v[0]);
 
    char *pid = v[1];
    char path[256], line[256], state[64];
    int ppid = 0, vmrss = 0;

    snprintf(path, sizeof(path), "proc/%s/status", pid);    //points to status folder (path)
    FILE *f = fopen(path, "r");                      //creates a path for r
    if(!f){
        perror("Error opening status file");
        return 1;
    }
    
 
    while(fgets(line, sizeof(line), f)){
        if(strncmp(line, "State:", 6) == 0) sscanf(line, "State: %s", state);
        if(strncmp(line, "PPid:", 5) == 0) sscanf(line, "PPid: %d", &ppid);
        if(strncmp(line, "VmRSS:", 6) == 0) sscanf(line, "VmRSS: %d", &vmrss);
    }
    fclose(f);

    printf("PID:%s\n", v[1]);
    printf("State:%s\n", state);
    printf("PPID:%d\n", ppid);

    snprintf(path, sizeof(path), "proc/%s/cmdline", v[1]);    //points to cmdline folder (path)
    f = fopen(path, "r");        
    if (f){
        printf("Cmd:");
        int ch;
        while ((ch = fgetc(f)) != EOF) putchar(ch == '\0' ? ' ' : ch);
        printf("\n");
        fclose(f);        
    }

    snprintf(path, sizeof(path), "proc/%s/stat", v[1]);    //points to stat folder (path)
    f = fopen(path, "r");        
    if (f){
        unsigned long utime, stime;
        if(fscanf(f, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime) == 2) {
            long hz = sysconf(_SC_CLK_TCK);
            printf("CPU:%.0f %.3f\n", (double)utime / hz, (double)stime / hz);
        }   
        fclose(f);
    }
    printf("VmRSS:%d\n", vmrss);

 return 0;
}
