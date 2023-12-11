#ifndef _GALILEOFM_NOTIFY
#define _GALILEOFM_NOTIFY

/*****************************************************************************

 Notification

 *****************************************************************************/

// Notification message
typedef struct
{
	struct Message		gn_Msg;		// Message header
	ULONG			gn_Type;	// Type of message
	ULONG			gn_UserData;	// User-supplied data
	ULONG			gn_Data;	// Message-specific data
	ULONG			gn_Flags;	// Flags
	struct FileInfoBlock	*gn_Fib;	// FIB for some messages
	char			gn_Name[1];	// Name for some messages
} GalileoNotify;

// gn_Msg.mn_Node.ln_Type
#define NT_GALILEOFM_NOTIFY		199

// gn_Type
#define GN_WRITE_ICON		(1<<0)		// Icon written
#define GN_APP_ICON_LIST	(1<<1)		// AppIcon added/removed
#define GN_APP_MENU_LIST	(1<<2)		// AppMenu added/removed
#define GN_CLOSE_WORKBENCH	(1<<3)		// Workbench closed
#define GN_OPEN_WORKBENCH	(1<<4)		// Workbench opened
#define GN_RESET_WORKBENCH	(1<<5)		// Workbench reset
#define GN_DISKCHANGE		(1<<6)		// Disk inserted/removed
#define GN_GALILEOFM_QUIT		(1<<7)		// Main program quit
#define GN_GALILEOFM_HIDE		(1<<8)		// Main program hide
#define GN_GALILEOFM_SHOW		(1<<9)		// Main program show
#define GN_GALILEOFM_START		(1<<10)		// Main program start
#define GN_DOS_ACTION		(1<<11)		// DOS action
#define GN_REXX_UP		(1<<12)		// REXX started
#define GN_FLUSH_MEM		(1<<13)		// Flush memory
#define GN_APP_WINDOW_LIST	(1<<14)		// AppWindow added/removed

// Flags with GN_WRITE_ICON
#define GNF_ICON_REMOVED	(1<<0)		// Icon removed
#define GNF_WINDOW_REMOVED	(1<<0)		// AppWindow removed
#define GNF_ICON_CHANGED	(1<<1)		// Image changed

// Flags with GN_DOS_ACTION
#define GNF_DOS_CREATEDIR	(1<<0)		// CreateDir
#define GNF_DOS_DELETEFILE	(1<<1)		// DeleteFile
#define GNF_DOS_SETFILEDATE	(1<<2)		// SetFileDate
#define GNF_DOS_SETCOMMENT	(1<<3)		// SetComment
#define GNF_DOS_SETPROTECTION	(1<<4)		// SetProtection
#define GNF_DOS_RENAME		(1<<5)		// Rename
#define GNF_DOS_CREATE		(1<<6)		// Open file (create)
#define GNF_DOS_CLOSE		(1<<7)		// Close file
#define GNF_DOS_RELABEL		(1<<8)		// Relabel disk

APTR AddNotifyRequest(ULONG,ULONG,struct MsgPort *);
void RemoveNotifyRequest(APTR);
void ReplyFreeMsg(struct Message *);
void SetNotifyRequest(APTR,ULONG,ULONG);


#endif
