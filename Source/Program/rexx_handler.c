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

#include "galileofm.h"

short rexx_handler_msg(
	char *handler,
	DirBuffer *buffer,
	short flags,
	ULONG args,...)
{
	return rexx_handler_msg_args(handler,buffer,flags,(HandlerArg *)&args);
}

short __stdargs rexx_handler_msg_args(
	char *handler,
	DirBuffer *buffer,
	short flags,
	HandlerArg *args)
{
	struct RexxMsg *msg;
	char qualbuf[40];
	short arg,res;
	unsigned long type=0,count=0;
	RexxDespatch *desp;
	struct MsgPort *port,*reply_port;

	// If no handler supplied, use buffer's handler
	if ((!handler || !*handler) && buffer) handler=buffer->buf_CustomHandler;

	// If no custom handler or no ARexx, return
	if (!*handler || !RexxSysBase) return 0;

	// Allocate despatch packet
	if (!(desp=AllocVec(sizeof(RexxDespatch),MEMF_CLEAR)))
		return 0;

	// Find our ARexx port
	Forbid();
	port=FindPort(GUI->rexx_port_name);
	Permit();

	// Create message packet
	if (!(msg=CreateRexxMsg(port,0,GUI->rexx_port_name)))
	{
		Permit();
		FreeVec(desp);
		return 0;
	}

	// Go through arguments
	for (arg=0;args[arg].ha_Type!=TAG_END;arg++)
	{
		// Argument not too large?
		if (args[arg].ha_Arg>15) continue;

		// Value?
		if (args[arg].ha_Type==HA_Value)
		{
			// Set message slot and conversion key bit
			msg->rm_Args[args[arg].ha_Arg]=(STRPTR)args[arg].ha_Data;
			type|=1<<args[arg].ha_Arg;
		}

		// Qualifier?
		else
		if (args[arg].ha_Type==HA_Qualifier)
		{
			// Get qualifier
			qualbuf[0]=0;
			if (args[arg].ha_Data&IEQUAL_ANYSHIFT) strcat(qualbuf,"shift ");
			if (args[arg].ha_Data&IEQUAL_ANYALT) strcat(qualbuf,"alt ");
			if (args[arg].ha_Data&IEQUALIFIER_CONTROL) strcat(qualbuf,"control ");
			if (args[arg].ha_Data&IEQUALIFIER_SUBDROP) strcat(qualbuf,"subdrop");

			// Set message slot
			msg->rm_Args[args[arg].ha_Arg]=qualbuf;
		}

		// String
		else msg->rm_Args[args[arg].ha_Arg]=(STRPTR)args[arg].ha_Data;

		// Fix count
		if (args[arg].ha_Arg>count) count=args[arg].ha_Arg;
	}

	// Increment count
	++count;

	// Set count
	msg->rm_Action=count|RXFUNC;//|RXFF_RESULT;

	// Fill in message
	if (!(FillRexxMsg(msg,count,type)))
	{
		// Failed
		DeleteRexxMsg(msg);
		FreeVec(desp);
		return 0;
	}

	// Fill in despatch packet
	stccpy(desp->handler,handler,sizeof(desp->handler));
	desp->rx_msg=msg;

	// Async?
	if (!(flags&RXMF_SYNC) || !(reply_port=CreateMsgPort()))
	{
		// Send to REXX process
		IPC_Command(GUI->rexx_proc,REXXCMD_SEND_RXMSG,flags,0,desp,0);
		return 1;
	}

	// Send message directly
	if (res=rexx_send_rxmsg_args(desp,flags,reply_port))
	{
		// Wait for reply
		WaitPort(reply_port);
		GetMsg(reply_port);

		// Get result
		res=msg->rm_Result1;

        // Clear message
        ClearRexxMsg(msg,count);

		// Free message
		DeleteRexxMsg(msg);
	}

    // Free the replyport
    DeleteMsgPort(reply_port);

	// Free despatch
	FreeVec(desp);
	return res;
}
