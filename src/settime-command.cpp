#include "settime-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void* SetTimeCommand::Execute(){
    struct timeval tv;
    
    tv.tv_sec = GetSeconds();   

    if (settimeofday(&tv, 0) != 0){
        perror ("Error! settimeofday()\n");
        exit(1);
    }

    return NULL;
}
