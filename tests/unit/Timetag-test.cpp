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
        memset(command_buf, 0, GETLOG_CMD_SIZE);
    }
    void teardown(){
    }
};

void create_file(const char* path, const char* msg)
{
    FILE* file = fopen(path, "w+");
    fprintf(file, "%s", msg);
    fclose(file);
}

TEST(TimetagTestGroup, AddJob)
{  
  char * date_time = TimetagCommand::GetCustomTime(DATETIMEFORMAT,5);
  char task[72] = "echo \"$(grep \"^${USER}:\" /etc/passwd | cut -d: -f5)\" \\>\\> /tmp/test.log";
  int result = TimetagCommand::AddJob(date_time,task);

  CHECK_EQUAL(0, result);
}

TEST(TimetagTestGroup, CancelJob)
{
  char * date_time = TimetagCommand::GetCustomTime(DATETIMEFORMAT,5);
  char task[48] = "grep -HinT --color=auto something /tmp/test.log";
  int result = Timetagcommand::AddJob(date_time,task);
  result = TimetagCommand::CancelJob(result); 
  CHECK_EQUAL(0, result);
}

/* 
TEST(TimetagTestGroup, EscapeCharacters)
{
  char to_escape[3] = {'<','>'}; // TODO NOT COMPHREHENSIVE
  char * test_string = "This is a << >> test string";
  char * result = escape_characters (test_string, 28, to_escape, 2);
  char * expected = "This is a \\<\\< \\>\\> test string";
  printf("Result: '%s'\r\n",result);
  printf("Expect: '%s'\r\n",expected);

  size_t limit = 31; 
  size_t z=0;
  for (z=0; z<limit; z++) { 
    CHECK_EQUAL(
        expected[z], result[z]
    );
  }
}
*/
