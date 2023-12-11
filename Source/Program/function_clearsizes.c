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

// CLEARSIZES internal function
GALILEOFM_FUNC(function_clearsizes)
{
	FunctionEntry *entry;
	Lister *lister;
	DirBuffer *buffer=0;

	// Get directories
	handle->instruction_flags=INSTF_WANT_DIRS;

	// Get current lister
	if (lister=function_lister_current(&handle->source_paths))
		buffer=lister->cur_buffer;

	// Go through files
	while (entry=function_get_entry(handle))
	{
		// Directory?
		if (entry->type>ENTRY_DIRECTORY &&
			entry->entry &&
			!(entry->flags&FUNCENTF_ICON_ACTION) &&
			entry->entry->size>0)
		{
			// Got a buffer?
			if (buffer)
			{
				// Remove from buffer counts
				buffer->buf_TotalBytes[0]-=entry->entry->size;

				// Selected?
				if (entry->entry->flags&ENTF_SELECTED)
					buffer->buf_SelectedBytes[0]-=entry->entry->size;
			}

			// Clear size
			entry->entry->size=-1;
		}

		// Get next entry
		function_end_entry(handle,entry,0);
	}

	return 1;
}
