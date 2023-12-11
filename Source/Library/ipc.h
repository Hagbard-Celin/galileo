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

#ifndef _GALILEOFM_IPC
#define _GALILEOFM_IPC

typedef struct {
	struct Message	msg;
	ULONG			command;
	ULONG			flags;
	APTR			data;
	APTR			data_free;
} IPCMessage;

typedef struct {
	struct MinNode			node;
	struct Process			*proc;			// Process pointer
	IPCMessage				startup_msg;	// Startup message
	struct MsgPort			*command_port;	// Port to send commands to
	struct ListLock			*list;			// List we're a member of
} IPCData;

enum {
	IPC_COMMAND_BASE=0x8000000,
	IPC_STARTUP,
	IPC_ABORT,
	IPC_QUIT,
	IPC_ACTIVATE,
	IPC_NEW,
};


// protos
ipc_launch(struct ListLock *,IPCData **,char *,ULONG,ULONG,ULONG);
ipc_startup(IPCData *ipc,APTR data,struct MsgPort *reply);
ULONG ipc_command(IPCData *ipc,ULONG command,ULONG flags,APTR data,APTR data_free,struct MsgPort *reply);
void ipc_reply(IPCMessage *msg);
void ipc_free(IPCData *);
void ipc_free_port(IPCData *ipc);
IPCData *ipc_find_proc(struct ListLock *,char *);
IPCData *ipc_proc_startup(ULONG *,ULONG (*)(IPCData *,APTR));

#endif
