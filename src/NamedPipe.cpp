#include <NamedPipe.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
//----------------------------------------------
//  Constructor
//----------------------------------------------
NamedPipe::NamedPipe(const char* fifo){
    strcpy(fifo_path, fifo);
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
NamedPipe::~NamedPipe(){
    unlink(fifo_path);
}
//----------------------------------------------
// CreatePipe 
//----------------------------------------------
bool NamedPipe::CreatePipe(){
    int retry = 0;
    bool result = false;
    while (result == false && retry < MAX_RETRY){
        retry += 1;
        if (mkfifo(fifo_path, S_IRUSR | S_IWUSR) == 0){
            result = true;
        }else{
            fprintf(stderr, "Can't create the pipe : %s\n", strerror(errno));
            switch(errno){
                case EEXIST : fprintf(stderr, "Continue...\n");                             // If pipe already exists, OK.
                              result = true;
                    break;

                 default : ;                                                                // result is false;
            }
        }

        usleep(100000);                                                                     // 100 ms
    }
    
    return result;
}
//----------------------------------------------
// ReadFromPipe
//----------------------------------------------
char* NamedPipe::ReadFromPipe(char* buffer){
    FILE* fifo = fopen(fifo_path, "r");
    if (fifo == NULL){
        return NULL;
    }

    fscanf(fifo, "%s", buffer);
    fclose(fifo);

    return buffer;
}
//----------------------------------------------
//  WriteToPipe
//----------------------------------------------
int NamedPipe::WriteToPipe(const char* str){
    int result;

    FILE* fifo = fopen(fifo_path, "w");
    if (fifo == NULL){
        fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
        return -1;
    }

    result = fprintf(fifo, "%s", str);

    if (result < 0){
        fprintf(stderr, "Can't write to the pipe : %s\n", strerror(errno));
    }

    fclose(fifo);

    return result;
}
//----------------------------------------------
//  Exist
//----------------------------------------------
bool NamedPipe::Exist(){
    if (access(fifo_path, F_OK) == 0){
        return true;
    }else{
        return false;
    }
}
