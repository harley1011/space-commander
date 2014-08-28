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
#define PROCESS "settime"
//constant in command_buf
static char command_buf[SETTIME_CMD_SIZE] = {'\0'};
// Test when changing envrionments such as kernel, make sure time_t is either 4 bytes or 8 bytes
//ENDIAN test checker
//Make sure the time is set. How to mock a clock

TEST_GROUP(SetTimeTestGroup)
{
    void setup()
    {

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

    command_buf[0] = '0';
  //command_buf[1] = SpaceString::get8Char(command_buf+1,rawtime);
    memcpy(command_buf+1,&rawtime,sizeof(rawtime));
    
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    char* result = (char*)command->Execute();
   
    InfoBytesSetTime* getsettime_info = (InfoBytesSetTime*)command->ParseResult(result);

    CHECK(getsettime_info->time_status == '1');
    
    CHECK(getsettime_info->time_set == rawtime);
    time_t newtime;
    time(&newtime);
    CHECK(newtime-rawtime < 1);        

    std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " << rawtime << " time is currently " << newtime << " difference is " << newtime - rawtime << endl;
   /* FILE* logfile;
    logfile=Shakespeare::open_log("/var/log/job-template",PROCESS);
     // write to log via shakespeare
     if(logfile!=NULL) {
        Shakespeare::log(logfile, Shakespeare::NOTICE, PROCESS, sprintf("Raw seconds elapsed since epoch is %d", rawtime ));
        }*/ 
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
    CHECK(sizeof(time_t) ==  SIZEOF_TIMET );
}
TEST(SetTimeTestGroup, Endian_Checker)
{ 
    short x=0x0100;
    char  temp = (*(char *)&x);
    CHECK(temp == LE);
}

