#include "settime-command.h"
#include <string.h>
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SpaceString.h>
#include <shakespeare.h>
#include "commands.h"
#include "SpaceDecl.h"
#include "subsystems.h"

extern const char* s_cs1_subsystems[];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : SetTimeCommand
*
* ARGUMENTS : time  : input - time to set 
* 
*-----------------------------------------------------------------------------*/
SetTimeCommand::SetTimeCommand(time_t time) {
    this->seconds = time;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Execute
*
* PURPOSE : Sets the time of the device to 'time'
*
* RETURNS : A buffer contaning the cmd number, cmd status, and time set
* 
*-----------------------------------------------------------------------------*/
void* SetTimeCommand::Execute(){
    struct timeval tv;
    char *result;
    result = (char*)malloc(sizeof(char) * SETTIME_RTN_SIZE);
    
    result[0] = SETTIME_CMD;
    tv.tv_sec = GetSeconds();   
    tv.tv_usec = 0;
    memcpy(result+2, &tv.tv_sec, sizeof(time_t)); 
    if (settimeofday(&tv, 0) != 0){
        perror ("Error! settimeofday()\n");
        result[1] = CS1_FAILURE;
        return (void*)result;        
    }
    result[1] = CS1_SUCCESS;
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

void* SetTimeCommand::ParseResult(const char *result)
{
    if(!result) {
        return (void*)0;
    }
    static struct InfoBytesSetTime info_bytes = {0};
    info_bytes.time_status = result[1];
    info_bytes.time_set = SpaceString::getTimet(result+2);

    FILE* logfile;
    logfile=Shakespeare::open_log("/home/logs",s_cs1_subsystems[COMMANDER]);
    
    char buffer[80];
   
    if(info_bytes.time_status == CS1_SUCCESS)
       sprintf(buffer,"SetTime success. Time set to %d seconds since epoch",info_bytes.time_set);
    else
       sprintf(buffer,"SetTime failure. Time failed to set %d seconds since epoch",info_bytes.time_set);

   if(logfile!=NULL) {
        Shakespeare::log(logfile, Shakespeare::NOTICE, s_cs1_subsystems[COMMANDER], buffer);
        } 
   
    return (void*)&info_bytes;
}
