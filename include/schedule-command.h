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

using namespace std;

class ScheduleCommand : public ICommand {
public: 
    ScheduleCommand(char * command, char * date_time) {
        this->command = command;
        this->date_time = date_time;
    }
    
    ~ScheduleCommand() {
        if (command != NULL) {
            delete command;
            command = NULL;
        }
    }

    void * Execute();
    char * GetCommand() { return command; }
    char * GetDateTime() { return date_time; }
private:
    char * command;
    char * date_time;
};

#endif 
