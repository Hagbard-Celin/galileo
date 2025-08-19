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

// Allocate a timer
struct TimerHandle *__asm __saveds L_AllocTimer(
	register __d0 ULONG unit,
	register __a0 struct MsgPort *port)
{
	TimerHandle *handle;

	// Allocate handle
	if (!(handle=AllocVec(sizeof(TimerHandle),MEMF_CLEAR)))
		return 0;

	// Open message port if we need one
	if (!port)
	{
		if (!(handle->my_port=CreateMsgPort()))
		{
			FreeVec(handle);
			return 0;
		}
		port=handle->my_port;
	}

	// Store port address
	handle->port=port;

	// Open timer
	if (OpenDevice("timer.device",unit,&handle->req.tr_node,0))
	{
		if (handle->my_port) DeleteMsgPort(handle->my_port);
		FreeVec(handle);
		return 0;
	}

	// Initialise timer
	handle->req.tr_node.io_Message.mn_ReplyPort=port;
	handle->req.tr_node.io_Command=TR_ADDREQUEST;
	return handle;
}


// Free a timer
void __asm __saveds L_FreeTimer(register __a0 TimerHandle *handle)
{
	// Valid handle?
	if (handle)
	{
		// Request active?
		L_StopTimer(handle);

		// Close device
		CloseDevice(&handle->req.tr_node);
		if (handle->my_port) DeleteMsgPort(handle->my_port);
		FreeVec(handle);
	}
}


// Start timer
void __asm __saveds L_StartTimer(
	register __a0 TimerHandle *handle,
	register __d0 ULONG seconds,
	register __d1 ULONG micros)
{
	// Valid handle?
	if (handle)
	{
		// Stop existing request
		L_StopTimer(handle);

		// Send new request	
		handle->req.tr_time.tv_secs=seconds;
		handle->req.tr_time.tv_micro=micros;
		SendIO(&handle->req.tr_node);
		handle->active=1;
	}
}


// Check timer
BOOL __asm __saveds L_CheckTimer(register __a0 TimerHandle *handle)
{
	// Valid handle?
	if (handle)
	{
		// Request active?
		if (handle->active)
		{
			// Check request
			if (CheckIO(&handle->req.tr_node))
			{
				// Yep, done
				WaitIO(&handle->req.tr_node);
				handle->active=0;
				return 1;
			}
		}
	}

	return 0;
}


// Abort timer
void __asm __saveds L_StopTimer(register __a0 TimerHandle *handle)
{
	// Valid handle?
	if (handle)
	{
		// Request already active?
		if (handle->active)
		{
			// Abort request
			AbortIO(&handle->req.tr_node);
			WaitIO(&handle->req.tr_node);

			// Clear active flag
			handle->active=0;

			// Initialise request
			handle->req.tr_node.io_Command=TR_ADDREQUEST;
			handle->req.tr_node.io_Flags=0;
		}
	}
}


// See if timer is running
BOOL __asm __saveds L_TimerActive(register __a0 TimerHandle *handle)
{
	// Valid and running?
	return (BOOL)(handle && handle->active);
}


// Get timer base
struct Library *__asm __saveds L_GetTimerBase(register __a6 struct Library *GalileoFMBase)
{
	return gfmlib_data.TimerBase;
}
