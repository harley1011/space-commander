#include "settime-command.h"
#include <string.h>
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <shakespeare.h>
#define PROCESS "settime"



void* SetTimeCommand::Execute(){
    struct timeval tv;
    char result[10];
    result[0] = '0';
    tv.tv_sec = GetSeconds();   
    tv.tv_usec = 0;
    memcpy(result+2, &tv.tv_sec, sizeof(time_t)); 
    if (settimeofday(&tv, 0) != 0){
        perror ("Error! settimeofday()\n");
        result[1] = '0';
        return (void*)result;        
    }
    result[1] = '1';
    return (void*)result;
}
void* SetTimeCommand::ParseResult(const char *result, const char *filename)
{
    if(!result) {
        return (void*)0;
    }
    char timeStatus = result[1];
    time_t timeSet;
    memcpy(&timeSet, result+3,sizeof(time_t));
/*    FILE* logfile;
    logfile=Shakespeare::open_log("/var/log/job-template",PROCESS);
     // write to log via shakespeare
     if(logfile!=NULL) {
        Shakespeare::log(logfile, Shakespeare::NOTICE, PROCESS, sprintf("Raw seconds elapsed since epoch is %d", rawtime ));
        } 
  */  
    return (void*)1;
}
