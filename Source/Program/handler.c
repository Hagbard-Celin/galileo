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

#include "galileofm.h"
#include "app_msg_protos.h"
#include "dirlist_protos.h"
#include "requesters.h"
#include "rexx.h"
#include "file_select.h"
#include "lsprintf_protos.h"
#include "handler.h"

// Allocate an AppMessage
void galileo_handler_msg(STRPTR handler,
			 STRPTR action,
		         DirEntry *entry,
			 Lister *source,
			 Lister *dest,
			 BPTR dest_lock,
			 STRPTR args,
			 FunctionHandle *handle,
			 ULONG data,
			 UWORD qual,
			 UWORD flags)
{
    struct MsgPort *port;
    STRPTR find_port;
    GalileoHandlerMessage *msg;

    // Allocate message
    if (!(msg = AllocMemH(NULL,
	sizeof(GalileoHandlerMessage))))
    {
	return;
    }

    // Set ArgList pointer
    msg->ghm_Command = action;
    msg->ghm_Data = data;
    msg->ghm_SourceLister = (APTR)source;
    msg->ghm_DestLister = (APTR)dest;
    msg->ghm_Entry = (APTR)entry;
    msg->ghm_Qual = qual;
    msg->ghm_Flags = flags;
    if (flags&RXMF_SYNC)
    {
	msg->ghm_DestLock = dest_lock;
	if (args && args[0])
	    msg->ghm_Args = args;
	msg->ghm_FunctionHandle = (APTR)handle;
    }
    else
    {
	if (dest_lock)
	    msg->ghm_DestLock = DupLock(dest_lock);
	if (args && args[0])
	    msg->ghm_Args = CopyString(NULL, args);
	msg->ghm_FunctionHandle = 0;
    }

    if (handler)
	find_port = handler;
    else
	find_port = source->cur_buffer->buf_CustomHandler;

    // Find port to send to
    Forbid();
    if (!(port=FindPort(find_port)))
    {
	char buf[128];

	// Port not found
	Permit();


	// Build error message
	lsprintf(buf, GetString(&locale, MSG_CUSTPORT_NOT_FOUND), source->cur_buffer->buf_CustomHandler);

	// Show error
	global_requester(buf);

	// Free message
	FreeMemH(msg);
    }

    // Got port
    else
    {
	struct MsgPort *reply_port = 0;

	// Sync?
	if (flags&RXMF_SYNC)
	    reply_port = CreateMsgPort();

	msg->ghm_Message.mn_ReplyPort = reply_port;

	// Send the message
	PutMsg(port,(struct Message *)msg);

	// Permit now its sent
	Permit();

	if (flags&RXMF_SYNC && reply_port)
	{
		// Wait for reply
		WaitPort(reply_port);
		GetMsg(reply_port);

		// Get result
		//res=msg->rm_Result1;
		FreeHandlerMessage(msg);
	}

	// Free the replyport
	if (reply_port)
	    DeleteMsgPort(reply_port);
    }
}

// Allocate an AppMessage
GalileoHandlerAppMessage *alloc_handler_appmsg(APTR memory,
					     struct MsgPort *reply,
					     ULONG num)
{
    GalileoHandlerAppMessage *msg;

    // Allocate message
    if (!(msg = AllocMemH(memory,
			    sizeof(GalileoHandlerAppMessage)+
			    ((sizeof(struct WBArg) + sizeof(struct GLAData)) * num))))
    {
	return 0;
    }

    // Set ArgList pointer
    msg->gham_NumArgs=num;

    // Set ArgList pointer
    if (num>0) msg->gham_ArgData=(struct GLAData *)(msg->gham_ArgList + num);

    return msg;
}


// Allocate and fill out an AppMessage
GalileoHandlerAppMessage *alloc_handler_appmsg_files(DirEntry *entry,
	                                             DirBuffer *buffer,
	                                             BOOL multifiles,
	                                             STRPTR action,
	                                             Lister *dest,
						     BPTR dest_lock,
	                                             UWORD qual)
{
    GalileoHandlerAppMessage *msg;

    // Allocate AppMessage
    if (!(msg = alloc_handler_appmsg(global_memory_pool,
				     GUI->appmsg_port,
				     (multifiles)?(buffer->buf_SelectedFiles[0]+buffer->buf_SelectedDirs[0]):1)))
		    
	    
    {
	return 0;
    }
	
    msg->gham_DestLock = DupLock(dest_lock);
    msg->gham_SourceLister = buffer->buf_CurrentLister;
    msg->gham_DestLister = dest;
    msg->gham_Command = action;
    msg->gham_Qual = qual;

    // Any arguments?
    if (msg->gham_NumArgs>0)
	set_wbargs_files(msg->gham_NumArgs, msg->gham_ArgList, msg->gham_ArgData, buffer->buf_Lock, entry, buffer);

    return msg;
}
