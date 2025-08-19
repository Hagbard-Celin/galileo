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
#include "function_launch_protos.h"
#include "function_protos.h"
#include "backdrop_protos.h"
#include "icons.h"

// Leave some objects out
GALILEOFM_FUNC(function_leaveout)
{
	PathNode *path;
	FunctionEntry *entry;
	short ret=1;
	short count=0;

	// Get current path
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BOOL ok=0;
		BPTR source_lock;

		// Check for abort
		if (function_check_abort(handle))
		{
			ret=0;
			break;
		}

		// Get source dir lock
		if (entry->fe_lock)
		    source_lock = entry->fe_lock;
		else
		    source_lock	= path->pn_lock;

		// Ignore if this is an icon
		if (!(isicon(entry->fe_name)))
		{
			// Leave object out
			if (backdrop_leave_out(
				GUI->backdrop,
				entry->fe_name,
				source_lock,
				BLOF_PERMANENT|BLOF_REFRESH,-1,-1))
			{
				ok=1;
				++count;
			}
		}

		// Get next entry
		function_end_entry(handle,entry,ok);
	}

	// Update leftout list
	if (count) backdrop_save_leftouts(GUI->backdrop);
	return ret;
}
