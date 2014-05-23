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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : NamedPipe 
* 
* PURPOSE : Constructors
*
*-----------------------------------------------------------------------------*/
NamedPipe::NamedPipe(const char* fifo)
{
    strcpy(this->fifo_path, fifo);
    this->fifo = -1;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ~NamedPipe 
* 
* PURPOSE : Destructor
*
*-----------------------------------------------------------------------------*/
NamedPipe::~NamedPipe()
{
    closePipe();
    //unlink(fifo_path);    // Do not remove! maybe a process is still reading from this pipe.
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : CreatePipe
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
bool NamedPipe::CreatePipe()
{
    int retry = 0;
    bool result = false;
    while (result == false && retry < MAX_RETRY) {
        retry += 1;
        if (mkfifo(fifo_path, S_IRUSR | S_IWUSR) == 0) {
            result = true;
        } else {
            fprintf(stderr, "Can't create the pipe : %s\n", strerror(errno));
            switch(errno) {
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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Open
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
bool NamedPipe::Open(char mode)
{
    if (this->fifo != -1) {    // Pipe already open.
        return true;
    }

    if (mode == 'w') {
         fifo = open(fifo_path, O_NONBLOCK | O_WRONLY);
         if(fifo == -1) {
            fprintf(stderr, "Couldn't open(\"%s\", O_NONBLOCK | O_WRONLY) : %s\n",
                                                                fifo_path, strerror(errno));
            switch(errno) {
                case ENXIO : // If this happens : make sure both the Commander and the Netman has created there instance
                             // of Net2Com BEFORE attempting to write to the pipes.
                        fprintf(stderr, "[ERROR] The pipe is not open for reading, hence you cannot write to it!\n");
                    break;
                 default : ;         
            }

         }

    } else if (mode == 'r') {
         fifo = open(fifo_path, O_NONBLOCK | O_RDONLY);
         if(fifo == -1) {
            fprintf(stderr, "Couldn't open(\"%s\", O_NONBLOCK | O_RDONLY) : %s\n", 
                                                                fifo_path, strerror(errno));
         }

    }

    return fifo != -1;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : closePipe
* 
* PURPOSE : Close the named pipe.
*
*-----------------------------------------------------------------------------*/
void NamedPipe::closePipe()
{
    if (fifo != -1) {
        if(close(fifo) == -1) {
            fprintf(stderr, "Couldn't close(fifo) : %s\n", strerror(errno));
        } else {
            fifo = -1;
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ReadFromPipe 
* 
* PURPOSE : Read 'size' bytes from the pipe into 'buffer'.
*
* RETURN : Number of bytes read.
*
*-----------------------------------------------------------------------------*/
int NamedPipe::ReadFromPipe(char* buffer, int size) 
{
    int bytes_read = 0;

    if(!Open('r')) {
       return 0;
    }

    struct pollfd fds;
    fds.fd = fifo;
    fds.events = POLLIN;

    // poll 5 ms to see if there's new data to be read
    if(poll(&fds, 1, 5)) {
       bytes_read = read(fifo, buffer, size);

       if(bytes_read == -1) {
          if(errno != EAGAIN) {
             fprintf(stderr, "Couldn't read(fifo) : %s\n", strerror(errno));
          }

          return 0;
       }
    }

    return bytes_read;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : WriteToPipe
* 
* PURPOSE : Writes 'size' byte from 'buffer' to the pipe.
*
* RETURN : Number of bytes written.
*
*-----------------------------------------------------------------------------*/
int NamedPipe::WriteToPipe(const void* buffer, int size)
{
    int bytes_written;

    if(!Open('w')) {
       return 0;
    }

    bytes_written = write(fifo, buffer, size);

    if (bytes_written == -1) {
        if(errno != EAGAIN) {
           fprintf(stderr, "Couldn't write(fifo) : %s\n", strerror(errno));
        }

        return 0;
    }

    return bytes_written;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Exist
* 
* PURPOSE : Checks if the named pipe exists.
*
*-----------------------------------------------------------------------------*/
bool NamedPipe::Exist()
{
    if (access(fifo_path, F_OK) == 0) {
        return true;
    } else {
        return false;
    }
}
