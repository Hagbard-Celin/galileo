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
#include "app_msg_protos.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "backdrop_protos.h"

// Format disks
void backdrop_format(BackdropInfo *info,BackdropObject *icon)
{
	BackdropObject *object;
	short count=0;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Disk?
		if (object->bdo_type!=BDO_DISK && object->bdo_type!=BDO_BAD_DISK)
			continue;

		// Selected or supplied?
		if ((!icon && object->bdo_state) || icon==object)
		{
			// Got a device name?
			if (object->bdo_device_name)
			{
				// Launch format
				function_launch(
					FUNCTION_RUN_FUNCTION,
					def_function_format,
					0,
					0,
					0,0,
					0,0,
					0,0,
					BuildArgArray(object->bdo_device_name,0),0,0);

				// Increment count
				++count;
			}

			// Icon supplied?
			if (icon) break;
		}
	}

	// Unlock backdrop
	unlock_listlock(&info->objects);

	// If we didn't launch any formats, run the format program
	if (count==0)
	{
		// Launch format
		function_launch_quick(FUNCTION_RUN_FUNCTION,def_function_format,0);
	}
}
