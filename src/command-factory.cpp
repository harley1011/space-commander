#include <cstddef>
#include <stdlib.h>
#include <cstring>
#include <time.h>

#include "SpaceDecl.h"
#include "SpaceString.h"
#include "command-factory.h"

ICommand* CommandFactory::CreateCommand(char * data) {
    if (data == NULL) { 
        fprintf(stderr, "NULL argument passed to CreateCommand() in %s\n", __FILE__);
        return NULL; 
    }

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
            /*
            * data[0]   :   Command number
            * data[1]   :   Option      - specifies if options are present or not
            * ... [2]   :   Subsystem   - see subsystems.h 
            *   [3-6]   :   Size        - 
            *   [7-10]  :   Date        - time_t
            */
            return CommandFactory::CreateGetLog(data);
        }
        case '4': {
            return CommandFactory::CreateReboot(data);
        }
        case '6': {
            return CommandFactory::CreateDecode(data);
        }
        case '7': {
            return CommandFactory::CreateDeleteLog(data);
        }
    }

    return NULL;
}

ICommand* CommandFactory::CreateDeleteLog(char* data) 
{
    DeleteLogCommand* result = 0;
    char opt_byte = data[1];

    if (opt_byte == 'I') { 
        // 'I' means that we exepect 4 bytes representing an ino_t (unsigned long)
        unsigned int inode = SpaceString::getUInt(data + 2);
        result = new DeleteLogCommand(inode); 
    } else {        
        // we expect a null terminated string (filename)
        result = new DeleteLogCommand(&data[2]); 
    }

    return result;
}

ICommand* CommandFactory::CreateGetLog(char* data) {    // 0x33 or '3'
    char opt_byte = data[1];
    char subsystem = data[2];
    size_t size = SpaceString::getUInt(data + 3);
    time_t raw_time = SpaceString::getUInt(data + 7);

    GetLogCommand* result = new GetLogCommand(opt_byte, subsystem, size, raw_time);

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
    time_t timeRecieved;
    memcpy(&timeRecieved, data + 1, 8);

    SetTimeCommand* result = new SetTimeCommand(timeRecieved);
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

    int executable = data[1] - '0';
    DecodeCommand* result = new DecodeCommand(dest, src, executable, decodedSize);
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
    return result;
}
