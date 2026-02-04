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
#include <gfm/icon_cache_protos.h>
#include <gfm/config_display.h>
#include "lister_ipc.h"
#include "misc_protos.h"
#include "menu_data.h"
#include "backdrop_protos.h"

// Got a notify message from PutDiskObject()
BOOL backdrop_check_notify(
	BackdropInfo *info,
	GalileoNotify *notify,
	Lister *lister)
{
	char *name_buf;
	BOOL disk=0,ret=0, validlister = FALSE;
	struct List *search;
	BackdropObject *object;

	if (!(name_buf=AllocVec(256,0)))
		return 0;

	if (lister && lister->cur_buffer && lister->cur_buffer->buf_Lock)
	    validlister = TRUE;

	// Disk icon?
	if (notify->gn_Name[strlen(notify->gn_Name)-1]==':')
	{
		char *ptr;

		// Get volume name
		if (ptr=strchr(notify->gn_Name,':'))
		{
			stccpy(name_buf,notify->gn_Name,ptr-notify->gn_Name+1);
			disk=1;
		}
	}

	// Otherwise copy name and clear it
	else
	{
		// Get name pointer
		char *name=FilePart(notify->gn_Name);

		// Copy name
		strcpy(name_buf,name);
		*name=0;

		// Strip trailing '/'
		if (*(name-1)=='/') *(name-1)=0;
	}

	// Is this a lister?
	if (lister)
	{
		BPTR lock;
		BOOL ok=0;

		if (validlister && (lock = LockFromPath(notify->gn_Name, NULL, FALSE)))
		{
		    if (SameLock(lock, lister->cur_buffer->buf_Lock) == LOCK_SAME)
			ok = 1;

		    UnLock(lock);
		}

		// Didn't match?
		if (!ok)
		{
			// Free message
			ReplyFreeMsg(notify);	
			FreeVec(name_buf);
			return 0;
		}
	}

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// See if there's an icon of this name
	search=&info->objects.list;
	while (object=(BackdropObject *)FindNameI(search,name_buf))
	{
		// Disk?
		if (object->bdo_type==BDO_DISK && disk)
		{
			// Matched
			break;
		}

		// Lister?
		else
		if (validlister)
		    break;

		// Valid object?
		else
		if (object->bdo_type!=BDO_APP_ICON &&
			object->bdo_type!=BDO_BAD_DISK &&
			object->bdo_path)
		{
			BPTR lock1, lock2;

			if (lock1 = LockFromPath(notify->gn_Name, NULL, FALSE))
			{
			    if (lock2 = LockFromPath(object->bdo_path, NULL, FALSE))
			    {
				BOOL br = FALSE;

				if (SameLock(lock1, lock2) == LOCK_SAME)
				    br = TRUE;

				UnLock(lock1);
				UnLock(lock2);

				if (br)
				    break;
			    }
			    else
				UnLock(lock1);
			}
		}

		// If this is a lister, there could only be one
		if (lister)
		{
			object=0;
			break;
		}

		// Keep searching from this object
		search=(struct List *)object;
	}

	// Got object?
	if (object)
	{
		ULONG old_image1=0,old_image2=0,new_image1,new_image2,old_flags=0,new_flags;
		BOOL redraw=0;
		struct DiskObject *old;

		// Save old icon
		old=object->bdo_icon;
		object->bdo_icon=0;

		// Not deleted?
		if (!notify->gn_Flags)
		{
			// Get image checksums
			old_image1=IconCheckSum(old,0);
			old_image2=IconCheckSum(old,1);
			old_flags=GetIconFlags(old);

			// Get new icon
			backdrop_get_icon(info,object,GETICON_CD|GETICON_NO_POS|GETICON_FAIL);
		}

		// No icon now?
		if (!object->bdo_icon)
		{
			// Replace old icon
			object->bdo_icon=old;

			// Erase old object
			backdrop_erase_icon(info,object,BDSF_RECALC);

			// Is object a disk?
			if (object->bdo_type==BDO_DISK)
			{
				// Signal to refresh drives
				IPC_Command(info->ipc,MAINCMD_REFRESH_DRIVES,0,0,0,0);
			}

			// Remove object from list
			backdrop_remove_object(info,object);
		}

		// Ok to keep going
		else
		{
			// Get image checksums
			new_image1=IconCheckSum(object->bdo_icon,0);
			new_image2=IconCheckSum(object->bdo_icon,1);
			new_flags=GetIconFlags(object->bdo_icon);

			// Mask out uninteresting flag bits
			old_flags&=ICONF_BORDER_OFF|ICONF_BORDER_ON|ICONF_NO_LABEL;
			new_flags&=ICONF_BORDER_OFF|ICONF_BORDER_ON|ICONF_NO_LABEL;

			// Need to redraw?
			if (old_image1!=new_image1 ||
				old_image2!=new_image2 ||
				old_flags!=new_flags)
			{
				// Erase old object
				backdrop_erase_icon(info,object,0);
				redraw=1;
			}

			// Free old icon
			if (old)
			{
				// Free icon remapping
				RemapIcon(old,(info->window)?info->window->WScreen:0,1);

				// Free icon
				FreeCachedDiskObject(old);
			}

			// Fix new icon size
			backdrop_get_icon(info,object,GETICON_POS_ONLY|GETICON_SAVE_POS|GETICON_KEEP);

			// Need to get masks?
			if (!backdrop_icon_border(object))
			{
				// Get masks for this icon
				backdrop_get_masks(object);
			}
			
			// Show new icon
			if (redraw) backdrop_render_object(info,object,BRENDERF_CLIP);
		}

		ret=1;
	}

	// Otherwise, got lister?
	else
	if (lister)
	{
		// Tell lister to get icons
		IPC_Command(lister->ipc,LISTER_GET_ICONS,0,0,0,0);
		ret=1;
	}

	// Or, on the desktop?
	else
	if (info->flags&BDIF_MAIN_DESKTOP)
	{
		BPTR lock;

		// Lock the changed directory
		if (lock=Lock(notify->gn_Name,ACCESS_READ))
		{
			// Same directory?
			if (SameLock(lock,GUI->desktop_dir_lock)==LOCK_SAME)
			{
				// Update the desktop folder
				misc_startup("galileo_desktop_update",MENU_UPDATE_DESKTOP,GUI->window,0,TRUE);
			}

			// Unlock lock
			UnLock(lock);
		}
	}

	// Unlock list
	unlock_listlock(&info->objects);

	// Free message
	ReplyFreeMsg(notify);	
	FreeVec(name_buf);
	return ret;
}
