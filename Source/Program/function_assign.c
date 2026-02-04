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
#include <gfm/paths_protos.h>
#include "lister.h"
#include "function_launch_protos.h"
#include "function_protos.h"
#include "misc_protos.h"
#include "lsprintf_protos.h"

GALILEOFM_FUNC(function_assign)
{
	BPTR lock;
	ULONG pathlen = 0;
	char *ptr, *pathname,name[35];
	short suc=0;

	// Valid path?
	if (!handle->func_source_lock) return 0;

	if (handle->flags&FUNCF_DRAG_DROP)
	    lock = handle->func_source_lock;
	else
	    lock = DupLock(handle->func_source_lock);

	// Get path
	if (pathname = PathFromLock(handle->memory,  handle->func_source_lock, PFLF_USE_DEVICENAME, NULL))
	{
	    pathlen = getreg(REG_D1);
	    // Get pointer to object name
	    if (!(ptr=FilePart(pathname)) || !*ptr)
		    ptr=pathname;

	    // Get truncated file name
	    get_trunc_filename(ptr,name);

	    // Loop while unsuccessful
	    while (!suc)
	    {
		    short ret;

		    // Build requester text
		    lsprintf(
			    handle->func_work_buf,
			    GetString(&locale,MSG_ENTER_ASSIGN_NAME),
			    name);

		    if (pathlen > 255)
		    {
		        // Ask for name
		        if (!(ret=function_request(
			        handle,
			        handle->func_work_buf,
			        SRF_BUFFER|SRF_PATH_FILTER,
			        handle->func_work_buf+512,30,
			        GetString(&locale,MSG_ASSIGN),
			        GetString(&locale,MSG_ADD),
				GetString(&locale,MSG_ABORT),0)) || !*(handle->func_work_buf+512))
			{
			    UnLock(lock);
			    return 0;
			}
		    }
		    else
		    {
				// Ask for name
				if (!(ret=function_request(
					handle,
					handle->func_work_buf,
					SRF_BUFFER|SRF_PATH_FILTER,
					handle->func_work_buf+512,30,
					GetString(&locale,MSG_ASSIGN),
					GetString(&locale,MSG_ADD),
					GetString(&locale,MSG_PATH),
					GetString(&locale,MSG_DEFER),
					GetString(&locale,MSG_ABORT),0)) || !*(handle->func_work_buf+512))
			{
			    UnLock(lock);
			    return 0;
			}
		    }

		    // Path assign?
		    if (ret==3) suc=AssignPath(handle->func_work_buf+512,pathname);

		    // Late assign?
		    else
		    if (ret==4) suc=AssignLate(handle->func_work_buf+512,pathname);

		    // Try to lock path
		    else
		    if (lock)
		    {
			    // Add?
			    if (ret==2) suc=AssignAdd(handle->func_work_buf+512,lock);

			    // Normal assign
			    else suc=AssignLock(handle->func_work_buf+512,lock);

			    // Successful?
			    if (suc) lock=0;
		    }

		    // Error?
		    if (!suc)
		    {
			    // Show error
			    suc=!(function_error(handle,handle->func_work_buf+512,MSG_ASSIGNING,-IoErr()));
		    }
			else
			{
				IPCData *ipc;
				Lister *lister;

				// Lock lister list
				lock_listlock(&GUI->lister_list,FALSE);

				// Go through listers
				for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
					ipc->node.mln_Succ;
					ipc=(IPCData *)ipc->node.mln_Succ)
				{
					// Get lister
					lister=IPCDATA(ipc);
					// Showing devicelist?
					if (lister->flags&LISTERF_DEVICE_LIST &&
						lister->cur_buffer &&
						lister->cur_buffer->more_flags&DWF_DEVICE_LIST)
					{
							// Activate this window
							IPC_Command(ipc,LISTER_RESCAN,0,0,0,0);
					}
				}

				// Unlock lister list
				unlock_listlock(&GUI->lister_list);
			}
	    }

	    // Free pathname
	    FreeMemH(pathname);
	}
	// Lock to free?
	if (lock) UnLock(lock);

	return 0;
}
