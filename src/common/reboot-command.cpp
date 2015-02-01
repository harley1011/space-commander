#include "common/reboot-command.h"
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>
#include "common/subsystems.h"
#include "common/commands.h"
#include "shakespeare.h"
#include <stdlib.h>
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
void* RebootCommand::Execute(size_t* pSize){
    char* result = (char*)malloc(sizeof(char) * CMD_HEAD_SIZE);
    *pSize = CMD_HEAD_SIZE; 
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
    if (!result || result[0] != REBOOT_CMD ){
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER],"Reboot failure: Can't parse result");
        return (void*)0;    
}
    static struct InfoBytesReboot info_bytes;
    info_bytes.reboot_status = result[1];
    char buffer[60];
    
    if(info_bytes.reboot_status == CS1_SUCCESS)
    {
        sprintf(buffer, "Reboot success.");
        Shakespeare::log(Shakespeare::NOTICE,s_cs1_subsystems[COMMANDER], buffer);
    }
    else
    {
        sprintf(buffer, "Reboot failure.");
        Shakespeare::log(Shakespeare::ERROR,s_cs1_subsystems[COMMANDER], buffer);
    }
    return (void*)&info_bytes;

}
