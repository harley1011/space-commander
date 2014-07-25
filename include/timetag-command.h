/*
 * schedule-command.h
 * Copyright (C) 2014 ngc224 <ngc224@Andromeda>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef SCHEDULE_COMMAND_H
#define SCHEDULE_COMMAND_H

#include <string>
#include "icommand.h"
#include <cstdlib>

#define TIMETAG_CMD_SIZE 255

using namespace std;

class TimetagCommand : public ICommand {
public: 
    TimetagCommand(char * command, char * date_time) {
        this->command = command;
        this->date_time = date_time;
    }
    
    ~TimetagCommand() {
        if (command != NULL) {
            delete command;
            command = NULL;
        }
    }

    void * Execute();
    std::string SysExec(char* orig_cmd); 
    char * GetCustomTime(std::string format, int moreminutes);
    int AddJob(char * date_time, char * executable);
    int CancelJob(const int job_id);
    void * ParseResult(const char * result);

    char * GetCommand() { return command; }
    char * GetDateTime() { return date_time; }

private:
    char * command;
    char * date_time;
};

#endif 
