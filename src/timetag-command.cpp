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
        delete command;
        command = NULL;
    }
}

void* TimetagCommand::Execute()
{
    int result = TimetagCommand::AddJob(this->timestamp,this->command);

    // TODO return should be standardized. e.g. [command][exit_status]
    if (result == 0) {
        return (void*)0x00;
    } else {
        return (void*)0x01;
    }
}

/**
 * Function to execute a command and get output
 */
// TODO use fork and kill zombies (if necessary)
std::string TimetagCommand::SysExec(char* orig_cmd) {
  char command[CMD_BUFFER_LEN] = {0};
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
  // TODO perhaps this function should return exit status and write output to data pipe
}

// TODO -> mainly for testing purposes but consider replacing or adding to another library instead of here.
// TODO -> thorough testing
// http://linux.die.net/man/3/strftime
char * TimetagCommand::GetCustomTime(std::string format, time_t rawtime, int moreminutes) {
    char *buffer = (char *) malloc(sizeof(char) * 80);
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo->tm_min = timeinfo->tm_min + moreminutes;
    if ( (timeinfo->tm_min + moreminutes) > 59 ) {
      timeinfo->tm_hour++;
      timeinfo->tm_min = 0 + ( moreminutes - (60-timeinfo->tm_min));
    } else {
      timeinfo->tm_min = timeinfo->tm_min + moreminutes;
    }
    strftime(buffer,80,format.c_str(),timeinfo);
    printf("Formatted date: %s\r\n",buffer);
    return buffer;
}

int TimetagCommand::CancelJob(const int job_id) {
  char cancel_job_command[CMD_BUFFER_LEN] = {0};

  // delete from at spool
  sprintf(cancel_job_command, "atrm %d", job_id);
  std::string output = SysExec(cancel_job_command);
  printf ( "Output: %s\r\n",output.c_str() );
 
  // TODO send to output pipe, store this detailed list remotely only, and reference vs local atq!
  sprintf(cancel_job_command, "sed -i '/^job %d.*/d' schedule.log", job_id);
  output = SysExec(cancel_job_command);
  printf ( "Output: %s\r\n",output.c_str() );

  return 0;
}

int TimetagCommand::AddJob(time_t timestamp, char * executable) {
  if (!IsFileExists(AT_RUNNER) || !IsFileExists(AT_EXEC)){
      return CS1_FILE_DOES_NOT_EXIST;
  }
  char * time_string = TimetagCommand::GetCustomTime(AT_FORMAT, timestamp, 0);
  char add_job_command[CMD_BUFFER_LEN] = {0};
  sprintf(add_job_command, "%s %s %s", AT_RUNNER, time_string, executable);
  printf ( "Commmand: %s\r\n", add_job_command);
  std::string output = SysExec(add_job_command);
  printf ( "Output: %s\r\n",output.c_str() );
  int retval = atoi(output.c_str());
  if (retval <= 0) { retval = -1; }
  return retval;
}
