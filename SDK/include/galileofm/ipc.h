#ifndef _GALILEOFM_IPC
#define _GALILEOFM_IPC

/*****************************************************************************

 Inter-process communication

 *****************************************************************************/

// IPC message
typedef struct {
	struct Message	msg;			// Exec message
	ULONG		command;		// Message command
	ULONG		flags;			// Message flags
	APTR		data;			// Message data
	APTR		data_free;		// Data to be FreeVec()ed automatically
	struct _IPC	*sender;		// Sender IPC
} IPCMessage;

#define REPLY_NO_PORT		(struct MsgPort *)-1	// Sync msg, no port supplied
#define REPLY_NO_PORT_IPC	(struct MsgPort *)-2	// Sync msg from a non-IPC process

// IPC process
typedef struct _IPC {
	struct MinNode		node;
	struct Process		*proc;		// Process pointer
	IPCMessage		startup_msg;	// Startup message
	struct MsgPort		*command_port;	// Port to send commands to
	struct ListLock		*list;		// List we're a member of
	APTR			userdata;
	APTR			memory;		// Memory
	struct MsgPort		*reply_port;	// Port for replies
	ULONG			flags;		// Flags
} IPCData;

#define IPCDATA(ipc)		((APTR)ipc->userdata)
#define SET_IPCDATA(ipc,data)	ipc->userdata=(APTR)data

// Used in the stack paramter for IPC_Launch
#define IPCF_GETPATH		(1<<31)		// Want copy of path list
#define IPCM_STACK(s)		(s&0xffffff)	// Mask out stack value

// Pre-defined commands
enum {
	IPC_COMMAND_BASE=0x8000000,
	IPC_STARTUP,				// Startup command
	IPC_ABORT,				// Abort!
	IPC_QUIT,				// Quit process
	IPC_ACTIVATE,				// Activate process
	IPC_HELLO,				// Something saying hello
	IPC_GOODBYE,				// Something saying goodbye
	IPC_HIDE,				// Process, hide thyself
	IPC_SHOW,				// Tell process to reveal itself
	IPC_RESET,				// Process should reset
	IPC_HELP,				// Help!
	IPC_NEW,				// Create something new
	IPC_GOT_GOODBYE,			// Got goodbye from something
	IPC_IDENTIFY,				// Identify yourself
	IPC_PRIORITY,				// Change your priority to this
	IPC_REMOVE,				// Remove yourself
};

// Pre-defined signals
#define IPCSIG_HIDE		SIGBREAKF_CTRL_D
#define IPCSIG_SHOW		SIGBREAKF_CTRL_E
#define IPCSIG_QUIT		SIGBREAKF_CTRL_F


ULONG IPC_Command(IPCData *,ULONG,ULONG,APTR,APTR,struct MsgPort *);
IPCData *IPC_FindProc(struct ListLock *,char *,BOOL,ULONG);
void IPC_Flush(IPCData *);
void IPC_Free(IPCData *);
long IPC_Launch(struct ListLock *,IPCData **,char *,ULONG,ULONG,ULONG,struct Library *);
void IPC_ListCommand(struct ListLock *,ULONG,ULONG,ULONG,BOOL);
IPCData *IPC_ProcStartup(ULONG *,ULONG (*__asm)(register __a0 IPCData *,register __a1 APTR));
void IPC_Reply(IPCMessage *);

#endif
