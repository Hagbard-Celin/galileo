#ifndef _GALILEOFM_DOS
#define _GALILEOFM_DOS

#include <galileofm/64bit.h>

/*****************************************************************************

 DOS functions

 *****************************************************************************/

// Launch programs
BOOL LaunchWB(char *,struct Screen *,short);
BOOL LaunchWBNew(char *,struct Screen *,short,long,char *);
BOOL LaunchWBNotify(char *,struct Screen *,short,long,char *,struct Process **,IPCData *,ULONG);
BOOL LaunchCLI(char *,struct Screen *,BPTR,BPTR,BPTR,short,long);


#define LAUNCHF_WAIT		(1<<0)		// Wait for return
#define LAUNCHF_USE_STACK	(1<<14)		// Stack is specified


// Defines for the 'wait' parameter for LaunchWB() and LaunchWBNew()

#define LAUNCH_REPLY		-1	// Return as soon as the process has started
#define LAUNCH_NOWAIT		0	// Return immediately
#define LAUNCH_WAIT		1	// Wait for process to exit
#define LAUNCH_WAIT_TIMEOUT	2	// Wait for 5 seconds then display a requester


// Device name functions
struct DosList *DeviceFromLock(BPTR,char *);
struct DosList *DeviceFromHandler(struct MsgPort *,char *);
BOOL DevNameFromLock(BPTR,char *,long);
BOOL IsDiskDevice(struct MsgPort *);


// Set environment variable
void SetEnv(char *,char *,BOOL);


// Date routines
char *ParseDateStrings(char *,char *,char *,long *);
BOOL DateFromStrings(char *,char *,struct DateStamp *,long);
void DateToStrings(struct DateStamp *date,char *date_buf,char *time_buf,int flags);

#define RANGE_BETWEEN		1
#define RANGE_AFTER		2
#define RANGE_WEIRD		-1


// Version
BOOL GetFileVersion(char *,short *,short *,struct DateStamp *,APTR);


// Search for text
long SearchFile(APTR,UBYTE *,ULONG,UBYTE *,ULONG);

#define SEARCH_NOCASE		(1<<0)	// Not case sensitive
#define SEARCH_WILDCARD		(1<<1)	// Wildcards
#define SEARCH_ONLYWORDS	(1<<2)	// Only match whole words


// Copy and free DOS path list
BPTR GetDosPathList(BPTR);
void FreeDosPathList(BPTR);

// 64bit dospacket
struct DosPacket64
{
    struct Message  *dp_Link;    /* EXEC message                    */
    struct MsgPort  *dp_Port;    /* Reply port for the packet,      */
							    /* must be filled in on each send. */
    LONG	    dp_Type;    /* See ACTION_... below            */
    LONG	    dp_Res0;    /* Special compatibility field. [See below] */

    LONG	    dp_Res2;    /* This is returned for IoErr()    */
    ULONG	    Pad1;
    UQUAD	    dp_Res1;    /* This is the 64 bit primary result */
    LONG	    dp_Arg1;    /* 32 bit argument */
    ULONG	    Pad2;
    UQUAD	    dp_Arg2;    /* 64 bit argument */
    LONG	    dp_Arg3;    /* 32 bit argument */
    LONG	    dp_Arg4;    /* 32 bit argument */
    UQUAD	    dp_Arg5;    /* 64 bit argument */
};

#define ACTION_GET_FILE_SIZE64	8004

// The DosPacket64 dp_Res0 member initialisation value.
#define DP64_INIT	-3L

struct StandardPacket64
{
    struct Message      sp_Msg;
    struct DosPacket64  sp_Pkt;
};

// Get 64bit filesize
ULONG GetFileSize64(BPTR,UQUAD *);

// Paths and Volumes
UBYTE VolIdFromLock(BPTR lock, struct DateStamp *date, char *name);
BPTR LockFromVolIdPath(STRPTR volumename, CONST_STRPTR path, struct DateStamp *volumedate, UBYTE namelen);
BPTR LockFromPath(CONST_STRPTR path, ULONG length, ULONG flags);
STRPTR PathFromLock(APTR memory, BPTR lock, ULONG flags, CONST_STRPTR append);


#define LFPF_TRY_ICON		(1<<0)
#define LFPF_TRY_ICON_FIRST	(1<<1)


#define PFLF_USE_DEVICENAME	(1<<0)
#define	PFLF_END_SLASH		(1<<1)
#define	PFLF_SLASH_APPEND	(1<<2)

#endif
