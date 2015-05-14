#include "common/update-command.h"
#include <stdio.h>
#include "space-commander/base64.h"
#include <cstring>
#include "common/commands.h"
#include "common/subsystems.h"
#include "SpaceString.h"
#include "SpaceDecl.h"
#include "shakespeare.h"

void* UpdateCommand::Execute(size_t* pSize) {
    FILE* fp_update_file = NULL;
    char* result = NULL;
    int retry = 10000;

    while(retry > 0 && fp_update_file == NULL){
        fp_update_file = fopen(this->GetPath(), "ab+");
        retry =- 1;
    }

    printf("Uploading file %s\n", this->GetPath());

    if(fp_update_file != NULL) {
        retry = 10000;
        size_t bytes_left = this->GetDataLength();
        size_t bytes_written = 0;
        while(retry > 0 && bytes_left > 0){
            bytes_left -= bytes_written;
            bytes_written += fwrite(this->file_data + bytes_written, sizeof(char), bytes_left, fp_update_file);
            retry =- 1;
        }

        fclose(fp_update_file); 

        result = (char* )malloc(sizeof(char) * (50 + CMD_RES_HEAD_SIZE) );
        *pSize = 50 + CMD_RES_HEAD_SIZE;
        memset(result + CMD_RES_HEAD_SIZE, '\0', sizeof(char) * 50);
        sprintf(result, "%lld", (long long)bytes_written);
        result[0] = UPDATE_CMD;
        result[1] = CS1_SUCCESS;
    }

    return result;         
}
InfoBytes* UpdateCommand::ParseResult(char *result)
{ 
    static struct InfoBytesUpdate info_bytes;
    if(!result || result[0] != SETTIME_CMD) {
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER],"Possible update failure: Can't parse result");
        info_bytes.update_status = CS1_FAILURE;
        return &info_bytes;
    }

    info_bytes.update_status = result[1];
    info_bytes.bytes_written = result + CMD_RES_HEAD_SIZE; 


    char buffer[100];
    if(info_bytes.update_status == CS1_SUCCESS)
    {
        snprintf(buffer,100,"Update success: Bytes written %s ",info_bytes.bytes_written);
        Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER], buffer);
    }
    else
    {
        snprintf(buffer,100,"Update failure: Unknown"); 
        Shakespeare::log(Shakespeare::ERROR,cs1_systems[CS1_COMMANDER], buffer);
    }
    return &info_bytes;


}
