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

// Allocate an AppMessage
GalileoAppMessage *__asm __saveds L_AllocAppMessage(
	register __a0 APTR memory,
	register __a1 struct MsgPort *reply,
	register __d0 short num)
{
	GalileoAppMessage *msg;

	// Allocate message
	if (!(msg=L_AllocMemH(memory,
		sizeof(GalileoAppMessage)+
			((sizeof(struct WBArg)+sizeof(Point))*num)))) return 0;

	// Set check pointer
	msg->da_Msg.am_Reserved[7]=(ULONG)msg;

	// Set ArgList pointer
	if (num>0) msg->da_Msg.am_ArgList=(struct WBArg *)(msg+1);
	msg->da_Msg.am_NumArgs=num;

	// Set drop position array pointer
	if (num>0) msg->da_DropPos=(Point *)(msg->da_Msg.am_ArgList+num);

	// Fill out miscellaneous fields
	msg->da_Msg.am_Message.mn_ReplyPort=reply;
	msg->da_Msg.am_Version=AM_VERSION;
	CurrentTime(&msg->da_Msg.am_Seconds,&msg->da_Msg.am_Micros);

	return msg;
}

// Free an AppMessage
void __asm __saveds L_FreeAppMessage(
	register __a0 GalileoAppMessage *msg)
{
	// Valid message?
	if (msg)
	{
		short arg;

		// Free any arguments
		if (msg->da_Msg.am_ArgList)
		{
			for (arg=0;arg<msg->da_Msg.am_NumArgs;arg++)
			{
				UnLock(msg->da_Msg.am_ArgList[arg].wa_Lock);
				L_FreeMemH(msg->da_Msg.am_ArgList[arg].wa_Name);
			}
		}

		// Free message
		L_FreeMemH(msg);
	}
}

// Reply to an AppMessage
void __asm __saveds L_ReplyAppMessage(
	register __a0 GalileoAppMessage *msg)
{
	// Valid message?
	if (msg)
	{
		if (msg->da_Msg.am_Message.mn_ReplyPort)
		{
			ReplyMsg((struct Message *)msg);
		}
		else
		{
			L_FreeAppMessage(msg);
		}
	}
}

// Check if a message is a galileo one
BOOL __asm __saveds L_CheckAppMessage(register __a0 GalileoAppMessage *msg)
{
	// Check check pointer
	return (BOOL)(msg && msg->da_Msg.am_Reserved[7]==(ULONG)msg);
}


// Copy a GalileoAppMessage
GalileoAppMessage *__asm __saveds L_CopyAppMessage(
	register __a0 GalileoAppMessage *orig,
	register __a1 APTR memory)
{
	GalileoAppMessage *msg;
	short arg;

	// Is the original a Galileo message?
	if (!L_CheckAppMessage(orig)) return 0;

	// Allocate new message
	if (!(msg=L_AllocAppMessage(memory,0,orig->da_Msg.am_NumArgs))) return 0;

	// Copy AppMessage fields
	msg->da_Msg.am_Type=orig->da_Msg.am_Type;
	msg->da_Msg.am_UserData=orig->da_Msg.am_UserData;
	msg->da_Msg.am_ID=orig->da_Msg.am_ID;
	msg->da_Msg.am_Class=orig->da_Msg.am_Class;
	msg->da_Msg.am_MouseX=orig->da_Msg.am_MouseX;
	msg->da_Msg.am_MouseY=orig->da_Msg.am_MouseY;
	msg->da_Msg.am_Seconds=orig->da_Msg.am_Seconds;
	msg->da_Msg.am_Micros=orig->da_Msg.am_Micros;

	// Copy arguments
	for (arg=0;arg<orig->da_Msg.am_NumArgs;arg++)
	{
		// Copy argument
		L_SetWBArg(
			msg,
			arg,
			orig->da_Msg.am_ArgList[arg].wa_Lock,
			orig->da_Msg.am_ArgList[arg].wa_Name,
			memory);
	}

	// Copy Drop-positions
	if (orig->da_DropPos)
		CopyMem(
			(char *)orig->da_DropPos,
			(char *)msg->da_DropPos,
			sizeof(Point)*orig->da_Msg.am_NumArgs);

	// Copy other info
	msg->da_DragOffset=orig->da_DragOffset;
	msg->da_Flags=orig->da_Flags;

	return msg;
}


// Set a WBArg entry
BOOL __asm __saveds L_SetWBArg(
	register __a0 GalileoAppMessage *msg,
	register __d0 short num,
	register __d1 BPTR lock,
	register __a1 char *name,
	register __a2 APTR memory)
{
	// Valid message and number?
	if (!msg || num>msg->da_Msg.am_NumArgs) return 0;

	// Null-name?
	if (!name) name="";

	// Copy name
	if (msg->da_Msg.am_ArgList[num].wa_Name=L_AllocMemH(memory,strlen(name)+1))
		strcpy(msg->da_Msg.am_ArgList[num].wa_Name,name);

	// Copy lock
	if (lock) msg->da_Msg.am_ArgList[num].wa_Lock=DupLock(lock);

	return 1;
}
