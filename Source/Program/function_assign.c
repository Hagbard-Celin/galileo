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

#include "galileofm.h"

GALILEOFM_FUNC(function_assign)
{
	char *ptr,name[35];
	short suc=0,len;

	// Valid path?
	if (!*handle->func_source_path) return 0;

	// Strip trailing /
	if (handle->func_source_path[(len=strlen(handle->func_source_path)-1)]=='/')
		handle->func_source_path[len]=0;

	// Get pointer to object name
	if (!(ptr=FilePart(handle->func_source_path)) || !*ptr)
		ptr=handle->func_source_path;

	// Get truncated file name
	get_trunc_filename(ptr,name);

	// Loop while unsuccessful
	while (!suc)
	{
		short ret;
		BPTR lock=0;

		// Build requester text
		lsprintf(
			handle->func_work_buf,
			GetString(&locale,MSG_ENTER_ASSIGN_NAME),
			name);

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
			GetString(&locale,MSG_ABORT),0)) || !*(handle->func_work_buf+512)) return 0;

		// Path assign?
		if (ret==3) suc=AssignPath(handle->func_work_buf+512,handle->func_source_path);

		// Late assign?
		else
		if (ret==4) suc=AssignLate(handle->func_work_buf+512,handle->func_source_path);

		// Try to lock path
		else
		if (lock=Lock(handle->func_source_path,ACCESS_READ))
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

		// Lock to free?
		if (lock) UnLock(lock);
	}

	return 0;
}
