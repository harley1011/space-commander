#ifndef UPDATE_COMMAND_H
#define UPDATE_COMMAND_H

#include <string>
#include "icommand.h"
#include <cstdlib>

using namespace std;

class UpdateCommand : public ICommand {
public:
    UpdateCommand(char* path, int data_length, char* file_data) {
        this->path = path;
        this->data_length = data_length;
        this->file_data = file_data;
    }
    
    ~UpdateCommand(){ 
        //Does file_data need to be set to null as well? yes!
        if (file_data != NULL){
            delete file_data;
            file_data = NULL;
        }
        
        if (path != NULL) {
            delete path;
            path = NULL;
        }
    }

    void* Execute();
    void* ParseResult(const char* result);
    char* GetPath()       { return path; }
    int   GetDataLength() { return data_length; }
private:
    char* path;
    int data_length;
    char* file_data;
};
#endif
