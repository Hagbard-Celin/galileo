/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2012 Roman Kargin <kas1e@yandex.ru>
Copyright 2023,2025 Hagbard Celine

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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofmlib.h"
#ifdef _DEBUG_STACK
#include "stack_check.h"
#endif


void ipc_remove_list(IPCData *ipc);

// Launch a generic process
BOOL __asm __saveds L_IPC_Launch(
	register __a0 struct ListLock *list,
	register __a1 IPCData **storage,
	register __a2 char *name,
	register __d0 ULONG entry,
	register __d1 ULONG stack,
	register __d2 ULONG data,
	register __a6 struct Library *GalileoFMBase)
{
	IPCData *ipc;
	BOOL path=0;
	struct TagItem *tags;
	BPTR pathlist=0;

#ifdef _DEBUG_IPCPROC
	KPrintF("L_IPC_Launch1 Launching: %s ResTrackBase: %lx at: %lx SysBase: %lx IconBase: %lx \n", name, ResTrackBase, &ResTrackBase, SysBase, IconBase);
#else
#ifdef _DEBUG
	KPrintF("IPC Launching: %s\n", name);
#endif
#endif

	// Want path?
	if (stack&IPCF_GETPATH) path=1;

	// Clear storage
	if (storage) *storage=0;

#ifdef _DEBUG_IPCPROC
	KPrintF("L_IPC_Launch2 Launching: %s ResTrackBase: %lx at: %lx SysBase: %lx IconBase: %lx \n", name, ResTrackBase, &ResTrackBase, SysBase, IconBase);
#endif

	// Allocate data
	if (!(ipc=AllocVec(sizeof(IPCData),MEMF_CLEAR)) ||
		!(tags=AllocVec(sizeof(struct TagItem)*8,MEMF_CLEAR)))
	{
		FreeVec(ipc);
		return 0;
	}

	// Store memory and list pointers
	ipc->memory=gfmlib_data.memory;
	ipc->list=list;

	// Fill out process tags
	tags[0].ti_Tag=NP_Entry;
	tags[0].ti_Data=entry;
	tags[1].ti_Tag=NP_Name;
	tags[1].ti_Data=(ULONG)name;
	tags[2].ti_Tag=NP_WindowPtr;
	tags[2].ti_Data=(ULONG)-1;
	tags[3].ti_Tag=NP_StackSize;
	tags[3].ti_Data=IPCM_STACK(stack);
	tags[4].ti_Tag=NP_Priority;
	tags[4].ti_Data=0;

	// Want a path?
	if (path)
	{

		// Lock path list
		GetSemaphore(&gfmlib_data.path_lock,SEMF_SHARED,0);

		// Get path list copy
		pathlist=GetDosPathList(gfmlib_data.path_list);

		// Unlock path list
		FreeSemaphore(&gfmlib_data.path_lock);

#ifdef _DEBUG_IPCPROC
	KPrintF("L_IPC_Launch3 Launching: %s with Pathlist: %lx \n", name, pathlist);
#endif

		// Fill out tags
		tags[5].ti_Tag=NP_Cli;
		tags[5].ti_Data=TRUE;
		tags[6].ti_Tag=NP_Path;
		tags[6].ti_Data=(ULONG)pathlist;
		tags[7].ti_Tag=TAG_END;
	}
	else tags[5].ti_Tag=TAG_END;

	// Launch process
	ipc->proc=CreateNewProc(tags);

	// Free tags now
	FreeVec(tags);

	// Failed to launch?
	if (!ipc->proc)
	{
	// Plug memory leak
	if (path)
		FreeDosPathList(pathlist);
		
	FreeVec(ipc);
		return 0;
	}

	// Store pointer
	if (storage) *storage=ipc;

	// Send startup message
	return L_IPC_Startup(ipc,(APTR)data,0);
}


// Send an IPC startup
BOOL __asm L_IPC_Startup(
	register __a0 IPCData *ipc,
	register __a1 APTR data,
	register __a2 struct MsgPort *reply)
{
	struct MsgPort *port=0;
	IPCMessage startup;

#ifdef RESOURCE_TRACKING
#define ResTrackBase (struct Library *)FindName(&((struct ExecBase *)*((ULONG *)4))->LibList,"g_restrack.library")
#endif

	// If no message port supplied, create one
	if (!reply) port=reply=CreateMsgPort();

	// Fill out startup message
	startup.msg.mn_ReplyPort=reply;
	startup.command=IPC_STARTUP;
	startup.flags=(ULONG)ipc;
	startup.data=data;

	// Send the startup message
	PutMsg(&ipc->proc->pr_MsgPort,(struct Message *)&startup);

	// Wait for reply back
	WaitPort(reply);
	GetMsg(reply);

	// Delete port if we created one
	if (port) DeleteMsgPort(port);

	// If there's no command port, report failure
	if (startup.command!=IPC_STARTUP) return FALSE;
	return TRUE;
}


// Generic IPC startup code
IPCData *__asm L_IPC_ProcStartup(
	register __a0 ULONG *data,
	register __a1 ULONG (*__asm code)(register __a0 IPCData *,register __a1 APTR))
{
	IPCData *ipc;
	IPCMessage *msg;
	struct Task *task;
	struct MsgPort *port;
	BOOL success=1;

	// Get our task
	task=FindTask(0);

	// Get startup message
	port=&((struct Process *)task)->pr_MsgPort;
	WaitPort(port);
	msg=(IPCMessage *)GetMsg(port);

	// Get IPC pointer
	ipc=(IPCData *)msg->flags;

#ifdef _DEBUG_STACK
	L_StackCheckBegin(task, &ipc->stack_debug);
#endif

	// Store IPC pointer
	task->tc_UserData=ipc;

	// Save data pointer
	if (data) *data=(ULONG)msg->data;

	// Run startup code, create command and reply ports
	if ((code && !(code(ipc,msg->data))) ||
		!(ipc->command_port=CreateMsgPort()) ||
		!(ipc->reply_port=CreateMsgPort()))
	{
		msg->command=0;
		success=0;
	}

	// Were we successful?
	if (success)
	{
		// Want to go into a list?
		if (ipc->list)
		{
			// Lock and add to list
			L_GetSemaphore(&ipc->list->lock,SEMF_EXCLUSIVE,0, getreg(REG_A6));
			AddTail(&ipc->list->list,(struct Node *)ipc);
			L_FreeSemaphore(&ipc->list->lock, getreg(REG_A6));

			// Set flag to say we're listed
			ipc->flags|=IPCF_LISTED;
		}
	}

	// Reply to startup message
	ReplyMsg((struct Message *)msg);

	// Were we successful?
	if (success) return ipc;

	return 0;
}


// Free an IPC
void __asm __saveds L_IPC_Free(register __a0 IPCData *ipc)
{
	if (ipc)
	{
#ifdef _DEBUG_STACK
		L_StackCheckEnd(FindTask(0), &ipc->stack_debug);
#endif
		// Mark as invalid
		ipc->flags|=IPCF_INVALID;

		// If we were in a list, remove ourselves
		ipc_remove_list(ipc);

		// Free command port
		L_IPC_Flush(ipc);
		DeleteMsgPort(ipc->command_port);

		// Free reply port
		DeleteMsgPort(ipc->reply_port);

		// Free IPC data
		FreeVec(ipc);
	}
}


// Flush an IPC port
void __asm __saveds L_IPC_Flush(register __a0 IPCData *ipc)
{
	IPCMessage *msg;

	// Flush port
	if (ipc->command_port)
	{
		// Get messages
		while (msg=(IPCMessage *)GetMsg(ipc->command_port))
		{
			// Abort message
			msg->command=IPC_ABORT;

			// Reply message
			L_IPC_Reply(msg);
		}
	}
}

#ifdef RESOURCE_TRACKING
#undef ResTrackBase
#endif

// Send an IPC command
ULONG __asm __saveds L_IPC_Command(
	register __a0 IPCData *ipc,
	register __d0 ULONG command,
	register __d1 ULONG flags,
	register __a1 APTR data,
	register __a2 APTR data_free,
	register __a3 struct MsgPort *reply)
{
	struct MsgPort *port=0;
	IPCData *sender_ipc=0;
	IPCMessage *msg;
	ULONG result=1;
	struct Task *task;
	APTR memory=0;

#ifdef RESOURCE_TRACKING
	struct Library *RTlib;
	//if (!ResTrackBase)
	RTlib=(struct Library *)FindName(&((struct ExecBase *)*((ULONG *)4))->LibList,"g_restrack.library");
	//else
	//RTlib=ResTrackBase;
#ifdef _DEBUG_IPCCOMMAND
	KPrintF("IPC_Command: ResTrackBase: %lx RTlib: %lx \n", ResTrackBase, RTlib);
#endif
#define ResTrackBase RTlib
#endif

	// Valid IPC?
	if (!ipc || !ipc->command_port || ipc->flags&IPCF_INVALID)
		return 0;

	// Get this task
	task=FindTask(0);
	if (reply==NO_PORT_IPC)
		reply=0;
	else
	if (reply!=REPLY_NO_PORT_IPC)
	{
		// Get sender
		if (sender_ipc=(IPCData *)(task->tc_UserData))
			memory=sender_ipc->memory;
	}

	// Allocate message
	if (!(msg=L_AllocMemH(memory,sizeof(IPCMessage))))
		return 0;

	// Get reply port from IPC if necessary
	if (reply==REPLY_NO_PORT)
	{
		if (sender_ipc) reply=sender_ipc->reply_port;
		else reply=REPLY_NO_PORT_IPC;
	}

	// Build message
	msg->command=command;
	msg->flags=flags;
	msg->data=data;
	msg->data_free=data_free;
	msg->sender=sender_ipc;

	// Want reply, no port supplied?
	if (reply==REPLY_NO_PORT_IPC)
	{
		// Create message port
		port=CreateMsgPort();

		// Use as reply port
		msg->msg.mn_ReplyPort=port;
		reply=port;
	}

	// Want reply, port supplied?
	else
	if (reply)
	{
		// Use supplied port
		msg->msg.mn_ReplyPort=reply;

		// Async?
		if (reply->mp_Node.ln_Pri == PORT_ASYNC_MAGIC) //original dopus5code: if (reply->mp_Flags&PF_ASYNC)
			reply=0;
	}

	// Send the message
	PutMsg(ipc->command_port,(struct Message *)msg);

	// Waiting for reply?
	if (reply)
	{
		// Loop for a reply
		FOREVER
		{
			// Got a reply?
			if ((GetMsg(reply))==(struct Message *)msg) break;

/*
			// Any messages in sender's port?
			if (sender_ipc &&
				!(IsMsgPortEmpty(sender_ipc->command_port)))
			{
				IPCMessage *mes,*next;

				// Go through message list; shouldn't need to Forbid() since
				// this list won't be looked at until we return
				for (mes=(IPCMessage *)sender_ipc->command_port->mp_MsgList.lh_Head;
					mes->msg.mn_Node.ln_Succ;
					mes=next)
				{
					// Cache next message
					next=(IPCMessage *)mes->msg.mn_Node.ln_Succ;

					// Is this message FROM the IPC we just sent a message to?
					if (mes->sender==ipc)
					{
						// Does it require a reply?
						if (mes->msg.mn_ReplyPort)
						{
KPrintF("*** deadlock message from %s (%lx) to %s (%lx)! ***\n"
	"command : %lx\nflags   : %lx\ndata    : %lx\n",
mes->sender->proc->pr_Task.tc_Node.ln_Name,mes->sender->proc,
task->tc_Node.ln_Name,task,
mes->command,mes->flags,mes->data);
							// Remove this message
							Remove((struct Node *)mes);

							// Abort message
							mes->command=IPC_ABORT;

							// Quietly reply to it
							L_IPC_Reply(mes);
						}
					}
				}
			}
*/

			// Wait for a message
			Wait(1<<reply->mp_SigBit);
/*
				((sender_ipc)?(1<<sender_ipc->command_port->mp_SigBit):0));
*/
		}

		// Get result of command
		result=msg->command;

		// Free message
		if (data_free)
			FreeVec(data_free);

		L_FreeMemH(msg);

		// Free port if we created one
		if (port) DeleteMsgPort(port);
	}

	return result;
}
#ifdef RESOURCE_TRACKING
#undef ResTrackBase
#undef GalileoFMBase
#endif

// Send a safe command
ULONG __asm __saveds L_IPC_SafeCommand(
	register __a0 IPCData *ipc,
	register __d0 ULONG command,
	register __d1 ULONG flags,
	register __a1 APTR data,
	register __a2 APTR data_free,
	register __a3 struct MsgPort *reply,
	register __a4 struct ListLock *list)
{
	IPCData *look;
	ULONG res=(ULONG)-1;

	// Lock list
	L_GetSemaphore(&list->lock,SEMF_SHARED,0, getreg(REG_A6));

	// See if locker is still valid
	for (look=(IPCData *)list->list.lh_Head;
		look->node.mln_Succ;
		look=(IPCData *)look->node.mln_Succ)
	{
		// Match pointer?
		if (ipc==look)
		{
			// Send command
			res=L_IPC_Command(ipc,command,flags,data,data_free,reply);
			break;
		}
	}

	// Unlock list
	L_FreeSemaphore(&list->lock, getreg(REG_A6));
	return res;
}


// Reply to an IPC command
void __asm __saveds  L_IPC_Reply(register __a0 IPCMessage *msg)
{
	// Valid message?
	if (!msg) return;

	// Message needs a reply?
	if (msg->msg.mn_ReplyPort && msg->msg.mn_Node.ln_Type!=NT_REPLYMSG)
		ReplyMsg((struct Message *)msg);

	// Otherwise, free it
	else
	{
		if (msg->data_free)
		    FreeVec(msg->data_free);
		L_FreeMemH(msg);
	}
}


// Find a process in a list
IPCData *__asm __saveds L_IPC_FindProc(
	register __a0 struct ListLock *list,
	register __a1 char *name,
	register __d0 BOOL activate,
	register __d1 ULONG data)
{
	IPCData *ipc;

	// Lock list
	L_GetSemaphore(&list->lock,SEMF_SHARED,0, getreg(REG_A6));

	// Go through list
	for (ipc=(IPCData *)list->list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Is this the one we're after?
		if (ipc->proc &&
			ipc->proc->pr_Task.tc_Node.ln_Name &&
			strcmp(name,ipc->proc->pr_Task.tc_Node.ln_Name)==0)
			break;
	}

	// Found?
	if (ipc && ipc->node.mln_Succ)
	{
		// Send activate message?
		if (activate) L_IPC_Command(ipc,IPC_ACTIVATE,0,(APTR)data,0,0);
	}
	else ipc=0;

	// Release semaphore
	L_FreeSemaphore(&list->lock, getreg(REG_A6));

	return ipc;
}


// Kill a process by name
void __asm __saveds L_IPC_QuitName(
	register __a0 struct ListLock *list,
	register __a1 char *name,
	register __d0 ULONG quit_flags)
{
	IPCData *ipc;

	// Lock list
	L_GetSemaphore(&list->lock,SEMF_SHARED,0, getreg(REG_A6));

	// Go through list
	for (ipc=(IPCData *)list->list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Is this the one we're after?
		if (ipc->proc &&
			ipc->proc->pr_Task.tc_Node.ln_Name &&
			strcmp(name,ipc->proc->pr_Task.tc_Node.ln_Name)==0)
		{
			// Send quit message
			L_IPC_Quit(ipc,quit_flags,0);
			break;
		}
	}

	// Release semaphore
	L_FreeSemaphore(&list->lock, getreg(REG_A6));
}


// Send a quit message to an IPC process
void __asm __saveds L_IPC_Quit(
	register __a0 IPCData *ipc,
	register __d0 ULONG quit_flags,
	register __d1 BOOL wait)
{
	// Valid process?	
	if (ipc && ipc->proc)
	{
		// Signal the process with control-F
		Signal((struct Task *)ipc->proc,IPCSIG_QUIT);

		// Send quit message
		L_IPC_Command(ipc,IPC_QUIT,quit_flags,0,0,(wait)?REPLY_NO_PORT:0);
	}
}


// Send hello to a launching process
void __asm __saveds L_IPC_Hello(
	register __a0 IPCData *ipc,
	register __a1 IPCData *owner)
{
	L_IPC_Command(owner,IPC_HELLO,0,ipc,0,REPLY_NO_PORT);
}


struct Goodbye_Packet
{
	IPCData			*ipc;
	struct ListLock	*list;
};

// Send goodbye to a launching process
void __asm __saveds L_IPC_Goodbye(
	register __a0 IPCData *ipc,
	register __a1 IPCData *owner,
	register __d0 ULONG goodbye_flags)
{
	struct Goodbye_Packet *pkt=0;

	// If we were in a list, remove ourselves
	ipc_remove_list(ipc);

	// If we're in a list, need a goodbye packet for ListQuit
	if (ipc->list)
	{
		// Allocate and initialise goodbye packet
		if (pkt=AllocVec(sizeof(struct Goodbye_Packet),0))
		{
			pkt->ipc=ipc;
			pkt->list=ipc->list;
		}
	}

	// Send goodbye message to owner
	L_IPC_Command(owner,IPC_GOODBYE,goodbye_flags,ipc,pkt,0);
}


// Get a goodbye message from an IPC process
ULONG __asm __saveds L_IPC_GetGoodbye(register __a0 IPCMessage *msg)
{
	return msg->flags;
}


// Tell a list of processes to quit, and optionally wait for them
ULONG __asm __saveds L_IPC_ListQuit(
	register __a0 struct ListLock *list,
	register __a1 IPCData *owner,
	register __d0 ULONG quit_flags,
	register __d1 BOOL wait)
{
	IPCData *ipc;
	IPCMessage *msg;
	long count=0;

	// Lock list
	L_GetSemaphore(&list->lock,SEMF_SHARED,0, getreg(REG_A6));

	// Tell all processes to quit
	for (ipc=(IPCData *)list->list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Tell process to quit
		L_IPC_Quit(ipc,quit_flags,0);
		++count;
	}

	// Unlock list
	L_FreeSemaphore(&list->lock, getreg(REG_A6));

	// Not waiting?
	if (!wait || !owner) return (ULONG)count;

	// Loop while the process list is not empty
	while (!(L_IsListLockEmpty(list)))
	{
		// Go through message list
		Forbid();
		for (msg=(IPCMessage *)owner->command_port->mp_MsgList.lh_TailPred;
			msg->msg.mn_Node.ln_Pred;)
		{
			IPCMessage *next=(IPCMessage *)msg->msg.mn_Node.ln_Pred;

			// Goodbye message, from this list?
			if (msg->command==IPC_GOODBYE)
			{
				struct Goodbye_Packet *pkt;
				struct ListLock *test_list;

				// Got a packet?
				if (pkt=(struct Goodbye_Packet *)msg->data_free)
				{
					// Get list from packet
					test_list=pkt->list;
				}

				// Otherwise, get list from IPC (dangerous)
				else test_list=((IPCData *)msg->data)->list;

				// See if list matches
				if (test_list==list)
				{
					// Remove message
					Remove((struct Node *)msg);

					// Reply message
					L_IPC_Reply(msg);
				}
			}

			// Get next message
			msg=next;
		}
		Permit();

		// Wait for a message
		Wait(1<<owner->command_port->mp_SigBit);
	}

	return (ULONG)count;
}


// Send a command to list of processes
void __asm __saveds L_IPC_ListCommand(
	register __a0 struct ListLock *list,
	register __d0 ULONG command,
	register __d1 ULONG flags,
	register __d2 ULONG data,
	register __d3 BOOL wait)
{
	IPCData *ipc;
	struct MsgPort *port=0;
	long count=0;

	// If we need replies, create port
	if (wait && (port=CreateMsgPort()))
		port->mp_Node.ln_Pri = PORT_ASYNC_MAGIC; //original dopus5code: port->mp_Flags|=PF_ASYNC;

	// Lock list
	L_GetSemaphore(&list->lock,SEMF_SHARED,0, getreg(REG_A6));

	// Send command to processes in list
	for (ipc=(IPCData *)list->list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Send command
		if (L_IPC_Command(ipc,command,flags,(APTR)data,0,port)) ++count;
	}

	// Unlock list
	L_FreeSemaphore(&list->lock, getreg(REG_A6));

	// Wait for replies?
	if (port)
	{
		// Loop while there's outstanding messages
		while (count>0)
		{
			IPCMessage *msg;

			// Wait for a message
			WaitPort(port);

			// Get it
			if (msg=(IPCMessage *)GetMsg(port))
			{
				// Clear reply port and free
				msg->msg.mn_ReplyPort=0;
				L_IPC_Reply(msg);
			}

			// Decrement count
			--count;
		}

		// Free port
		DeleteMsgPort(port);
	}
}


// Remove ourselves from a list
void ipc_remove_list(IPCData *ipc)
{
	// Valid list?
	if (ipc->list && (ipc->flags&IPCF_LISTED))
	{
		L_GetSemaphore(&ipc->list->lock,SEMF_EXCLUSIVE,0, getreg(REG_A6));
		Remove((struct Node *)ipc);
		L_FreeSemaphore(&ipc->list->lock, getreg(REG_A6));
		ipc->flags&=~IPCF_LISTED;
	}
}
