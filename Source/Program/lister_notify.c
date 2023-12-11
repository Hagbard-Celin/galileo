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

// Handle a notify message
void lister_handle_notify(Lister *lister,GalileoNotify *notify,char *name)
{
	BOOL show=0;

	// Lock buffer
	buffer_lock(lister->cur_buffer,TRUE);

	// Create dir?
	if (notify->gn_Flags&GNF_DOS_CREATEDIR)
	{
		DirEntry *entry;
		BPTR lock;
		struct FileInfoBlock __aligned fib;

		// Lock directory
		if (lock=Lock(name,ACCESS_READ))
		{
			// Examine it
			Examine(lock,&fib);
			UnLock(lock);

			// Create entry
			if (entry=create_file_entry(
				lister->cur_buffer,0,
				fib.fib_FileName,
				fib.fib_Size,
				fib.fib_DirEntryType,
				&fib.fib_Date,
				fib.fib_Comment,
				fib.fib_Protection,
				0,0,0,0))
			{
				// Add to buffer
				add_file_entry(lister->cur_buffer,entry,0);

				// Save date in buffer (assume this is the latest thing!)
				lister->cur_buffer->buf_DirectoryDate=fib.fib_Date;

				// Mark for refresh
				show=1;
			}
		}
	}

	// Unlock buffer
	buffer_unlock(lister->cur_buffer);

	// Refresh?
	if (show) lister_refresh_display(lister,REFRESHF_SLIDERS|REFRESHF_STATUS);
}
