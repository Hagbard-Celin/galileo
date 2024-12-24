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

// Show info on backdrop objects
void backdrop_info(BackdropInfo *info,BackdropObject *icon)
{
	BackdropObject *object;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->node.ln_Succ;
		object=(BackdropObject *)object->node.ln_Succ)
	{
		// Is object selected, or one supplied?
		if ((!icon && object->state) ||
			(icon==object))
		{
			// Bad disk?
			if (object->type==BDO_BAD_DISK)
				DisplayBeep(info->window->WScreen);

			// AppIcon?
			else
			if (object->type==BDO_APP_ICON)
			{
				// Does it support Info?
				if ((WB_AppIconFlags((struct AppIcon *)object->misc_data))&APPENTF_INFO)
				{
					// Send info message
					backdrop_appicon_message(object,BAPPF_INFO);
				}

				// Fail
				else DisplayBeep(info->window->WScreen);
			}

			// Other
			else
			{
				BPTR old,lock;

				// Get icon lock
				if (lock=backdrop_icon_lock(object))
				{
					// Change directory
					old=CurrentDir(lock);

					// Show info
					misc_startup(
						"galileo_icon_info",
						MENU_ICON_INFO,
						info->window,
						(object->type==BDO_DISK)?":":object->name,
						0);

					// Restore directory
					CurrentDir(old);
					UnLock(lock);
				}
			}

			// Icon supplied?
			if (icon) break;
		}
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}
