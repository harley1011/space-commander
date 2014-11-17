#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <subsystems.h>

#include "SpaceDecl.h"
#include "SpaceString.h"
#include "shakespeare.h"
#include "gettime-command.h"
#include "commands.h"
void* GetTimeCommand::Execute(){
    struct timeval tv;

    char* result = (char*)malloc(sizeof(char) * GETTIME_RTN_SIZE + CMD_HEAD_SIZE);
    printf("Getting time\n");

    if(gettimeofday(&tv, 0) == -1){
        perror ("Error! gettimeofday()\n");
        exit(1);
    }

    time_t* rawtime = (time_t*)malloc(sizeof(time_t));
    *rawtime = tv.tv_sec;

    std::cout << *rawtime << endl;

    memset(result, '\0', sizeof(char) * 50);
    sprintf(result, "%lld", (long long)*rawtime);

    return (void*)result;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ParseResult
*
* PURPOSE : Parse the result buffer returned by the execute function
*
* ARGUMENTS : result    : pointer to the result buffer
*
* RETURNS : struct InfoBytes* to STATIC memory
* 
*-----------------------------------------------------------------------------*/

void* GetTimeCommand::ParseResult(const char *result)
{
    if(!result) {
        return (void*)0;
    }
    static struct InfoBytesGetTime info_bytes = {0};
    info_bytes.time_status = result[1];
    info_bytes.time_set = SpaceString::getTimet(result+CMD_HEAD_SIZE);

    char buffer[80];
   
    if(info_bytes.time_status == CS1_SUCCESS)
       sprintf(buffer,"GetTime success. Time recieved is %u seconds since epoch",(unsigned)info_bytes.time_set);
    else
       sprintf(buffer,"SetTime failure: Unknown",(unsigned)info_bytes.time_set);

    Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER], buffer);
   
    return (void*)&info_bytes;
}
