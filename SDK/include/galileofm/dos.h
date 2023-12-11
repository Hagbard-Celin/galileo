#ifndef _GALILEOFM_DOS
#define _GALILEOFM_DOS

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
BOOL DateFromStrings(char *,char *,struct DateStamp *);
BOOL DateFromStringsNew(char *,char *,struct DateStamp *,long);

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


#endif
