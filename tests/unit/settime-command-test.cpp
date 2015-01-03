/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Harley
*
* TITLE : settime-command-test.cpp 
*
* DESCRIPTION : Tests the SetTimeCommand class
*
*----------------------------------------------------------------------------*/
#include <time.h>
#include <unistd.h>
#include <dirent.h>     // DIR
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "SpaceDecl.h"
#include "SpaceString.h"
#include "common/command-factory.h"
#include "common/icommand.h"
#include "common/settime-command.h"
#include "fileIO.h"
#include "common/commands.h"
#include "common/subsystems.h"
#include "dirUtl.h"
#include "common/commands.h"

//constant in command_buf
static char command_buf[SETTIME_CMD_SIZE] = {'\0'};
// Test when changing envrionments such as kernel, make sure time_t is either 4 bytes or 8 bytes
//ENDIAN test checker
//Make sure the time is set. How to mock a clock

TEST_GROUP(SetTimeTestGroup)
{
    void setup()
    {
        command_buf[0] = SETTIME_CMD;
    }
    void teardown()
    {

    }
};


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Check_Settime
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime)
{   
    if (getuid() == 0)
    {
        time_t rawtime;
        size_t result_size;
        time(&rawtime);

        SpaceString::getTimetInChar(command_buf+1,rawtime);
        command_buf[SETTIME_CMD_SIZE - 1] = 0xFF;   
        ICommand* command = CommandFactory::CreateCommand(command_buf);
        char* result = (char*)command->Execute(&result_size);
        
        CHECK(result_size == SETTIME_RTN_SIZE + CMD_HEAD_SIZE);

        InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)SetTimeCommand::ParseResult(result);

        CHECK(getsettime_info->time_status == CS1_SUCCESS);
    
        CHECK(getsettime_info->time_set == rawtime);
        time_t newtime;
        time(&newtime);
        CHECK(newtime-rawtime < 1);        

    #ifdef CS1_DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " << rawtime << " time is currently " << newtime << " difference is " << newtime - rawtime << endl;
#endif    
        if ( command != NULL)
        {
            delete command;
            command = NULL;
        }
        if (result) {
            free(result);
            result = 0;
        }
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Endian_Checker
* 
*-----------------------------------------------------------------------------*/

TEST(SetTimeTestGroup, Endian_Checker)
{ 
    short x=0x0100;
    char  temp = (*(char *)&x);
    CHECK(temp == LE);
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Settime_Parseresult
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup,SetTime_ParseResult)
{    
    time_t rawtime = 100;
    memcpy(command_buf+1,&rawtime,sizeof(time_t));
    
    ICommand* command = CommandFactory::CreateCommand(command_buf);
 
    char* result = (char*)malloc(sizeof(char) * 10);
    result[0] = SETTIME_CMD;
    result[1] = CS1_SUCCESS;
    memcpy(result+2,&rawtime, sizeof(time_t));
    InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)SetTimeCommand::ParseResult(result);
    CHECK(getsettime_info->time_set == rawtime);
    CHECK(getsettime_info->time_status == CS1_SUCCESS);
    if (result) {
        free(result);
        result = 0;
    }
    if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Check_Settime_Rtc
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime_Rtc)
{
    std::ifstream ifs;
    ifs.open("/dev/rtc1",std::ifstream::in);
    if ( ifs)
    if (getuid() == 0)//This command can only be executed as root user
    {
        ifs.close();
        time_t rawtime;
        size_t result_buffer;
        time(&rawtime);
        
        SpaceString::getTimetInChar(command_buf+1,rawtime);
        command_buf[SETTIME_CMD_SIZE + CMD_HEAD_SIZE - 1] = 0x01;
        
        ICommand* command = CommandFactory::CreateCommand(command_buf);
        char* result = (char*)command->Execute(&result_buffer);
        CHECK(result_buffer == SETTIME_RTN_SIZE + CMD_HEAD_SIZE);
        InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)SetTimeCommand::ParseResult(result);

        CHECK(getsettime_info->time_status == CS1_SUCCESS);
    
        CHECK(getsettime_info->time_set == rawtime);
        time_t newtime;
        time(&newtime);
        CHECK(newtime-rawtime < 1);        

#ifdef CS1_DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " << rawtime << " time is currently " << newtime << " difference is " << newtime - rawtime << endl;
#endif    
        if ( command != NULL)
        {
            delete command;
            command = NULL;
        }
        if (result) 
        {
            free(result);
            result = 0;
        }
    }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Check_Settime_Fail
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Settime_Fail)
{   
    if (getuid() == 0)
    {
        time_t rawtime = -1;
        size_t result_size; 
        
        SpaceString::getTimetInChar(command_buf+1,rawtime);
        command_buf[SETTIME_CMD_SIZE - 1] = 0xFF;   
        ICommand* command = CommandFactory::CreateCommand(command_buf);
        char* result = (char*)command->Execute(&result_size);
        
        CHECK(result_size == SETTIME_RTN_SIZE + CMD_HEAD_SIZE);    
    
        InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)SetTimeCommand::ParseResult(result);

        CHECK(getsettime_info->time_status == CS1_FAILURE);

        if ( command != NULL)
        {
            delete command;
            command = NULL;
        }
        if (result) {
            free(result);
            result = 0;
        }
    }
}
