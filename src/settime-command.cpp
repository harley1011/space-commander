#include "settime-command.h"

void* SetTimeCommand::Execute(){
    struct timeval tv;
    
    if(gettimeofday(&tv, 0) == -1){
        printf ("Error! gettimeofday()\n");
        exit(1);
    }

    tv.tv_sec = GetSeconds();   

    if (settimeofday(&tv, 0) != 0){
        perror ("Error! settimeofday()\n");
        exit(1);
    }

    return;
}
