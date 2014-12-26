#include <stdio.h>
#include "base64.h"
#include <cstring>
#include <sys/stat.h>
#include "decode-command.h"
#include "shakespeare.h"
#include "SpaceDecl.h"
#include "SpaceString.h"
#include "commands.h"
#include "subsystems.h"


void* DecodeCommand::Execute() {
    FILE* fpDestFile = NULL;
    FILE* fpSrcFile = NULL;
    char* result = NULL;
    int retry    = 10000;

    while(retry > 0 && fpSrcFile == NULL){
        fpSrcFile = fopen(this->GetSrcPath(), "rb");
        retry =- 1;
    }

    printf("  GetSrcPath:   %s\n", this->GetSrcPath());
    printf("  GetDestPath:  %s\n", this->GetDestPath());
    printf("  IsExecutable: %d\n", this->IsExecutable());
    fflush(stdout);

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

        if (this->IsExecutable() && chmod(this->GetDestPath(), S_IRWXU) != 0){
            printf("  Error: Chmod Failed!:\n");
            fflush(stdout);
        } 
 
        if (remove(this->GetSrcPath()) != 0){
            printf("  Error: Remove Failed!:\n");
            fflush(stdout);
        }

        result = (char* )malloc(sizeof(char) * 50 + CMD_HEAD_SIZE);
        memset(result + CMD_HEAD_SIZE, '\0', sizeof(char) * 50);
        sprintf(result + CMD_HEAD_SIZE, "%lld", (long long)bytes_written);
        result[0] = DECODE_CMD;
        result[1] = CS1_SUCCESS;
    }

    return result;         
}
void* DecodeCommand::ParseResult(const char *result)
{
    if (!result || result[0] != DECODE_CMD){
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER],"Decode failure: Can't parse result");
        return (void*)0;
    }
    static struct InfoBytesDecode info_bytes = {0};
    info_bytes.decode_status = result[1];

    char buffer[100];
    if(info_bytes.decode_status == CS1_SUCCESS)
    {    
        snprintf(buffer,100,"Decode success: No informaation to report");
        Shakespeare::log(Shakespeare::NOTICE,cs1_systems[CS1_COMMANDER], buffer);
    }
    else
    {
        snprintf(buffer,100,"Decode failure: Unknown");
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER], buffer);
    }
    return (void*)&info_bytes;

}

