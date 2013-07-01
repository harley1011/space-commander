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

int main(){
    Net2Com* channel =  new Net2Com(PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR);

    #ifdef LONELY   //***********************************************************//
    pid_t pid;
    pid = fork();

    if (pid == 0){
        Net2Com* commander = new Net2Com(PIPE_TWO, PIPE_ONE, PIPE_FOUR, PIPE_THREE);
        char info[1] = {};
        char buffer[10];

        commander->ReadFromInfoPipe(info);
        printf("Info : %d\t", (unsigned char)(*info) );

        commander->ReadFromDataPipe(buffer); 
        printf("Data : %d", (unsigned char)buffer[0] );
    
        delete commander;
        return 0;
    }
    #endif          //***********************************************************//

    Write(channel, 252);    // Start session signal.

    // TODO Send command

    delete channel;

    return 0;
}
