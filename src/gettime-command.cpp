#include "gettime-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>

void* GetTimeCommand::Execute(){
    struct timeval tv;

    if(gettimeofday(&tv, 0) == -1){
        perror ("Error! gettimeofday()\n");
        exit(1);
    }

    time_t* rawtime = (time_t*)malloc(sizeof(time_t));
    *rawtime = tv.tv_sec;

    std::cout << *rawtime << endl;    

    char* result = (char*)malloc(sizeof(char) * 50);
    memset(result, '\0', sizeof(char) * 50);
    sprintf(result, "%lld", (long long)*rawtime);
    
    return (void*)result;
}
