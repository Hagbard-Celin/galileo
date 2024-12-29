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

// PARENT, ROOT
GALILEOFM_FUNC(function_parent)
{
	PathNode *path;
	short ret=0,a;

	// Get current source lister
	if (!(path=function_path_current(&handle->source_paths)) ||
		!path->pn_lister) return 0;

	// Get current path
	strcpy(handle->inst_data,path->pn_path);

	// Try twice
	for (a=0;a<2;a++)
	{
		// Do parent/root
		if (command->function==FUNC_PARENT) ret=path_parent(handle->inst_data);
		else ret=path_root(handle->inst_data);

		// Successful?
		if (ret) break;

		// For second time through, expand path
		if (a==0)
		{
			BPTR lock;

			// Lock path
			if (lock=Lock(handle->inst_data,ACCESS_READ))
			{
				// Expand path
				DevNameFromLock(lock,handle->inst_data,512);
				UnLock(lock);
			}
		}
	}

	// Valid directory?
	if (ret)
	{
		// Read directory
		handle->flags=GETDIRF_CANCHECKBUFS|GETDIRF_CANMOVEEMPTY;
		function_read_directory(handle,path->pn_lister,handle->inst_data);
	}

	return 1;
}
