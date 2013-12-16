#include "update-command.h"
#include <stdio.h>
#include "base64.h"
#include <cstring>

void* UpdateCommand::Execute() {
    FILE* fp_update_file = NULL;
    char* result = NULL;
    int retry = 10000;

    while(retry > 0 && fp_update_file == NULL){
        fp_update_file = fopen(this->GetPath(), "ab+");
        retry =- 1;
    }

    if(fp_update_file != NULL) {
        std::string file_data_sting = this->file_data;            
        string decoded_data = base64_decode(file_data_sting);

        retry = 10000;
        size_t bytes_left = 142; //this->data_length;
        size_t bytes_written = 0;
        while(retry > 0 && bytes_left > 0){
            bytes_left = 142 - bytes_written;
            bytes_written += fwrite(decoded_data.c_str() + bytes_written, sizeof(char), bytes_left, fp_update_file);
            retry =- 1;
        }

        fclose(fp_update_file); 

        result = (char* )malloc(sizeof(char) * 50);
        memset(result, '\0', sizeof(char) * 50);
        sprintf(result, "%lld", (long long)bytes_written);
    }

    return result;         
}

