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

short read_desktop_folder(BackdropInfo *info,BOOL show)
{
	BPTR cdlock;
	BackdropObject *icon;
	short count=0;

	// Lock icon list
	lock_listlock(&info->objects,TRUE);

	// Stop notification on the desktop
	stop_file_notify(GUI->desktop_notify);
	GUI->desktop_notify=0;

	// Go through icons, mark desktop folder objects as invalid
	for (icon=(BackdropObject *)info->objects.list.lh_Head;
		icon->node.ln_Succ;
		icon=(BackdropObject *)icon->node.ln_Succ)
	{
		// Desktop folder object?
		if (icon->type==BDO_LEFT_OUT &&
			icon->flags&BDOF_DESKTOP_FOLDER)
		{
			// Mark as not ok
			icon->flags&=~BDOF_OK;
		}
	}

	// Lock desktop folder
	if (cdlock=Lock(environment->env->desktop_location,ACCESS_READ))
	{
		BPTR old,lock;
		struct FileInfoBlock __aligned fib;
	
		// Copy lock to examine directory
		lock=DupLock(cdlock);

		// Change directories
		old=CurrentDir(cdlock);

		// Examine directory lock
		Examine(lock,&fib);

		// Go through it
		while (ExNext(lock,&fib))
		{
			struct List *search;
			char *ptr;

			// An icon?
			if (ptr=sufcmp(fib.fib_FileName,".info"))
			{
				BPTR test;
	
				// Strip the icon suffix
				*ptr=0;

				// See if file exists
				if (test=Lock(fib.fib_FileName,ACCESS_READ))
				{
					// We won't use the icon, since there's a file to use instead
					UnLock(test);

					// Continue with directory
					continue;
				}
			}

			// See if we already have this one
			search=&info->objects.list;
			while (icon=(BackdropObject *)FindNameI(search,fib.fib_FileName))
			{
				// Match?
				if (icon->type==BDO_LEFT_OUT &&
					icon->flags&BDOF_DESKTOP_FOLDER)
				{
					// Mark as ok
					icon->flags|=BDOF_OK;
					break;
				}

				// Keep searching from this icon
				search=(struct List *)icon;
			}

			// Didn't have it?
			if (!icon)
			{
				// Create new icon
				if (icon=backdrop_leftout_new(info,fib.fib_FileName,environment->env->desktop_location,0))
				{
					// Set 'desktop folder' flag, and mark as ok
					icon->flags|=BDOF_DESKTOP_FOLDER;

					// Link?
					if (fib.fib_DirEntryType==ST_SOFTLINK ||
						fib.fib_DirEntryType==ST_LINKDIR ||
						fib.fib_DirEntryType==ST_LINKFILE)
					{
						// Set flag
						icon->flags|=BDOF_LINK_ICON;
					}

					// Get icon
					backdrop_get_icon(info,icon,GETICON_CD);

					// Set ok flag if we have an icon
					if (icon->icon) icon->flags|=BDOF_OK;

					// Increment count
					count++;

					// Position icon if showing
					if (show) backdrop_place_object(info,icon);
				}
			}
		}

		// Unlock folder
		UnLock(lock);

		// Restore CD
		UnLock(CurrentDir(old));
	}

	// Go through icons, remove old ones
	for (icon=(BackdropObject *)info->objects.list.lh_Head;
		icon->node.ln_Succ;)
	{
		// Cache next icon
		BackdropObject *next=(BackdropObject *)icon->node.ln_Succ;

		// Desktop folder object?
		if (icon->type==BDO_LEFT_OUT &&
			icon->flags&BDOF_DESKTOP_FOLDER)
		{
			// Not marked as ok?
			if (!(icon->flags&BDOF_OK))
			{
				// Erase icon from display
				if (icon->icon && show)
					backdrop_erase_icon(info,icon,0);

				// Remove and free icon
				backdrop_remove_object(info,icon);
			}
		}

		// Get next
		icon=next;
	}

	// Show icons?
	if (show && count>0) backdrop_show_objects(info,BDSF_RECALC);

	// Restart notification on the desktop
	GUI->desktop_notify=start_file_notify(environment->env->desktop_location,NOTIFY_DESKTOP_CHANGED,GUI->appmsg_port);

	// Unlock icon list
	unlock_listlock(&info->objects);

	return count;
}


