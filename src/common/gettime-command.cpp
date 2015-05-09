#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include "common/subsystems.h"

#include "SpaceDecl.h"
#include "SpaceString.h"
#include "shakespeare.h"
#include "common/gettime-command.h"
#include "common/commands.h"

void* GetTimeCommand::Execute(size_t * pSize){
    struct timeval tv;
    char* result; 
    result = (char*)malloc(sizeof(char) * GETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE);
    *pSize = GETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE;
    
    result[0] = GETTIME_CMD;
    result[1] = CS1_SUCCESS;
    if(gettimeofday(&tv, 0) == -1){
        result[1] = CS1_FAILURE;
        return (void*)result;
    }
    memcpy(result+CMD_RES_HEAD_SIZE, &tv.tv_sec, sizeof(time_t));
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

InfoBytes* GetTimeCommand::ParseResult(char *result)
{
    static struct InfoBytesGetTime info_bytes;
    if(!result || result[0] != GETTIME_CMD) {
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER],"GetTime failure: Can't parse result");
        info_bytes.time_status = CS1_FAILURE;
        return &info_bytes;
    }
    info_bytes.time_status = result[1];
    info_bytes.time_set = SpaceString::getTimet(result+CMD_RES_HEAD_SIZE);

    char buffer[100];
    struct tm *time_info = localtime(&info_bytes.time_set); 
    if(info_bytes.time_status == CS1_SUCCESS)
    {    
        snprintf(buffer,100,"GetTime success. Time recieved is %u seconds since epoch or %i-%i-%i %i:%i:%i",(unsigned)info_bytes.time_set,1900 + time_info->tm_year, time_info->tm_mon,time_info->tm_mday,time_info->tm_hour,time_info->tm_min,time_info->tm_sec);
        Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER], buffer);    
    }
    else
    {
        snprintf(buffer,100,"GetTime failure: Unknown");
        Shakespeare::log(Shakespeare::ERROR, cs1_systems[CS1_COMMANDER], buffer);
   }
    return &info_bytes;
}
