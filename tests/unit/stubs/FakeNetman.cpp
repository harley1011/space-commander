#include <Net2Com.h>

int main(){
    Net2Com* channel =  new Net2Com(PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR);
    char ONE = 1;
    char TWO_FIFTY_TWO = 252;
    channel->WriteToInfoPipe(one);
    channel->WriteToDataPipe(TWO_FIFTY_TWO);


    delete channel;

    return 0;
}
