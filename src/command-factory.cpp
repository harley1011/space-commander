#include "command-factory.h"
#include <cstddef>

ICommand* CommandFactory::CreateCommand(char * data) {
    if (data == NULL) { return NULL; }

    switch(data[0]) {
        case '0': {
            return CommandFactory::CreateSetTime(data);
        }
        case '1': {
            return CommandFactory::CreateGetTime(data);
        }
        case '2': {
            return CommandFactory::CreateUpdate(data);
        }
        case '3': {
            return CommandFactory::CreateGetLog(data);
        }
    }

    return NULL;
}

ICommand* CommandFactory::CreateGetLog(char* data) {
    GetLogCommand* result = new GetLogCommand(data[1], (size_t)data[2]);
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
    SetTimeCommand* result = new SetTimeCommand(data[0], data[1], data[2], data[3], data[4], data[5]);
    return result;
}

ICommand* CommandFactory::CreateGetTime(char* data) {
    GetTimeCommand* result = new GetTimeCommand();
    return result;
}
