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
    this->fifo = NULL;
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
NamedPipe::~NamedPipe(){
    this->close();
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
//  persist_open          
//----------------------------------------------
bool NamedPipe::persist_open(char mode){
    if (this->fifo != NULL){                                                                // Pipe already open.
        return true;
    }

    if (mode == 'w'){
        this->fifo = fopen(fifo_path, "wb");
    }else if (mode == 'r'){
        this->fifo = fopen(fifo_path, "rb");
    }

    if (this->fifo == NULL){
        fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
        return false;
    }

    return true;
}

//----------------------------------------------
//  close
//----------------------------------------------
void NamedPipe::close(){
    if (this->fifo != NULL){
        fclose(this->fifo);
        this->fifo == NULL;
    }
}

//----------------------------------------------
// ReadFromPipe
//----------------------------------------------
int NamedPipe::ReadFromPipe(char* buffer, int buf_size){
    const int NUMBER_OF_BYTES_READ = 1;
    int total_bytes_read = 0;

    if (this->fifo == NULL){
        FILE* fifo = fopen(fifo_path, "rb");
        if (fifo == NULL){
            return NULL;
        }   

        int bytes_read = 0;

        while (total_bytes_read < buf_size && (bytes_read = fread(buffer, 1, NUMBER_OF_BYTES_READ, fifo)) > 0){
            total_bytes_read += bytes_read;
            buffer += NUMBER_OF_BYTES_READ;
        }
        
        fclose(fifo);
    }else{
        int bytes_read = 0;

        while (total_bytes_read < buf_size && (bytes_read = fread(buffer, 1, NUMBER_OF_BYTES_READ, fifo)) > 0){
            total_bytes_read += bytes_read;
            buffer += NUMBER_OF_BYTES_READ;
        }
    }

    return total_bytes_read;
}
//----------------------------------------------
//  WriteToPipe
//----------------------------------------------
int NamedPipe::WriteToPipe(const void* data, int size){
    int result;
    
    if (this->fifo == NULL){
        FILE* fifo = fopen(fifo_path, "wb");
        if (fifo == NULL){
            fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
            return -1;
        }

        result = fwrite(data, 1, size, fifo);

        fclose(fifo);
    }else{
        result = fwrite(data, 1, size, fifo);
    }

    if (result < 0){
        fprintf(stderr, "Can't write to the pipe : %s\n", strerror(errno));
    }

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
