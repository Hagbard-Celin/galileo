/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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


// Allocate an AppMessage
struct AppMessage *__asm __saveds L_AllocAppMessage(
	register __a0 APTR memory,
	register __a1 struct MsgPort *reply,
	register __d0 WORD num)
{
	struct AppMessage *msg;

	// Allocate message
	if (!(msg=L_AllocMemH(memory,
		sizeof(struct AppMessage)+
			((sizeof(struct WBArg))*num) + 1))) return 0;

	// Set check pointer
	//msg->am_Reserved[7]=(ULONG)msg;

	// Set ArgList pointer
	if (num>0) msg->am_ArgList=(struct WBArg *)(msg+1);
	msg->am_NumArgs=num;

	// Set drop position array pointer
	//if (num>0) msg->ga_DropPos=(Point *)(msg->am_ArgList+num);

	// Fill out miscellaneous fields
	msg->am_Message.mn_ReplyPort=reply;
	msg->am_Version=AM_VERSION;
	CurrentTime(&msg->am_Seconds,&msg->am_Micros);

	return msg;
}

// Free an AppMessage
void __asm __saveds L_FreeAppMessage(
	register __a0 struct AppMessage *msg)
{
	// Valid message?
	if (msg)
	{
		WORD arg;

		// Free any arguments
		if (msg->am_ArgList)
		{
			for (arg=0;arg<msg->am_NumArgs;arg++)
			{
				UnLock(msg->am_ArgList[arg].wa_Lock);
				L_FreeMemH(msg->am_ArgList[arg].wa_Name);
			}
		}

		// Free message
		L_FreeMemH(msg);
	}
}

// Reply to an AppMessage
void __asm __saveds L_ReplyAppMessage(
	register __a0 struct AppMessage *msg)
{
	// Valid message?
	if (msg)
	{
		if (msg->am_Message.mn_ReplyPort)
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
BOOL __asm __saveds L_CheckAppMessage(register __a0 struct AppMessage *msg)
{
	// Check check pointer
	return (BOOL)(msg->am_Type == MTYPE_LISTER_APPWINDOW &&	msg->am_Class == GLAMCLASS_LISTER);
	//return (BOOL)(msg && msg->ga_Msg.am_Reserved[7]==(ULONG)msg);
}


// Copy a GalileoAppMessage
struct AppMessage *__asm __saveds L_CopyAppMessage(
	register __a0 struct AppMessage *orig,
	register __a1 APTR memory)
{
	struct AppMessage *msg;
	WORD arg;

	// Is the original a Galileo message?
	//if (!L_CheckAppMessage(orig)) return 0;

	// Allocate new message
	if (!(msg=L_AllocAppMessage(memory,0,orig->am_NumArgs))) return 0;

	// Copy AppMessage fields
	msg->am_Type=orig->am_Type;
	msg->am_UserData=orig->am_UserData;
	msg->am_ID=orig->am_ID;
	msg->am_Class=orig->am_Class;
	msg->am_MouseX=orig->am_MouseX;
	msg->am_MouseY=orig->am_MouseY;
	msg->am_Seconds=orig->am_Seconds;
	msg->am_Micros=orig->am_Micros;

	// Copy arguments
	for (arg=0;arg<orig->am_NumArgs;arg++)
	{
		// Copy argument
		L_SetWBArg(
			msg,
			arg,
			orig->am_ArgList[arg].wa_Lock,
			orig->am_ArgList[arg].wa_Name,
			memory);
	}

	return msg;
}


// Set a WBArg entry
BOOL __asm __saveds L_SetWBArg(
	register __a0 struct AppMessage *msg,
	register __d0 WORD num,
	register __d1 BPTR lock,
	register __a1 char *name,
	register __a2 APTR memory)
{
	// Valid message and number?
	if (!msg || num>msg->am_NumArgs) return 0;

	// Null-name?
	if (!name) name="";

	// Copy name
	if (msg->am_ArgList[num].wa_Name=L_AllocMemH(memory,strlen(name)+1))
		strcpy(msg->am_ArgList[num].wa_Name,name);

	// Copy lock
	if (lock) msg->am_ArgList[num].wa_Lock=DupLock(lock);

	return 1;
}
