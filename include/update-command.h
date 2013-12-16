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
//Does file_data need to be set to null as well?
        if (path != NULL) {
            delete path;
            path = NULL;
        }
        
    }

    void* Execute();
    char* GetPath() { return path; }
private:
    string Base64ToAscii(string base64_data);

    char* path;
    int data_length;
    char* file_data;
};
#endif
