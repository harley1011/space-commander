#include "reboot-command.h"
#include <sys/reboot.h>

void* RebootCommand::Execute(){
    reboot(RB_AUTOBOOT);

    return 0;
}
