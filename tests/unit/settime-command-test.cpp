/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph
*
* TITLE : getlog-command-test.cpp 
*
* DESCRIPTION : Tests the GetLogCommand class
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
* NAME : GetInfoBytes_returnsCorrectInfoBytes
* 
*-----------------------------------------------------------------------------*/
TEST(SetTimeTestGroup, Check_Time)
{

    time_t rawtime;
    time(&rawtime);
    SetTimeCommand* command = new SetTimeCommand(rawtime);
    std::cerr << "[DEBUG] " << __FILE__ << "Seconds elapsed " << command->GetSeconds() << endl; 
    if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
}
TEST(SetTimeTestGroup, Check_Command)
{
    time_t rawtime;
    time(&rawtime);

    command_buf[0] = '0';
    memcpy(command_buf+1,&rawtime,8);
    int* result = 0; 
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    result = (int*)command->Execute();
    CHECK(result);    
    std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed " << rawtime << endl;
/*    FILE* logfile;
    logfile=Shakespeare::open_log("/var/log/job-template",PROCESS);
     // write to log via shakespeare
     if(logfile!=NULL) {
        Shakespeare::log(logfile, Shakespeare::NOTICE, PROCESS, sprintf("Raw seconds elapsed since epoch is %d", rawtime ));
        } */
    if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
   // STRCMP_EQUAL(15, day);
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
    time_t test;
    CHECK(sizeof(test) == SETTIME_CMD_SIZE - 1 );
}
