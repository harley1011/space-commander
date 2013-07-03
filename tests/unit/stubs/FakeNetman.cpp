#include <Net2Com.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

void Write(Net2Com* channel, unsigned char byte){
    channel->WriteToInfoPipe(sizeof(unsigned char));
    channel->WriteToDataPipe(byte);
}
void Write(Net2Com* channel, const char* data){
    channel->WriteToInfoPipe(strlen(data));
    channel->WriteToDataPipe(data);
}
void Write(Net2Com* channel, const void* data, int size){
    channel->WriteToInfoPipe(size);
    channel->WriteToDataPipe(data, size);
}

int main(){
    
    struct settime {
        unsigned char type;
        int year;
        unsigned char month;
        unsigned char day;
        unsigned char hour;
        unsigned char min;
        unsigned char sec;
    };
    typedef struct settime settime;
    settime settime_command;

    settime_command.type = 0;
    settime_command.year = 2015;
    settime_command.month = 12;
    settime_command.day = 1;
    settime_command.hour = 12;
    settime_command.min = 59;
    settime_command.sec = 59;
    
    Net2Com* channel =  new Net2Com(PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR);

    #ifdef LONELY   //***********************************************************//
    pid_t pid;
    pid = fork();

    if (pid == 0){
        Net2Com* commander = new Net2Com(PIPE_TWO, PIPE_ONE, PIPE_FOUR, PIPE_THREE);
        char info[2] = {};
        char buffer[100];

        commander->ReadFromInfoPipe(info);
        printf("Info : %d\t", (unsigned char)(*info) );

        commander->ReadFromDataPipe(buffer); 
        printf("Data : %d", (unsigned char)buffer[0] );
    
        commander->ReadFromInfoPipe(info);
        printf("Info : %d\t", (unsigned char)(*info) );
        commander->ReadFromDataPipe(buffer);
        settime *com = (settime*)buffer;
        printf("date : %d/%d/%d\n", com->year, (unsigned char)com->month, (unsigned char)com->day);
        delete commander;
        return 0;
    }
    #endif          //***********************************************************//

    Write(channel, 252);    // Start session signal.
    Write(channel, &settime_command, sizeof(settime));


    delete channel;

    return 0;
}
