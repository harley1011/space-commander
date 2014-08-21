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
#define CMD_BUFFER_LEN 190
#define AT_RUNNER "/usr/bin/at-runner.sh"
#define AT_EXEC "/usr/bin/at"
#define AT_FORMAT "%Y%m%d%H%M"

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
  printf ("JOB ID: %d\r\n",result_struct.job_id);
  printf ("JOB TIME: %ld\r\n",result_struct.job_timestamp);
  printf ("JOB COMMAND: %s\r\n",result_struct.job_command);
  return (void*)&result_struct;
}

/**
 * Function to execute a command and get output
 */
// TODO use fork and kill zombies (if necessary)
std::string TimetagCommand::SysExec(char* orig_cmd) {
  char command[TIMETAG_MAX_JOB_COMMAND] = {0};
  sprintf(command, orig_cmd, "2>&1"); // redirect stderr to stdout

  FILE * exec_pipe = popen(command, "r");
  if (!exec_pipe) return "ERROR, failed to open pipe";
  char buffer[CMD_BUFFER_LEN];
  std::string result = "";
  while( !feof(exec_pipe) ) {
    if ( fgets(buffer, CMD_BUFFER_LEN, exec_pipe) != NULL ) {
      result+=buffer;
    }
  }
  pclose(exec_pipe);
  return result;
}

// TODO -> mainly for testing purposes but consider replacing or adding to another library instead of here.
// TODO -> thorough testing
// http://linux.die.net/man/3/strftime
int TimetagCommand::GetCustomTime(std::string format, char * output_date, int output_length, time_t rawtime) {
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    return strftime(output_date,output_length,format.c_str(),timeinfo);
}

int TimetagCommand::CancelJob(const int job_id) {
  char cancel_job_command[CMD_BUFFER_LEN] = {0};

  // delete from at spool
  sprintf(cancel_job_command, "atrm %d", job_id);
  std::string output = SysExec(cancel_job_command);

  return atoi(output.c_str());
}

int TimetagCommand::AddJob(time_t timestamp, char * executable) {
  if (!IsFileExists(AT_RUNNER) || !IsFileExists(AT_EXEC)){
      return CS1_FILE_DOES_NOT_EXIST;
  }
  char time_string[13] = {0};
  TimetagCommand::GetCustomTime(AT_FORMAT, time_string, 13, timestamp);

  printf ("Formatted date: %s\r\n",time_string);

  char add_job_command[CMD_BUFFER_LEN] = {0};
  sprintf(add_job_command, "%s %s %s\r\n", AT_RUNNER, &time_string, executable);

  printf ( "Command: %s\r\n", add_job_command);

  std::string output = SysExec(add_job_command);

  int retval = atoi(output.c_str());

  printf ( "Job ID: %d\r\n, ", retval );

  if (retval <= 0) { retval = -1; }
  return retval;
}


