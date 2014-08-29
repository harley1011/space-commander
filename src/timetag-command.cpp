/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Shawn 
*
* TITLE : timetag-command.cpp
*
*----------------------------------------------------------------------------*/
#include "timetag-command.h"
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
#define LOG_PATH        "/home/logs/commander/"
#define PROCESS         "TIMETAG"
#define CMD_BUFFER_LEN  190
#define AT_RUNNER       "/usr/bin/at-runner.sh"
#define AT_EXEC         "/usr/bin/at"
#define AT_FORMAT       "%Y%m%d%H%M"

TimetagCommand::TimetagCommand()
{
    this->command = 0x0;
    this->timestamp = 0x0;
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

void* TimetagCommand::Execute()
{
    // TODO, all dynamic memory allocation should be performed with new/delete, not malloc/free
    unsigned char * execute_result = (unsigned char *) malloc (TIMETAG_CMD_SIZE);
    short int job_id = TimetagCommand::AddJob(this->timestamp,this->command);
    struct TimetagBytes info_bytes;
    info_bytes.job_id = job_id;
    info_bytes.job_timestamp = this->timestamp;
    info_bytes.job_command = this->command;
    memcpy (execute_result, &info_bytes, TIMETAG_CMD_SIZE); 
    return (void*)execute_result;
}

void* TimetagCommand::ParseResult(unsigned char * timetag_result_bytes)
{
  // TODO log results with shakespeare
  static struct TimetagBytes result_struct = {0};
  memcpy (&result_struct,timetag_result_bytes,TIMETAG_CMD_SIZE); 
  char log_entry[255];
  sprintf (log_entry,"%d ",result_struct.job_id);
  sprintf (log_entry,"%ld ",result_struct.job_timestamp);
  sprintf (log_entry,"%s",result_struct.job_command);
  Shakespeare::log_shorthand(LOG_PATH, Shakespeare::NOTICE, PROCESS, log_entry);
  return (void*)&result_struct;
}

// TODO -> mainly for testing purposes but consider replacing or adding to another library instead of here.
// TODO -> thorough testing
// http://linux.die.net/man/3/strftime
int TimetagCommand::GetCustomTime(std::string format, char * output_date, int output_length, time_t rawtime) 
{
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    return strftime(output_date,output_length,format.c_str(),timeinfo);
}

int TimetagCommand::CancelJob(const int job_id) 
{
  char cancel_job_command[CMD_BUFFER_LEN] = {0};

  // delete from at spool
  sprintf(cancel_job_command, "atrm %d", job_id);
  SystemProcess * process = new SystemProcess(cancel_job_command);
  std::string output = process->Execute();
  delete process;

  return atoi(output.c_str());
}

int TimetagCommand::AddJob(time_t timestamp, char * executable) 
{
  if (!IsFileExists(AT_RUNNER) || !IsFileExists(AT_EXEC)){
      return CS1_FILE_DOES_NOT_EXIST;
  }
  char time_string[13] = {0};
  TimetagCommand::GetCustomTime(AT_FORMAT, time_string, 13, timestamp);

  char add_job_command[CMD_BUFFER_LEN] = {0};
  sprintf(add_job_command, "%s %s %s\r\n", AT_RUNNER, time_string, executable);

  SystemProcess * process = new SystemProcess(add_job_command);
  std::string output = process->Execute();
  delete process;

  int retval = atoi(output.c_str());

  if (retval <= 0) { retval = -1; }
  return retval;
}
