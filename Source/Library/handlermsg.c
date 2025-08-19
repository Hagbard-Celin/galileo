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

#define RXMF_SYNC		(1<<2)

// Free an AppMessage
void __asm __saveds L_FreeHandlerMessage(
	register __a0 GalileoHandlerMessage *msg)
{
    // Valid message?
    if (msg)
    {
	if (!strcmp(msg->ghm_Command, "dropfrom") || !strcmp(msg->ghm_Command, "drop"))
	{
	    GalileoHandlerAppMessage *amsg;
	    ULONG arg;

	    amsg = (GalileoHandlerAppMessage *)msg;
	    
	    if (amsg->gham_DestLock)
		UnLock(amsg->gham_DestLock);

	    // Free any arguments
	    if (amsg->gham_ArgList)
	    {
	        for (arg=0;arg<amsg->gham_NumArgs;arg++)
	        {
		    if (amsg->gham_ArgList[arg].wa_Lock && !(amsg->gham_ArgData[arg].glad_Flags&WBAF_NOT_A_LOCK))
		        UnLock(amsg->gham_ArgList[arg].wa_Lock);
		    if (amsg->gham_ArgList[arg].wa_Name)
		        L_FreeMemH(amsg->gham_ArgList[arg].wa_Name);
	        }
	    }
	}
	else
	{
	    if (!(msg->ghm_Flags&RXMF_SYNC))
	    {
		if (msg->ghm_DestLock)
		    UnLock(msg->ghm_DestLock);

		if (msg->ghm_Args)
		    L_FreeMemH(msg->ghm_Args);
	    }
	}

	// Free message
	L_FreeMemH(msg);
    }
}

// Reply to an AppMessage
void __asm __saveds L_ReplyHandlerMessage(register __a0 GalileoHandlerMessage *msg)
{
    // Valid message?
    if (msg)
    {
	if (msg->ghm_Message.mn_ReplyPort)
	{
	    ReplyMsg((struct Message *)msg);
	}
	else
	{
	    L_FreeHandlerMessage(msg);
	}
    }
}
