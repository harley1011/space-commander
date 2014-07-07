#include "settime-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void* SetTimeCommand::Execute(){
    struct timeval tv;
    
    tv.tv_sec = GetSeconds();   
    tv.tv_usec = 0;
    if (settimeofday(&tv, 0) != 0){
        perror ("Error! settimeofday()\n");
        return (void*)0;        
    }

    return (void*)1;
}
