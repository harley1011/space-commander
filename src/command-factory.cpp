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
        case '4': {
            return CommandFactory::CreateReboot(data);
        }
        case '6': {
            return CommandFactory::CreateDecode(data);
        } 
    }

    return NULL;
}

ICommand* CommandFactory::CreateGetLog(char* data) {
    int bytes = GetLength3(data, 2);

    GetLogCommand* result = new GetLogCommand(data[1], (size_t)bytes);
    return result;
}

ICommand* CommandFactory::CreateUpdate(char* data) {
    const int PATH_LENGTH = 3;
    int offset = 1;

    int pathLength = GetLength3(data, offset);

    offset += PATH_LENGTH;
    char* path = GetPath(data, pathLength, offset);

    offset += pathLength;
    int fileDataLength = GetLength3(data, offset);

    offset += PATH_LENGTH;
    char* fileData = GetPath(data, fileDataLength, offset);
    
    UpdateCommand* result = new UpdateCommand(path, fileDataLength, fileData);
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


ICommand* CommandFactory::CreateReboot(char* data){
    RebootCommand* result = new RebootCommand();
    return result;
}

ICommand* CommandFactory::CreateDecode(char* data) {
    const int PATH_LENGTH = 3;
    int offset = 2;
    
    int srcLength = GetLength3(data, offset);

    offset += PATH_LENGTH;
    char* src = GetPath(data, srcLength, offset);

    offset += srcLength;
    int destLength = GetLength3(data, offset);

    offset += PATH_LENGTH;
    char* dest = GetPath(data, destLength, offset);
    
    offset += destLength;
    int decodedSize = GetLength10(data, offset);

    DecodeCommand* result = new DecodeCommand(dest, src, 0, decodedSize);
    return result;
}

int CommandFactory::GetLength3(char* data, int offset) {
    char length[4] = { data[offset] , data[offset + 1] , data[offset + 2] , '\0' };
    int result = atoi(length);
    return result;
}

int CommandFactory::GetLength10(char* data, int offset) {
    char length[11] = { data[offset]     , data[offset + 1] , data[offset + 2],  
                        data[offset + 3] , data[offset + 4] , data[offset + 5], 
                        data[offset + 6] , data[offset + 7] , data[offset + 8], 
                        data[offset + 9] , '\0' };
    int result = atoi(length);
    return result;
}

char* CommandFactory::GetPath(char* data, size_t length, int offset) {
    char* result = (char* )malloc(sizeof(char) * length + 1);
    memset(result, '\0', length + 1); 
    strncpy(result, data + offset, length);
}
