#include "settime-command.h"
#include <string.h>
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/rtc.h>
#include <SpaceString.h>
#include <shakespeare.h>
#include "i2c-device.h"
#include "commands.h"
#include "SpaceDecl.h"
#include "subsystems.h"
extern const char* s_cs1_subsystems[];
const char* ST_LOGNAME = cs1_systems[CS1_COMMANDER];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : SetTimeCommand
*
* ARGUMENTS : time  : input - time to set 
* 
*-----------------------------------------------------------------------------*/
SetTimeCommand::SetTimeCommand(time_t time) {
    this->seconds = time;
    this->rtc_bus_number = -1;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : SetTimeCommand
*
* ARGUMENTS : time  : input - time to set 
* 
*-----------------------------------------------------------------------------*/
SetTimeCommand::SetTimeCommand(time_t time, int rtc_bus_number) {
    this->seconds = time;
    this->rtc_bus_number = rtc_bus_number;
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
    result[1] = CS1_FAILURE;
    tv.tv_sec = GetSeconds();   
    tv.tv_usec = 0;
    memcpy(result+2, &tv.tv_sec, sizeof(time_t)); 
    if (settimeofday(&tv, 0) != 0){
        #ifdef CS1_DEBUG
            perror ("Error! settimeofday()\n");
        #endif
        result[1] = CS1_FAILURE;
        return (void*)result;        
    }
    if (rtc_bus_number > -1)
    {
        struct tm * time_info = localtime(&tv.tv_sec);
        struct rtc_time rt = { time_info->tm_sec, time_info->tm_min, time_info->tm_hour, time_info->tm_mday, time_info->tm_mon, time_info->tm_year, time_info->tm_wday,time_info->tm_yday,time_info->tm_isdst};
        if (I2CDevice::I2CWriteToRTC(rt,rtc_bus_number) == -1)
            result[1] = CS1_FAILURE; 


    }
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

    char buffer[80];
   
    if(info_bytes.time_status == CS1_SUCCESS)
       sprintf(buffer,"SetTime success. Time set to %u seconds since epoch",(unsigned)info_bytes.time_set);
    else
       sprintf(buffer,"SetTime failure. Time failed to set %u seconds since epoch",(unsigned)info_bytes.time_set);

    Shakespeare::log(Shakespeare::NOTICE, ST_LOGNAME, buffer);
   
    return (void*)&info_bytes;
}
