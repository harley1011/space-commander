#include "timetag-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#define CMD_BUFFER_LEN 190
#define AT_RUNNER "/usr/bin/at-runner.sh"

void* TimetagCommand::Execute(){
    FILE* at_file = NULL;
    // schedule stuff

    return NULL;
}

/**
 * Function to execute a command and get output
 */
// TODO use fork and kill zombies (if necessary?)!
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

int TimetagCommand::CancelJob(const int job_id) {
  char cancel_job_command[CMD_BUFFER_LEN] = {0};

  // delete from at spool
  sprintf(cancel_job_command, "atrm %d", job_id);
  std::string output = sysexec(cancel_job_command);
  printf ( "Output: %s",output.c_str() );
 
  // TODO send to output pipe, store this detailed list remotely only, and reference vs local atq!
  sprintf(cancel_job_command, "sed -i '/^job %d.*/d' schedule.log", job_id);
  output = sysexec(cancel_job_command);
  printf ( "Output: %s",output.c_str() );

  return 0;
}

int TimetagCommand::TimetagCommand(char * date_time, char * executable) {
  char add_job_command[CMD_BUFFER_LEN] = {0};
  sprintf(add_job_command, "sh %s %s %s", AT_RUNNER, date_time, executable);
  printf(add_job_command);
  std::string output = sysexec(add_job_command);
  printf ( "Output: %s",output.c_str() );
  int retval = atoi(output.c_str());
  if (retval <= 0) { retval = -1; }
  free (date_time);
  free (executable);
  return retval;
}
