#include "getlog-command.h"
#include <cerrno>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char ACS          = 0;
const char COMMS        = 1;
const char HARDWARE     = 2;
const char MECH         = 3;
const char PAYLOAD      = 4;
const char POWER        = 5;
const char SOFTWARE     = 6;
const char WATCH_PUPPY  = 7;

const size_t GetLogCommand::MAX_LENGTH = 140;

char* GetLogCommand::GetLogFilename(char subsystem) {
    char* result = (char* )malloc(sizeof(char) * 255);
    //TODO: Replace by Alex's function
    strcpy(result, "/Users/Robert/watchpuppy.log");
    return result;
}

char* GetLogCommand::ReadLogFile(char* filename, size_t length) {
    if (length > GetLogCommand::MAX_LENGTH) { length = GetLogCommand::MAX_LENGTH; }

    // Find file size
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    unsigned long size = ftell(fp);
    rewind(fp);

    if (length > size) { length = size; }

    char* bufferRead  = (char* )malloc(sizeof(char) * length);
    char* bufferWrite = (char* )malloc(sizeof(char) * size - length);

    size_t bytesRead = 0;
    int retries      = 10;

    // Read requested length text
    while (bytesRead == 0 && retries > 0) {
        bytesRead = fread(bufferRead, 1, length, fp);
        retries -= 1;
    }

    // Read remaining text
    if (bytesRead > 0) {
        bytesRead = fread(bufferWrite, 1, size - length, fp);
    }

    fclose(fp);

    // Write remaining text
    if (bytesRead > 0) {
        fp = fopen(filename, "w");
        fwrite(bufferWrite, sizeof(char), size - length, fp);
        fclose(fp);
    }

    free(bufferWrite);

    return bufferRead;
}

void* GetLogCommand::Execute(){
    char* filename = GetLogFilename(this->GetSubSystem());
    char* data     = ReadLogFile(filename, this->GetLength());

    free(filename);

    return (void*)data;
}
