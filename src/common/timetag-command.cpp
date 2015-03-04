/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Shawn 
*
* TITLE : timetag-command.cpp
*
*----------------------------------------------------------------------------*/
#include "common/timetag-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <SpaceDecl.h>
#include <time.h>
#include <string.h>
#include <fileIO.h>
#include <iostream>
#include <SystemProcess.h>
#include <shakespeare.h>
#define PROCESS         "TIMETAG"
#define AT_RUNNER       "/usr/bin/at-runner.sh"
#define AT_EXEC         "/usr/bin/at"
#define AT_FORMAT       "%Y%m%d%H%M"

#define TIMETAG_JOB_ID_INDEX        0
#define TIMETAG_JOB_TIMESTAMP_INDEX sizeof(int)
#define TIMETAG_JOB_COMMAND_INDEX   sizeof(int)+sizeof(time_t)

TimetagCommand::TimetagCommand()
{
    this->command = 0;
    this->timestamp = 0;
}

TimetagCommand::TimetagCommand(char * command, time_t timestamp)
{
    this->command = command;
    this->timestamp = timestamp;
}

TimetagCommand::~TimetagCommand()
{
    if (command != NULL) {
        free (command);
        command = NULL;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : Execute
 * DESCRIPTION: TODO
 *
 *-------------------------------------------------------------------------------*/
void* TimetagCommand::Execute(size_t *pSize)
{
    short int job_id = TimetagCommand::AddJob(this->timestamp,this->command);
    unsigned char * timetag_result;
    size_t timetag_instance_length = 0;

    // if (! timetag_instance_length <= TIMETAG_CMD_SIZE) // TODO {}
    
    // TODO, all dynamic memory allocation should be performed with new/delete, not malloc/free
    timetag_result = (unsigned char *) malloc (timetag_instance_length);

    timetag_result[0] = job_id;
    timetag_result[TIMETAG_JOB_TIMESTAMP_INDEX] = this->timestamp;
    memcpy (timetag_result+TIMETAG_JOB_COMMAND_INDEX, this->command, TIMETAG_MAX_JOB_COMMAND); 

    return (void *) timetag_result;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : ParseResult  
 * DESCRIPTION: Parses the result beffer returned from Execute into the InfoBytes
 * format
 *
 *-------------------------------------------------------------------------------*/
InfoBytes* TimetagCommand::ParseResult(const unsigned char * timetag_result_bytes)
{
  InfoBytesTimetag * timetag_infobytes;
  timetag_infobytes->job_id = timetag_result_bytes[0];
  timetag_infobytes->job_timestamp = timetag_result_bytes[0];
  memcpy (&timetag_infobytes->job_command,
          (char *)timetag_result_bytes[TIMETAG_JOB_COMMAND_INDEX],
          TIMETAG_MAX_JOB_COMMAND); 
#if CS1_DEBUG
  char log_entry[255];
  snprintf (
          log_entry,
          CMD_BUFFER_LEN, 
          "%d %ld %s",
          timetag_infobytes->job_id,
          timetag_infobytes->job_timestamp,
          timetag_infobytes->job_command
  );
  Shakespeare::log(Shakespeare::NOTICE, PROCESS, log_entry);
#endif 
  return (InfoBytes *) timetag_infobytes;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : GetCustomTime
 * DESCRIPTION: TODO
 *
 *-------------------------------------------------------------------------------*/
// TODO -> mainly for testing purposes but consider replacing or adding to another library instead of here.
// TODO -> thorough testing
// http://linux.die.net/man/3/strftime
int TimetagCommand::GetCustomTime(std::string format, char * output_date, int output_length, time_t rawtime) 
{
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    return strftime(output_date,output_length,format.c_str(),timeinfo);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : CancelJob
 * DESCRIPTION: TODO
 *
 *-------------------------------------------------------------------------------*/
int TimetagCommand::CancelJob(const int job_id) 
{
  char cancel_job_command[CMD_BUFFER_LEN] = {0};

  // delete from at spool
  sprintf(cancel_job_command, "atrm %d", job_id);
  SystemProcess * process = new SystemProcess();
  std::string output = process->Execute(cancel_job_command);
  delete process;

  return atoi(output.c_str());
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * NAME : AddJob
 * DESCRIPTION: TODO
 *
 *-------------------------------------------------------------------------------*/
int TimetagCommand::AddJob(time_t timestamp, char * executable) 
{
  if (!IsFileExists(AT_RUNNER) || !IsFileExists(AT_EXEC)){
      return CS1_FILE_DOES_NOT_EXIST;
  }
  char time_string[13] = {0};
  TimetagCommand::GetCustomTime(AT_FORMAT, time_string, 13, timestamp);

  char add_job_command[CMD_BUFFER_LEN] = {0};
  sprintf(add_job_command, "%s %s %s\r\n", AT_RUNNER, time_string, executable);

  SystemProcess * process = new SystemProcess();
  std::string output = process->Execute(add_job_command);
  delete process;

  int retval = atoi(output.c_str());

  if (retval <= 0) { retval = -1; }
  return retval;
}
