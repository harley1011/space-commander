#include "command-factory.h"
#include <cstddef>

ICommand* CommandFactory::CreateCommand(char * data) {
    if (data == NULL) { return NULL; }

    switch(data[0]) {
            case 0: {
                return CommandFactory::CreateSetTime(data);
            }
            case 1: {
                return CommandFactory::CreateUpdate(data);
        }
            case 2: {
            return CommandFactory::CreateGetLog(data);
        }
    }
}

ICommand* CommandFactory::CreateGetLog(char* data) {
    GetLogCommand* result = new GetLogCommand(data[1]);
    return result;
}

ICommand* CommandFactory::CreateUpdate(char* data) {
    char* start = data + 2;
    char length = data[1];

    string* s = new string(start, length);

    UpdateCommand* result = new UpdateCommand(s);
    return result;
}

ICommand* CommandFactory::CreateSetTime(char* data) {
    data += 1;
    unsigned int seconds = (data[0] << 24 & 0xFF000000) | (data[1] << 16 & 0x00FF0000) | (data[2] << 8 & 0x0000FF00) | (data[3] & 0x000000FF);
    SetTimeCommand* result = new SetTimeCommand(seconds);
    return result;
}
