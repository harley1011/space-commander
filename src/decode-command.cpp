#include "decode-command.h"
#include <stdio.h>
#include "base64.h"
#include <cstring>
#include <sys/stat.h>

void* DecodeCommand::Execute() {
    FILE* fpDestFile = NULL;
    FILE* fpSrcFile = NULL;
    char* result = NULL;
    int retry    = 10000;

    while(retry > 0 && fpSrcFile == NULL){
        fpSrcFile = fopen(this->GetSrcPath(), "rb");
        retry =- 1;
    }

    if(fpSrcFile != NULL) {
        fseek(fpSrcFile, 0L, SEEK_END);
        long s = ftell(fpSrcFile);
        rewind(fpSrcFile);

        char* buffer = (char* )malloc(sizeof(char) * s);
        if (buffer != NULL) {
            fread(buffer, s, 1, fpSrcFile);
            fclose(fpSrcFile);
            fpSrcFile = NULL;
        }

        retry = 10000;
        while(retry > 0 && fpDestFile == NULL){
            fpDestFile = fopen(this->GetDestPath(), "wb");
            retry =- 1;
        }


        std::string std_buffer = buffer;
        string decoded_data = base64_decode(std_buffer);

        retry = 10000;
        size_t bytes_left = this->GetTotalSize();
        size_t bytes_written = 0;
        while(retry > 0 && bytes_left > 0){
            bytes_left -= bytes_written;
            bytes_written += fwrite(decoded_data.c_str() + bytes_written, sizeof(char), bytes_left, fpDestFile);
            retry =- 1;
        }

        fclose(fpDestFile); 

        if (chmod(this->GetDestPath(), S_IRWXU) != 0){
            // TODO log it   
        } 
 
        if (remove(this->GetSrcPath()) != 0){
            // TODO log it
        }

        result = (char* )malloc(sizeof(char) * 50);
        memset(result, '\0', sizeof(char) * 50);
        sprintf(result, "%lld", (long long)bytes_written);
    }

    return result;         
}

