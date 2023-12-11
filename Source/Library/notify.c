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

#include "galileofmlib.h"

// Add a notify request
APTR __asm __saveds L_AddNotifyRequest(
	register __d0 ULONG type,
	register __d1 ULONG userdata,
	register __a0 struct MsgPort *port,
	register __a6 struct MyLibrary *libbase)
{
	NotifyNode *node;
	struct LibData *data;

	// Allocate a new node
	if (!(node=AllocVec(sizeof(NotifyNode),MEMF_CLEAR)))
		return 0;

	// Fill in node
	node->port=port;
	node->userdata=userdata;
	node->type=type;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Lock notify list
	ObtainSemaphore(&data->notify_lock);

	// Add to list
	AddTail((struct List *)&data->notify_list,(struct Node *)node);

	// Unlock notify list
	ReleaseSemaphore(&data->notify_lock);
	return node;
}


// Remove a notify request
void __asm __saveds L_RemoveNotifyRequest(
	register __a0 NotifyNode *node,
	register __a6 struct MyLibrary *libbase)
{
	GalileoNotify *msg;

	// Valid node?
	if (!node) return;

	// Lock notify list
	L_GetSemaphore(&((struct LibData *)libbase->ml_UserData)->notify_lock,SEMF_EXCLUSIVE,0);

	// Remove node
	Remove((struct Node *)node);

	// Forbid to examine port
	Forbid();

	// Go through messages in port
	for (msg=(GalileoNotify *)node->port->mp_MsgList.lh_Head;
		msg->gn_Msg.mn_Node.ln_Succ;)
	{
		GalileoNotify *next=(GalileoNotify *)msg->gn_Msg.mn_Node.ln_Succ;

		// Notify message?
		if (msg->gn_Msg.mn_Node.ln_Type==NT_GALILEOFM_NOTIFY &&
			(node->type&msg->gn_Type))
		{
			// Remove and free this message
			Remove((struct Node *)msg);
			FreeVec(msg);
		}

		// Get next
		msg=next;
	}

	// Enable multitasking
	Permit();

	// Unlock notify list
	L_FreeSemaphore(&((struct LibData *)libbase->ml_UserData)->notify_lock);

	// Free node
	FreeVec(node);
}


// Send a notify message
void __asm __saveds L_SendNotifyMsg(
	register __d0 ULONG type,
	register __d1 ULONG data,
	register __d2 ULONG flags,
	register __d3 short wait,
	register __a0 char *name,
	register __a1 struct FileInfoBlock *fib,
	register __a6 struct MyLibrary *libbase)
{
	NotifyNode *node;
	struct Task *this_task=0;
	struct MsgPort reply_port;
	short count=0;

	// Initialise fake reply port if needed
	if (wait)
	{
		// Get current task
		this_task=FindTask(0);

		// Fill out port
		reply_port.mp_Flags=PA_SIGNAL;
		reply_port.mp_SigBit=SIGB_INTUITION;
		reply_port.mp_SigTask=this_task;
		NewList(&reply_port.mp_MsgList);
	}

	// Lock notify list
	L_GetSemaphore(&((struct LibData *)libbase->ml_UserData)->notify_lock,SEMF_SHARED,0);

	// Go through notify list
	for (node=(NotifyNode *)((struct LibData *)libbase->ml_UserData)->notify_list.mlh_Head;
		node->node.mln_Succ;
		node=(NotifyNode *)node->node.mln_Succ)
	{
		GalileoNotify *msg;

		// Correct type?
		if (node->type&type)
		{
			short size;

			// Calculate size
			size=sizeof(GalileoNotify);
			if (name) size+=strlen(name);
			if (fib) size+=sizeof(struct FileInfoBlock)+1;

			// Allocate message
			if (msg=AllocVec(size,MEMF_CLEAR))
			{
				// Initialise message
				msg->gn_Msg.mn_Node.ln_Type=NT_GALILEOFM_NOTIFY;
				msg->gn_Msg.mn_Length=size;
				msg->gn_Type=type;
				msg->gn_Data=data;
				msg->gn_Flags=flags;
				msg->gn_UserData=node->userdata;
				if (name) strcpy(msg->gn_Name,name);

				// Got FileInfoBlock?
				if (fib)
				{
					char *ptr;

					// Get pointer to end of name
					ptr=msg->gn_Name+strlen(msg->gn_Name)+1;
					if (((ULONG)ptr)&1) ++ptr;

					// Set pointer and copy it
					msg->gn_Fib=(struct FileInfoBlock *)ptr;
					CopyMem((char *)fib,ptr,sizeof(struct FileInfoBlock));
				}

				// Wait for reply?
				if (wait)
				{
					// Prevent deadlocks
					if (node->port->mp_SigTask!=this_task)
					{
						// Set reply port address
						msg->gn_Msg.mn_ReplyPort=&reply_port;

						// Increment count
						++count;
					}
				}

				// Send message
				PutMsg(node->port,(struct Message *)msg);
			}
		}
	}

	// Unlock notify list
	L_FreeSemaphore(&((struct LibData *)libbase->ml_UserData)->notify_lock);

	// Wait for replies?
	if (wait && count)
	{
		// While outstanding replies
		while (count>0)
		{
			GalileoNotify *msg;

			// Get replies
			WaitPort(&reply_port);
			while (msg=(GalileoNotify *)GetMsg(&reply_port))
			{
				// Free message
				FreeVec(msg);

				// Decrement count
				--count;
			}
		}
	}
}


// Change notification flags
void __asm __saveds L_SetNotifyRequest(
	register __a0 NotifyNode *node,
	register __d0 ULONG new_flags,
	register __d1 ULONG mask,
	register __a6 struct MyLibrary *libbase)
{
	// Valid node?
	if (!node) return;

	// Set new flag values
	node->type=(node->type&~mask)|new_flags;
}
