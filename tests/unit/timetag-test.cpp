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
#include <sys/types.h>
#include <sys/stat.h>
#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "SpaceString.h"
#include "common/command-factory.h"
#include "common/timetag-command.h"
#include "common/icommand.h"
#include "fileIO.h"
#include "common/commands.h"
#include "dirUtl.h"

#include "Date.h"

#include "SpaceDecl.h"
#define DATETIMEFORMAT "%Y%m%d%H%M"

static char command_buf[TIMETAG_CMD_SIZE] = {'\0'};

TEST_GROUP(TimetagTestGroup)
{
    void setup()
    {
        memset(command_buf, 0, TIMETAG_CMD_SIZE);
    }
    void teardown()
    {
    
    }
};

TEST(TimetagTestGroup, AddJob)
{  
  int test_result=-1;

  char task[72] = "echo \"$(grep \"^${USER}:\" /etc/passwd | cut -d: -f5)\" \\>\\> /tmp/test.log";
  command_buf[0] = TIMETAG_CMD; 
  memcpy(command_buf+1,task,72); 

  TimetagCommand *command = (TimetagCommand*)CommandFactory::CreateCommand(command_buf);

  Date date(2014, 9, 16, 3, 4, 4);
  time_t rawtime = date.GetTimeT();

  int result = command->AddJob(rawtime,task);

  if ( command != NULL ) {
    delete command;
    command = NULL;
  }

  if (result > 0) {
     test_result=0; 
  }
  CHECK_EQUAL(CS1_SUCCESS, test_result);
}

TEST(TimetagTestGroup, CancelJob)
{
  int add_result=-1;

  char task[48] = "grep -HinT --color=auto canceljob /tmp/test.log";
  command_buf[0] = TIMETAG_CMD; 
  memcpy(command_buf+1,task,48); // TODO isn't copying the task here redundant?

  TimetagCommand *command = (TimetagCommand*)CommandFactory::CreateCommand(command_buf);

  Date date(2014, 9, 15, 10, 10, 10);
  time_t rawtime = date.GetTimeT();

  int result = command->AddJob(rawtime,task);

  if (command != NULL ) {
    delete command;
    command = NULL;
  }

  if (result > 0) {
     add_result=0; 
  }
  CHECK_EQUAL(CS1_SUCCESS, add_result);

  int cancel_result = command->CancelJob(result); 
  CHECK_EQUAL(CS1_SUCCESS, cancel_result);
}

TEST(TimetagTestGroup, CommandExecution)
{
  size_t result_buffer_size = sizeof(InfoBytesTimetag);
  int test_result = -1;
  
  command_buf[0] = TIMETAG_CMD; 

  Date date(2014, 9, 15, 10, 10, 10);
  time_t rawtime = date.GetTimeT();
  memcpy(command_buf+1,&rawtime,sizeof(time_t)); 

  char task[48] = "grep -HinT --color=auto execution /tmp/test.log";
  memcpy(command_buf+9,task,48); 

  ICommand* command = CommandFactory::CreateCommand(command_buf);
  unsigned char * execute_result = (unsigned char*)((TimetagCommand*)command)->Execute(&result_buffer_size);

  InfoBytesTimetag timetag_infobytes; 
  unsigned char * result_buffer = (unsigned char*)((TimetagCommand*)command)->ParseResult(execute_result);
  timetag_infobytes = (InfoBytesTimetag&)result_buffer; // TODO fix segfault

  if (command != NULL ) {
    delete command;
    command = NULL;
  }
  if (execute_result != NULL ) {
    free (execute_result);
  }
  if (timetag_infobytes.job_id > 0) {
    test_result = 0; 
  }
  CHECK_EQUAL(CS1_SUCCESS,test_result);
}

// TODO check the job_command on both ends 
