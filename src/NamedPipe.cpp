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
bool NamedPipe::ensure_open(char mode){
    if (this->fifo != NULL){                                                                // Pipe already open.
        return true;
    }

    if (mode == 'w'){
         fprintf(stderr, "Initializing %s\n", __FILE__);

         int fifo_fd = open(fifo_path, O_NONBLOCK | O_WRONLY);
         if(fifo_fd == -1){
            fprintf(stderr, "Couldn't open fd for %s\n", fifo_path);
            fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
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
            fprintf(stderr, "Can't open the pipe : %s\n", strerror(errno));
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
    int bytes_read = 0;

    this->ensure_open('r');

    struct pollfd fds;

    fds.fd     = fifo;
    fds.events = POLLIN;

    // poll 5 ms to see if it's ready
    if(poll(&fds, 1, 5)){
       bytes_read = fread(buffer, 1, buf_size, fifo);
    }

    return bytes_read;
}
//----------------------------------------------
//  WriteToPipe
//----------------------------------------------
int NamedPipe::WriteToPipe(const void* data, int size){
    int result;

    this->ensure_open('w');

    printf("Writing %d bytes to %s... ", size, fifo_path);
    result = fwrite(data, 1, size, fifo);
    printf("done, wrote %d bytes\n", result);

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
