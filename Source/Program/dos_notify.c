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
#include <gfm/strings_protos.h>
#include "function_launch_protos.h"
#include "desktop.h"

// Handle notification of DOS events
void handle_dos_notify(GalileoNotify *notify,FunctionHandle *handle)
{
	// Not DOS action?
	if (!notify || !(notify->gn_Type&GN_DOS_ACTION))
		return;

	// CreateDir?
	if (notify->gn_Flags&GNF_DOS_CREATEDIR)
	{
		// Got valid FileInfoBlock?
		if (notify->gn_Fib)
		{
			// Add change to add file
			function_filechange_addfile(
				handle,
				notify->gn_Name,
				NULL,
				notify->gn_Fib,
				0,0);
		}
	}

	// Delete file?
	else
	if (notify->gn_Flags&GNF_DOS_DELETEFILE)
	{
		char *ptr, tmp = 0;

		// Get pointer to filename
		if (ptr = FilePart(notify->gn_Name))
		{
			// Copy to separate buffer and break connection
			strcpy(handle->func_work_buf, ptr);
			tmp = ptr[0];
			*ptr=0;
		}

		// Got valid file?
		if (ptr)
		{
			// Add change to remove file
			function_filechange_delfile(handle,
				                    notify->gn_Name,
				                    NULL,
				                    handle->func_work_buf,
						    0, 1);

			ptr[0] = tmp;
		}
	}

	// Modify?
	else
	if (notify->gn_Flags&(	GNF_DOS_SETFILEDATE|
							GNF_DOS_SETCOMMENT|
							GNF_DOS_SETPROTECTION|
							GNF_DOS_RENAME|
							GNF_DOS_CREATE|
							GNF_DOS_CLOSE))
	{
		// Got valid FileInfoBlock?
		if (notify->gn_Fib)
		{
			struct TagItem tag;

			// Initialise tag
			tag.ti_Tag=TAG_END;
			tag.ti_Data=0;

			// Create?
			if (notify->gn_Flags&GNF_DOS_CREATE)
			{
				// Add change to add file
				function_filechange_addfile(
					handle,
					notify->gn_Name,
					NULL,
					notify->gn_Fib,
					0,0);
			}

			// SetFileDate
			else
			if (notify->gn_Flags&GNF_DOS_SETFILEDATE)
			{
				// Fill out tag
				tag.ti_Tag=FM_Date;
				tag.ti_Data=(ULONG)&notify->gn_Fib->fib_Date;
			}

			// SetComment
			else
			if (notify->gn_Flags&GNF_DOS_SETCOMMENT)
			{
				// Fill out tag
				tag.ti_Tag=FM_Comment;
				tag.ti_Data=(ULONG)notify->gn_Fib->fib_Comment;
			}

			// SetProtection
			else
			if (notify->gn_Flags&GNF_DOS_SETPROTECTION)
			{
				// Fill out tag
				tag.ti_Tag=FM_Protect;
				tag.ti_Data=(ULONG)notify->gn_Fib->fib_Protection;
			}

			// Rename
			else
			if (notify->gn_Flags&GNF_DOS_RENAME)
			{
				// Fill out tag
				tag.ti_Tag=FM_Name;
				tag.ti_Data=(ULONG)notify->gn_Fib->fib_Comment;

				// Is this a directory?
				if (notify->gn_Fib->fib_DirEntryType>0)
				{
					char *oldpath, *newpath;

					if (oldpath = JoinString(handle->memory,
								 notify->gn_Name,
								 notify->gn_Fib->fib_FileName,
								 NULL,
								 JSF_FS_SLASH|JSF_E_SLASH))
					{
					    if (newpath = JoinString(handle->memory,
								     notify->gn_Name,
								     notify->gn_Fib->fib_Comment,
								     NULL,
								     JSF_FS_SLASH|JSF_E_SLASH))
					    {
						// Add directory rename
						function_filechange_rename(handle,
							                   oldpath,
							                   NULL,
							                   newpath);

						FreeMemH(newpath);
					    }

					    FreeMemH(oldpath);
					}
				}
			}

			// Size (ie Close)
			else
			if (notify->gn_Flags&GNF_DOS_CLOSE)
			{
				// Fill out tag
				tag.ti_Tag=FM_Size;
				tag.ti_Data=(ULONG)notify->gn_Fib->fib_Size;
			}

			// Valid tag?
			if (tag.ti_Tag!=TAG_END)
			{
				// Add change to modify file
				function_filechange_modify(
					handle,
					notify->gn_Name,
					NULL,
					notify->gn_Fib->fib_FileName,
					tag.ti_Tag,tag.ti_Data,
					TAG_END);
			}
		}
	}

	// Relabel?
	else
	if (notify->gn_Flags&GNF_DOS_RELABEL)
	{
		// Pass name change through to things
		notify_disk_name_change(0,notify->gn_Name,notify->gn_Fib->fib_FileName);
	}
}
