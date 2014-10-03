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

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "SpaceDecl.h"
#include "SpaceString.h"
#include "command-factory.h"
#include "getlog-command.h"
#include "icommand.h"
#include "fileIO.h"
#include "commands.h"
#include "subsystems.h"
#include "dirUtl.h"
#include "commands.h"
#include "subsystems.h"
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
    time_t rawtime;
    time(&rawtime);

    SpaceString::getTimetInChar(command_buf+1,rawtime);
//    memcpy(command_buf+1,&rawtime,sizeof(rawtime));
    
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    char* result = (char*)command->Execute();
   
    InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)command->ParseResult(result);

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
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : SetTimeTestGroup
*
* NAME : Check_Bytes_Of_Timet_On_System
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Bytes_Of_Timet_On_System)
{
    CHECK(sizeof(time_t) ==  8 );
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
    InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)command->ParseResult(result);
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
