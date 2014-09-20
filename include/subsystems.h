/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
*
*
*
*-----------------------------------------------------------------------------*/
#ifndef _SUBSYSTEM_H_
#define _SUBSYSTEM_H_

extern const char* s_cs1_subsystems[];

#define ACS          0x00
#define COMMS        0x01
#define HARDWARE     0x02
#define MECH         0x03
#define PAYLOAD      0x04
#define POWER        0x05
#define SOFTWARE     0x06
#define UPDATER      0x07
#define WATCH_PUPPY  0x08
#define COMMANDER    0x09 

#define UNDEF_SUB    0xFF

#endif
