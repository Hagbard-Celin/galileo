/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

This program is based on the source code of Directory Opus Magellan II,
released by GPSoftware under the APL license in 2012. Re-licensed under GPL by
permission of Dr Greg Perry, Managing Director of GPSoftware.

Opus® and Directory Opus® and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#ifndef _GALILEOFMBASE_IPC_H
#define _GALILEOFMBASE_IPC_H

#include <exec/ports.h>
#ifdef _DEBUG_STACK
#include <gfm/stack_check_ipc.h>
#endif

// Inter-Process Communication
typedef struct {
    struct Message  msg;		    // Exec message
    ULONG	    command;		    // Message command
    ULONG	    flags;		    // Message flags
    APTR	    data;		    // Message data
    APTR	    data_free;		    // Data to be FreeVec()ed automatically
    struct _IPC	    *sender;		    // Sender IPC
} IPCMessage;


#define PORT_ASYNC_MAGIC  0x5A // original dopus5code: #define PF_ASYNC	(1<<7)

#define REPLY_NO_PORT		(struct MsgPort *)-1
#define REPLY_NO_PORT_IPC	(struct MsgPort *)-2
#define NO_PORT_IPC		(struct MsgPort *)-3

typedef struct _IPC {
    struct MinNode	    node;
    struct Process	    *proc;	    // Process pointer
    IPCMessage		    startup_msg;    // Startup message
    struct MsgPort	    *command_port;  // Port to send commands to
    struct ListLock	    *list;	    // List we're a member of
    APTR		    userdata;
    APTR		    memory;	    // Memory
    struct MsgPort	    *reply_port;    // Port for replies
    ULONG		    flags;	    // Flags
#ifdef _DEBUG_STACK
    StackData		    stack_debug;
#endif
} IPCData;

#define IPCF_INVALID		(1<<0)
#define IPCF_LISTED		(1<<1)
#define IPCF_STARTED		(1<<2)
#define IPCF_DETACHEDCHILD	(1<<3) // started asynchronous child
#define IPCF_ASYNC		(1<<4) // running asynchronously

#define IPCDATA(ipc)		((APTR)ipc->userdata)
#define SET_IPCDATA(ipc,data)	ipc->userdata=(APTR)data

#define FUNCASYNC(ipc)		(ipc->flags&IPCF_ASYNC)

#define IPCF_GETPATH		(1<<31)
#define IPCF_DEBUG		(1<<30)
#define IPCM_STACK(s)		(s&0xffffff)

enum {
    IPC_COMMAND_BASE=0x8000000,
    IPC_STARTUP,
    IPC_ABORT,
    IPC_QUIT,
    IPC_ACTIVATE,
    IPC_HELLO,
    IPC_GOODBYE,
    IPC_HIDE,
    IPC_SHOW,
    IPC_RESET,
    IPC_HELP,
    IPC_NEW,
    IPC_GOT_GOODBYE,
    IPC_IDENTIFY,
    IPC_PRIORITY,
    IPC_REMOVE,
    IPC_OPEN,
};

#define IPCSIG_HIDE		SIGBREAKF_CTRL_D
#define IPCSIG_SHOW		SIGBREAKF_CTRL_E
#define IPCSIG_QUIT		SIGBREAKF_CTRL_F

BOOL IPC_Launch(struct ListLock *,IPCData **,char *,ULONG,ULONG,ULONG);
void IPC_Free(IPCData *);
void IPC_Flush(IPCData *);
ULONG IPC_Command(IPCData *,ULONG,ULONG,APTR,APTR,struct MsgPort *);
IPCData *IPC_ProcStartup(ULONG *,ULONG (*__asm)(register __a0 IPCData *,register __a1 APTR));
void IPC_Reply(IPCMessage *);
ULONG IPC_GetGoodbye(IPCMessage *);
void IPC_ListCommand(struct ListLock *list,ULONG command,ULONG flags,ULONG data,BOOL wait);
ULONG IPC_ListQuit(struct ListLock *list,IPCData *owner,ULONG quit_flags,BOOL wait);
void IPC_Goodbye(IPCData *,IPCData *,ULONG);
void IPC_Quit(IPCData *,ULONG,BOOL);

#endif
