#include <cerrno>
#include <linux/rtc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <SpaceString.h>
#include <shakespeare.h>

#include "common/commands.h"
#include "common/settime-command.h"
#include "common/subsystems.h"
#include "i2c-device.h"
#include "SpaceDecl.h"

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
SetTimeCommand::SetTimeCommand(time_t time, char rtc_bus_number) {
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
void* SetTimeCommand::Execute(size_t* pSize){
    struct timeval tv = {0};
    char *result = 0;

    result = (char*)malloc(sizeof(char) * (SETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE) );
    *pSize = SETTIME_RTN_SIZE + CMD_RES_HEAD_SIZE;
    result[CMD_ID] = SETTIME_CMD;
    result[CMD_STS] = CS1_SUCCESS;
    tv.tv_sec = this->GetSeconds();   
    tv.tv_usec = 0;
    memcpy(result + CMD_RES_HEAD_SIZE, &tv.tv_sec, sizeof(time_t)); 

    if (settimeofday(&tv, 0) != 0) {
        result[CMD_STS] = CS1_FAILURE;
        return (void*)result;        
    }

    if (rtc_bus_number != EOF) {
        int rtc_bus_number_convert = (int)rtc_bus_number;
        struct tm * time_info = localtime(&tv.tv_sec);
        struct rtc_time rt = {  time_info->tm_sec, 
                                time_info->tm_min, 
                                time_info->tm_hour, 
                                time_info->tm_mday, 
                                time_info->tm_mon, 
                                time_info->tm_year, 
                                time_info->tm_wday,
                                time_info->tm_yday,
                                time_info->tm_isdst
                              };
        if (I2CDevice::I2CWriteToRTC(rt,rtc_bus_number_convert) == -1) {
            result[CMD_STS] = CS1_FAILURE; 
        }
    }

    return (void*)result;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : ParseResult
 *
 * PURPOSE : Parse the result buffer returned by the execute function and logs it with shakespeare
 *
 * ARGUMENTS : result    : pointer to the result buffer
 *
 * RETURNS : struct InfoBytes* to STATIC memory
 *
 * NOTES : Logs with the Ground Commander tag because this only is run on the 
 *         Ground
 * 
 *-----------------------------------------------------------------------------*/
InfoBytes* SetTimeCommand::ParseResult(char *result) {
    static struct InfoBytesSetTime info_bytes;

    if (!result || result[CMD_ID] != SETTIME_CMD) {
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_GND_COMMANDER],"Possible SetTime failure: Can't parse result");
        info_bytes.time_status = CS1_FAILURE;

        return &info_bytes;
    }

    info_bytes.time_status = result[CMD_STS];
    info_bytes.time_set = SpaceString::getTimet(result + CMD_RES_HEAD_SIZE);

    char buffer[CS1_MAX_LOG_ENTRY];
   
    if (info_bytes.time_status == CS1_SUCCESS) {
        snprintf(buffer, CS1_MAX_LOG_ENTRY,
                    "SetTime success: Time set to %u seconds since epoch", (unsigned)info_bytes.time_set); 
        Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_GND_COMMANDER], buffer);
    } else {
        snprintf(buffer, CS1_MAX_LOG_ENTRY,
                    "SetTime failure: Time failed to set %u seconds since epoch", (unsigned)info_bytes.time_set);
        Shakespeare::log(Shakespeare::ERROR, cs1_systems[CS1_GND_COMMANDER], buffer);
    }

    return &info_bytes;
}
