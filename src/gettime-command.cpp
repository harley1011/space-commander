#include "gettime-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void* GetTimeCommand::Execute(){
    struct timeval tv;

    if(gettimeofday(&tv, 0) == -1){
        perror ("Error! gettimeofday()\n");
        exit(1);
    }

    time_t* rawtime = (time_t*)malloc(sizeof(time_t));
    *rawtime = tv.tv_sec;
    
    return (void*)rawtime;
}
