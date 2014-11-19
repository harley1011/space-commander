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
    result[0] = GETTIME_CMD;
    result[1] = CS1_SUCCESS;
    if(gettimeofday(&tv, 0) == -1){
        result[1] = CS1_FAILURE;
        return (void*)result;
    }

    time_t* rawtime = (time_t*)malloc(sizeof(time_t));
    *rawtime = tv.tv_sec;

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
    if(!result || result[0] != GETTIME_CMD) {
        Shakespeare::log(Shakespeare::NOTICE,cs1_systems[CS1_COMMANDER],"GetTime failure: Can't parse result");
        return (void*)0;
    }
    static struct InfoBytesGetTime info_bytes = {0};
    info_bytes.time_status = result[1];
    info_bytes.time_set = SpaceString::getTimet(result+CMD_HEAD_SIZE);

    char buffer[100];
   
    if(info_bytes.time_status == CS1_SUCCESS)
       snprintf(buffer,100,"GetTime success. Time recieved is %u seconds since epoch",(unsigned)info_bytes.time_set);
    else
       snprintf(buffer,100,"GetTime failure: Unknown");

    Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER], buffer);
   
    return (void*)&info_bytes;
}
