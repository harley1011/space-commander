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
private :
    char * command;
    time_t timestamp;

public: 
    TimetagCommand(char * command, time_t timestamp);
    ~TimetagCommand();

    void * Execute();
    std::string SysExec(char* orig_cmd); 
    char * GetCustomTime(std::string format, int moreminutes);
    int AddJob(time_t timestamp, char * executable);
    int CancelJob(const int job_id);

    char * GetCommand();
    char * GetDateTime();
#endif 
