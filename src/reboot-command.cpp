#include "reboot-command.h"
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>

void* RebootCommand::Execute(){

    reboot(LINUX_REBOOT_CMD_RESTART);

    return 0;
}
