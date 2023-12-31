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

// Open a disk for IO
DiskHandle *__asm __saveds L_OpenDisk(
	register __a0 char *disk,
	register __a1 struct MsgPort *port)
{
	DiskHandle *handle;
	struct DosList *dl;
	unsigned long blocks;
	char *ptr;

	// Allocate handle
	if (!(handle=AllocVec(sizeof(DiskHandle),MEMF_CLEAR)))
		return 0;

	// Copy name, strip colon
	stccpy(handle->dh_name,disk,31);
	if (ptr=strchr(handle->dh_name,':')) *ptr=0;

	// Lock DOS list
	dl=LockDosList(LDF_DEVICES|LDF_READ);

	// Find entry
	if (!(dl=FindDosEntry(dl,handle->dh_name,LDF_DEVICES)))
	{
		// Not found
		UnLockDosList(LDF_DEVICES|LDF_READ);
		FreeVec(handle);
		return 0;
	}

	// Add colon back on
	strcat(handle->dh_name,":");

	// Get pointer to startup message and geometry
	handle->dh_startup=(struct FileSysStartupMsg *)BADDR(dl->dol_misc.dol_handler.dol_Startup);
	handle->dh_geo=(struct DosEnvec *)BADDR(handle->dh_startup->fssm_Environ);

	// Shortcuts to root block and blocksize
	blocks=handle->dh_geo->de_BlocksPerTrack*
			handle->dh_geo->de_Surfaces*
			(handle->dh_geo->de_HighCyl-handle->dh_geo->de_LowCyl+1);
	handle->dh_root=(blocks-1+handle->dh_geo->de_Reserved)>>1;
	handle->dh_blocksize=handle->dh_geo->de_SizeBlock<<2;

	// Get real device name
	L_BtoCStr((BPTR)handle->dh_startup->fssm_Device,handle->dh_device,32);

	// Get information
	handle->dh_result=DoPkt(dl->dol_Task,ACTION_DISK_INFO,MKBADDR(&handle->dh_info),0,0,0,0);

	// Unlock dos list
	UnLockDosList(LDF_DEVICES|LDF_READ);

	// Create message port if needed
	if (!port)
	{
		if (!(handle->dh_port=CreateMsgPort()))
		{
			FreeVec(handle);
			return 0;
		}
		port=handle->dh_port;
	}

	// Create IO request
	if (!(handle->dh_io=(struct IOExtTD *)CreateIORequest(port,sizeof(struct IOExtTD))))
	{
		if (handle->dh_port) DeleteMsgPort(handle->dh_port);
		FreeVec(handle);
		return 0;
	}

	// Open device
	if (OpenDevice(
		handle->dh_device,
		handle->dh_startup->fssm_Unit,
		(struct IORequest *)handle->dh_io,
		handle->dh_startup->fssm_Flags))
	{
		// Failed to open
		L_CloseDisk(handle);
		return 0;
	}

	return handle;
}


// Close a disk
void __asm __saveds L_CloseDisk(register __a0 DiskHandle *handle)
{
	if (handle)
	{
		// IO request?
		if (handle->dh_io)
		{
			// Device open?
			if (handle->dh_io->iotd_Req.io_Device)
				CloseDevice((struct IORequest *)handle->dh_io);

			// Delete IO request
			DeleteIORequest((struct IORequest *)handle->dh_io);

			// Delete message port
			if (handle->dh_port) DeleteMsgPort(handle->dh_port);
		}

		// Free handle
		FreeVec(handle);
	}
}
