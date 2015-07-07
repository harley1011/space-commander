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
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "SpaceDecl.h"
#include "SpaceString.h"
#include "common/command-factory.h"
#include "common/getlog-command.h"
#include "common/icommand.h"
#include "common/gettime-command.h"
#include "fileIO.h"
#include "common/commands.h"
#include "common/subsystems.h"
#include "dirUtl.h"

static char command_buf[GETTIME_CMD_SIZE] = {'\0'};

TEST_GROUP(GetTimeTestGroup)
{
    void setup() {
        command_buf[0] = GETTIME_CMD;
    }

    void teardown() {
        //*/
    }
};


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : GetTimeTestGroup
*
* NAME : Check_Gettime_ParseResult
* 
*-----------------------------------------------------------------------------*/
TEST(GetTimeTestGroup, Check_Gettime)
{   
    ICommand* command = CommandFactory::CreateCommand(command_buf);
    size_t size;
    char* result = (char*)command->Execute(&size);
    InfoBytesGetTime* gettime_info = (InfoBytesGetTime*)command->ParseResult(result);

    CHECK(gettime_info->time_status == CS1_SUCCESS);
    CHECK(size == (sizeof(time_t) + CMD_RES_HEAD_SIZE));
    time_t newtime;
    time(&newtime);
    CHECK(newtime-gettime_info->time_set < 1);        

    #ifdef CS1_DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << "Raw Seconds elapsed from gettime is " << gettime_info->time_set 
                  << " time is currently " << newtime << " difference is " << newtime - gettime_info->time_set 
                  << endl;
    #endif    

    if (command != NULL) {
        delete command;
        command = NULL;
    }

    if (result) {
        free(result);
        result = 0;
     }
}
