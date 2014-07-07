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

static char command_buf[2] = {'\0'};



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
    command_buf[1] = rawtime;
    int* result = 0; 
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    result = (int*)command->Execute();
    CHECK(result);    
   if ( command != NULL)
    {
        delete command;
        command = NULL;
    }
   // STRCMP_EQUAL(15, day);
}
