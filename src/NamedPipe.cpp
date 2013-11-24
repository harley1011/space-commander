#include <NamedPipe.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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
         fprintf(stderr, "Initializing %s\n", __FILE__);

         int fifo_fd = open(fifo_path, O_NONBLOCK | O_WRONLY);
         if(fifo_fd == -1){
            fprintf(stderr, "Couldn't open fd for %s\n", fifo_path);
            return false;
         }

         this->fifo = fdopen(fifo_fd, "wb");

         if(!this->fifo){
            fprintf(stderr, "Failed to open he100 pipe");
            return false;
         }


         fprintf(stderr, "Initialization successful!\n");

         return true;
    }else if (mode == 'r'){

         fprintf(stderr, "Initializing %s\n", __FILE__);

         int fifo_fd = open(fifo_path, O_NONBLOCK | O_RDONLY);
         if(fifo_fd == -1){
            fprintf(stderr, "Couldn't open fd for %s\n", fifo_path);
            return false;
         }

         this->fifo = fdopen(fifo_fd, "rb");

         if(!this->fifo){
            fprintf(stderr, "Failed to open he100 pipe");
            return false;
         }


         fprintf(stderr, "Initialization successful!\n");

         return true;
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
        this->fifo = NULL;
    }
}

//----------------------------------------------
// ReadFromPipe
//----------------------------------------------
int NamedPipe::ReadFromPipe(char* buffer, int buf_size){
    int bytes_read;
    bool need_to_close_pipe;

    if (fifo == NULL){
        fifo = fopen(fifo_path, "rb");
        if (fifo == NULL){
            fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
            return 0;
        }
        need_to_close_pipe = true;
    }else{
        need_to_close_pipe = false;
    }

    bytes_read = fread(buffer, 1, buf_size, fifo);

    if(bytes_read == 0){
      if(feof(fifo)){
         printf("feof(fifo)!!\n");
      }
    }

    if(need_to_close_pipe){
       fclose(fifo);
    }

    return bytes_read;
}
//----------------------------------------------
//  WriteToPipe
//----------------------------------------------
int NamedPipe::WriteToPipe(const void* data, int size){
    bool need_to_close_pipe;
    int result;

    if (fifo == NULL){
        fifo = fopen(fifo_path, "wb");
        if (fifo == NULL){
            fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
            return -1;
        }
        need_to_close_pipe = true;
    }else{
        need_to_close_pipe = false;
    }

    result = fwrite(data, 1, size, fifo);

    if (result < 0){
        fprintf(stderr, "Can't write to the pipe : %s\n", strerror(errno));
    }

    if(need_to_close_pipe){
       fclose(fifo);
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
