/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Shawn
*
* TITLE : timetag-test.cpp 
*
* DESCRIPTION : Tests the TimetagCommand class
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
#include "timetag-command.h"
#include "icommand.h"
#include "fileIO.h"
#include "commands.h"
#include "subsystems.h"
#include "dirUtl.h"

static char command_buf[GETLOG_CMD_SIZE] = {'\0'};

static void create_file(const char* path, const char* msg);

#define UTEST_SIZE_OF_TEST_FILES 6
static const char* data_6_bytes = "123456";

TEST_GROUP(TimetagTestGroup)
{
    void setup(){
        mkdir(CS1_TGZ, S_IRWXU);
        memset(command_buf, 0, GETLOG_CMD_SIZE);

    }
    void teardown(){
        DeleteDirectoryContent(CS1_TGZ);
        rmdir(CS1_TGZ);
    }
};

void create_file(const char* path, const char* msg)
{
    FILE* file = fopen(path, "w+");
    fprintf(file, "%s", msg);
    fclose(file);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* GROUP : TimetagTestGroup
*
* NAME : GetInfoBytes_returnsCorrectInfoBytes
* 
*-----------------------------------------------------------------------------*/
TEST(TimetagTestGroup, AddJob)
{  char * date_time = get_custom_time(DATETIMEFORMAT,5);
  //char task[43] = "/bin/echo $(date --iso) \\>\\> /tmp/test.log";
  char task[72] = "echo \"$(grep \"^${USER}:\" /etc/passwd | cut -d: -f5)\" \\>\\> /tmp/test.log";
  //char task[41] = "/usr/bin/touch /tmp/test.log";
  //char task[48] = "grep -HinT --color=auto something /tmp/test.log";
  int result = TimetagCommand::AddJob(date_time,task);

  CHECK_EQUAL(-1, result);
}

