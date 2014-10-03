#include "reboot-command.h"
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>
#include "subsystems.h"
#include "commands.h"
#include "shakespeare.h"
#include <stdio.h>
extern const char* s_cs1_subsystems[];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Execute
*
* PURPOSE : Reboots the device
*
* RETURNS : A buffer contaning the cmd number and cmd status
* 
*-----------------------------------------------------------------------------*/
void* RebootCommand::Execute(){
    char* result = (char*)malloc(sizeof(char) * REBOOT_RTN_SIZE); 
    reboot(CMD_HEAD_SIZE);
    result[0] = REBOOT_CMD;
    result[1] = CS1_SUCCESS;
    return result;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ParseResult
*
* PURPOSE : Parse the result buffer returned by the reboot function
*
* ARGUMENTS : result    : pointer to the result buffer
*
* RETURNS : struct InfoBytes* to STATIC memory
* 
*-----------------------------------------------------------------------------*/
void* RebootCommand::ParseResult(const char * result)
{
    static struct InfoBytesReboot info_bytes = {0};
    info_bytes.reboot_status = result[1];

    FILE* logfile;
    logfile=Shakespeare::open_log("home/logs",s_cs1_subsystems[COMMANDER]);

    char buffer[60];
    
    if(info_bytes.reboot_status == CS1_SUCCESS)
        sprintf(buffer, "Reboot success.");
    else
        sprintf(buffer, "Reboot failure.");

    if(logfile!=NULL) {
        Shakespeare::log(logfile, Shakespeare::NOTICE,s_cs1_subsystems[COMMANDER], buffer);
    }
    
    return (void*)&info_bytes;

}
