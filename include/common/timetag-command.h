/* Space Concordia 
 *
 * NAME: Shawn
 *
 * TITLE: timetag-command.h
 *
 * DESCRIPTION: Schedule a command to be run on the satellite shell
 *  providing the command to be run, and a timestamp of the time
 *  the comannd should be executed
 *
 */

#ifndef TIMETAG_COMMAND_H
#define TIMETAG_COMMAND_H

#include <string>
#include "icommand.h"
#include "infobytes.h"
#include <cstdlib>

#define CMD_BUFFER_LEN          CS1_MAX_FRAME_SIZE // TODOtwhat is this?
#define TIMETAG_CMD_SIZE        CS1_MAX_FRAME_SIZE-1
#define TIMETAG_CMD_JOB_ID_SIZE 2
#define TIMETAG_MAX_JOB_COMMAND ( CS1_MAX_FRAME_SIZE - sizeof(time_t) - TIMETAG_CMD_JOB_ID_SIZE )

using namespace std;

class InfoBytesTimetag : public InfoBytes {
    public:
        int         job_id;
        time_t      job_timestamp;
        char *      job_command;

        string * ToString() {
            string* infoStatus = new string (1, *job_command);
            return infoStatus;
        }
};

class TimetagCommand : public ICommand 
{
  private :
      char * command;
      time_t timestamp;
  public: 
      TimetagCommand();
      TimetagCommand(char * command, time_t timestamp);
      ~TimetagCommand();
      void* Execute(size_t *pSize);
      int GetCustomTime(std::string format, char * output_date, int output_length, time_t);
      int AddJob(time_t timestamp, char * executable);
      int CancelJob(const int job_id);
      InfoBytes* ParseResult(const unsigned char * timetag_result_bytes);
      char* GetCommand();
      char* GetDateTime();
};

#endif 
