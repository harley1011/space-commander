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

static char command_buf[6] = {'\0'};



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
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    command_buf[0] = '0';
    command_buf[1] = timeinfo->tm_year;
    command_buf[2] = timeinfo->tm_mon;
    command_buf[3] = timeinfo->tm_mday;
    command_buf[4] = timeinfo->tm_hour;
    command_buf[5] = timeinfo->tm_min;
    command_buf[6] = timeinfo->tm_sec; 
    SetTimeCommand* command = new SetTimeCommand(14,6,15,16,43,55);
//    ICommand* command = CommandFactory::CreateCommand(command_buf);
   char c = command->GetDay();
    //CHECK(c==16);
    std::cerr << "[DEBUG] " << __FILE__ << "Seconds elapsed " << command->GetSeconds() << endl; 
    if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
   // STRCMP_EQUAL(15, day);
}
TEST(SetTimeTestGroup, Check_Command)
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    command_buf[0] = '0';
    command_buf[1] = timeinfo->tm_year;
    command_buf[2] = timeinfo->tm_mon;
    command_buf[3] = timeinfo->tm_mday;
    command_buf[4] = timeinfo->tm_hour;
    command_buf[5] = timeinfo->tm_min;
    command_buf[6] = timeinfo->tm_sec; 
     
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    command->Execute();
    
   if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
   // STRCMP_EQUAL(15, day);
}
