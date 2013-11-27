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
#include <poll.h>
//----------------------------------------------
//  Constructor
//----------------------------------------------
NamedPipe::NamedPipe(const char* fifo){
    strcpy(fifo_path, fifo);
    this->fifo = -1;
}
//----------------------------------------------
//  Destructor
//----------------------------------------------
NamedPipe::~NamedPipe(){
    closePipe();
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
    if (fifo != -1){ // Pipe already open.
        return true;
    }

    if (mode == 'w'){
         fifo = open(fifo_path, O_NONBLOCK | O_WRONLY);
         if(fifo == -1){
            fprintf(stderr, "Couldn't open(\"%s\", O_NONBLOCK | O_WRONLY) : %s\n"
                          ,                   fifo_path,                     strerror(errno));
         }

    }else if (mode == 'r'){
         fifo = open(fifo_path, O_NONBLOCK | O_RDONLY);
         if(fifo == -1){
            fprintf(stderr, "Couldn't open(\"%s\", O_NONBLOCK | O_RDONLY) : %s\n"
                          ,                   fifo_path,                     strerror(errno));
         }

    }

    return fifo != -1;
}

//----------------------------------------------
//  close
//----------------------------------------------
void NamedPipe::closePipe(){
    if (fifo != -1){
        if(close(fifo) == -1){
            fprintf(stderr, "Couldn't close(fifo) : %s\n", strerror(errno));
        }else{
            fifo = -1;
        }
    }
}

//----------------------------------------------
// ReadFromPipe
//----------------------------------------------
int NamedPipe::ReadFromPipe(char* buffer, int size){
    int bytes_read = 0;

    if(!ensure_open('r')){
       return 0;
    }

    // TODO - make this a private member and initialize only once (?)
    struct pollfd fds;

    fds.fd     = fifo;
    fds.events = POLLIN;

    // poll 5 ms to see if there's new data to be read
    if(poll(&fds, 1, 5)){
       bytes_read = read(fifo, buffer, size);

       if(bytes_read == -1){
          if(errno != EAGAIN){
             fprintf(stderr, "Couldn't read(fifo) : %s\n", strerror(errno));
          }
          return 0;
       }
    }

    return bytes_read;
}
//----------------------------------------------
//  WriteToPipe
//----------------------------------------------
int NamedPipe::WriteToPipe(const void* buffer, int size){
    int bytes_written;

    if(!ensure_open('w')){
       return 0;
    }

    bytes_written = write(fifo, buffer, size);

    if (bytes_written == -1){
        if(errno != EAGAIN){
           fprintf(stderr, "Couldn't write(fifo) : %s\n", strerror(errno));
        }
        return 0;
    }

    return bytes_written;
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
