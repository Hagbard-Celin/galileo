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

#include "diskchange.h"

void init_diskchange(DiskChangeData *data)
{
	short a;

	// Clear pointers
	for (a=0;a<4;a++)
	{
		data->req[a]=0;
		data->count[a]=0;
	}
	data->flags=0;

	// Create message port
	if (!(data->port=CreateMsgPort()))
		return;

	// Initialise interrupt
	data->interrupt.is_Node.ln_Type=NT_UNKNOWN;
	data->interrupt.is_Node.ln_Pri=0;
	data->interrupt.is_Data=FindTask(0);
	data->interrupt.is_Code=(void (*)())diskchange_code;

	// Open devices
	for (a=0;a<4;a++)
	{
		// Create IORequest	
		if (data->req[a]=(struct IOExtTD *)CreateIORequest(data->port,sizeof(struct IOExtTD)))
		{
			// Open trackdisk device
			if (OpenDevice("trackdisk.device",a,(struct IORequest *)data->req[a],0))
			{
				// Failed
				DeleteIORequest(data->req[a]);
				data->req[a]=0;
			}

			// It's open
			else
			{
				// Get initial change count
				data->req[a]->iotd_Req.io_Flags=IOF_QUICK;
				data->req[a]->iotd_Req.io_Command=TD_CHANGENUM;
				BeginIO((struct IORequest *)data->req[a]);

				// Store change count
				data->count[a]=data->req[a]->iotd_Req.io_Actual;

				// See if there's a disk in the drive
				data->req[a]->iotd_Req.io_Flags=IOF_QUICK;
				data->req[a]->iotd_Req.io_Command=TD_CHANGESTATE;
				BeginIO((struct IORequest *)data->req[a]);

				// If there is, set flag
				if (data->req[a]->iotd_Req.io_Actual==0)
					data->flags|=(1<<a);

				// Start diskchange interrupt
				data->req[a]->iotd_Req.io_Length=sizeof(struct Interrupt);
				data->req[a]->iotd_Req.io_Data=(APTR)&data->interrupt;
				data->req[a]->iotd_Req.io_Command=TD_ADDCHANGEINT;
				SendIO((struct IORequest *)data->req[a]);
			}
		}
	}
}


void free_diskchange(DiskChangeData *data)
{
	short a;

	// Go through units
	for (a=0;a<4;a++)
	{
		// Got device open?
		if (data->req[a])
		{
			// Remove diskchange interrupt
			data->req[a]->iotd_Req.io_Length=sizeof(struct Interrupt);
			data->req[a]->iotd_Req.io_Data=(APTR)&data->interrupt;
			data->req[a]->iotd_Req.io_Command=TD_REMCHANGEINT;
			DoIO((struct IORequest *)data->req[a]);

			// Close device
			CloseDevice((struct IORequest *)data->req[a]);

			// Free IORequest
			DeleteIORequest((struct IORequest *)data->req[a]);
		}
	}

	// Free message port
    struct Message *msg;
        while (msg=GetMsg(data->port))
			ReplyFreeMsg(msg);
	DeleteMsgPort(data->port);
}


void __asm __saveds diskchange_code(register __a1 struct Task *task)
{
	// Signal our task
	Signal(task,SIGBREAKF_CTRL_F);
}


unsigned long diskchange_check(DiskChangeData *data)
{
	short a;
	unsigned long change=0;

	// Go through units
	for (a=0;a<4;a++)
	{
		// Device open?
		if (data->req[a])
		{
			// Get change count
			data->req[a]->iotd_Req.io_Flags=IOF_QUICK;
			data->req[a]->iotd_Req.io_Command=TD_CHANGENUM;
			BeginIO((struct IORequest *)data->req[a]);

			// Has change count changed?
			if (data->count[a]!=data->req[a]->iotd_Req.io_Actual)
			{
				// Set change flag
				change|=(1<<a);

				// Store change count
				data->count[a]=data->req[a]->iotd_Req.io_Actual;
			}

			// See if there's a disk in the drive
			data->req[a]->iotd_Req.io_Flags=IOF_QUICK;
			data->req[a]->iotd_Req.io_Command=TD_CHANGESTATE;
			BeginIO((struct IORequest *)data->req[a]);

			// If there is, set flag
			if (data->req[a]->iotd_Req.io_Actual==0)
				data->flags|=(1<<a);

			// Otherwise clear flag
			else data->flags&=~(1<<a);
		}
	}

	return change;
}
