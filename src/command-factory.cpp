#include "command-factory.h"
#include <cstddef>
#include <stdlib.h>
#include <cstring>

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
        case '6': {
            return CommandFactory::CreateDecode(data);
        }
    }

    return NULL;
}

ICommand* CommandFactory::CreateGetLog(char* data) {
    GetLogCommand* result = new GetLogCommand(data[1], (size_t)data[2]);
    return result;
}

ICommand* CommandFactory::CreateUpdate(char* data) {
    char path_length[4] = { data[1] , data[2] , data[3] , '\0' };
    int path_length_itoa = atoi(path_length);
    char* path = (char* )malloc(sizeof(char) * path_length_itoa + 1);
    memset(path, '\0', path_length_itoa + 1); 
    strncpy(path, data + 4, path_length_itoa);

    char file_data_length[4] = { data[4 + path_length_itoa], data[5 + path_length_itoa], data[6 + path_length_itoa], '\0' };
    int file_data_length_itoa = atoi(file_data_length);
    char* file_data  = (char* )malloc(sizeof(char) * file_data_length_itoa + 1);
    memset(file_data, '\0', file_data_length_itoa + 1);
    strncpy(file_data, data + (4 + path_length_itoa + 3), file_data_length_itoa);
    UpdateCommand* result = new UpdateCommand(path, file_data_length_itoa, file_data);
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

ICommand* CommandFactory::CreateDecode(char* data) {
    char path_length[4] = { data[2] , data[3] , data[4] , '\0' };
    int path_length_itoa = atoi(path_length);
    char* path = (char* )malloc(sizeof(char) * path_length_itoa + 1);
    memset(path, '\0', path_length_itoa + 1); 
    strncpy(path, data + 5, path_length_itoa);

    char file_data_length[4] = { data[5 + path_length_itoa], data[6 + path_length_itoa], data[7 + path_length_itoa], '\0' };
    int file_data_length_itoa = atoi(file_data_length);
    char* file_data  = (char* )malloc(sizeof(char) * file_data_length_itoa + 1);
    memset(file_data, '\0', file_data_length_itoa + 1);
    strncpy(file_data, data + (5 + path_length_itoa + 3), file_data_length_itoa);
    DecodeCommand* result = new DecodeCommand( file_data, path, 0);
    return result;
}
