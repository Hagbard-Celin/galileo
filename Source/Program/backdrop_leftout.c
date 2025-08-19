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
#include "lsprintf_protos.h"
#include "misc_protos.h"
#include "backdrop_protos.h"
#include "icons.h"
#include "position_protos.h"

#define is_digit(c) ((c)>='0' && (c)<='9')


// Get a new left-out object
BackdropObject *backdrop_leftout_new(
	BackdropInfo *info,
	char *name,
	char *parent_dir,
	char *custom_label)
{
	BackdropObject *newob;
	BOOL validlister = FALSE;

	if (info->lister && info->lister->cur_buffer && info->lister->cur_buffer->buf_Lock)
	    validlister = TRUE;


	if (!validlister && (!parent_dir || !parent_dir[0]))
	{
	    BPTR lock;

	    if (!(lock = LockFromPath(name, NULL, FALSE)))
	    {
		// Couldn't lock object.. see if it's "Disk"
		if (stricmp(FilePart(name),"disk")==0)
		    return 0;

		// Not a disk.info, try for .info file
		if (!(lock = LockFromPath(name, NULL, TRUE)))
		    return 0;
	        else
		    UnLock(lock);
	    }
	    else
		UnLock(lock);
	}

	// Allocate a new object
	if (newob=AllocMemH(info->memory,sizeof(BackdropObject)))
	{
		char filelen;
		ULONG len;

		// Fill out object
		newob->bdo_type=BDO_LEFT_OUT;
		newob->bdo_flags=BDOF_NO_POSITION;

		if (validlister)
		    newob->bdo_parent_lock = info->lister->cur_buffer->buf_Lock;

		if (name && (len = strlen(name)))
		{
		    char filename[108];

		    // Copy filename and get length including terminating zero
		    filelen = stccpy(filename, FilePart(name), 108);

		    // Allocate name
		    if (!(newob->bdo_name = AllocMemH(info->memory, filelen)))
		    {
			FreeMemH(newob);
			return 0;
		    }

		    // Copy name
		    newob->bdo_node.ln_Name = newob->bdo_name;
		    strcpy(newob->bdo_name, filename);

		    if(!validlister)
		    {
			ULONG pathlen;

		        if (!parent_dir || !parent_dir[0])
		        {
		            // Get path length including terminating zero
			    pathlen = len - filelen + 2;

		            // Allocate parent dir
			    if (!pathlen || !(newob->bdo_path = AllocMemH(info->memory, pathlen)))
			        goto leftout_fail;

		            // Copy parent dir part of filename
		            stccpy(newob->bdo_path, name, pathlen);

			    pathlen--;
		        }
		        else
		        {
		            len = strlen(parent_dir);

		            // Allocate parent dir
		            if (!(newob->bdo_path = AllocMemH(info->memory, len + 1)))
			        goto leftout_fail;

		            // Copy parent dir
	                    strcpy(newob->bdo_path, parent_dir);

			    pathlen = len;
		        }

		        if (custom_label && (len = strlen(custom_label)))
		        {
			    char *path_ptr;

			    if ((path_ptr = strchr(newob->bdo_path, ':') + 1) == newob->bdo_path + pathlen)
				path_ptr = newob->bdo_path;

		            // Get lock on parent dir
			    if (!path_ptr || !(newob->bdo_parent_lock = LockFromPath(path_ptr, NULL, FALSE)))
		                goto leftout_fail;

		            // Allocate custom label
		            if (!(newob->bdo_device_name = AllocMemH(info->memory, len + 1)))
			        goto leftout_fail;

		            // Copy custom label
		            strcpy(newob->bdo_device_name, custom_label);

		            // Set flag
		            newob->bdo_flags|=BDOF_CUSTOM_LABEL;
		        }
		    }

		    // Add to backdrop list
		    AddTail(&info->objects.list,&newob->bdo_node);
		}
		else
		    FreeMemH(newob);
	}

	return newob;

leftout_fail:
	if (newob->bdo_name)
	    FreeMemH(newob->bdo_name);

	if (newob->bdo_path)
	    FreeMemH(newob->bdo_path);

	if (!validlister &&  newob->bdo_parent_lock)
	    UnLock(newob->bdo_parent_lock);

	FreeMemH(newob);
	return 0;
}


// Leave things out
void backdrop_leave_icons_out(BackdropInfo *info,BackdropObject *only_one,BOOL shortcut)
{
	BackdropObject *object=0;
	BOOL save=0;

	// Lock icon list
	lock_listlock(&info->objects,0);

	// Go through selected icons
	while (object=backdrop_next_object(info,object,only_one))
	{
		// Create shortcut?
		if (shortcut)
		{
			BPTR dir;
			char path[256];

			// Get icon lock
			if (dir=backdrop_icon_lock(object))
			{
				// Get icon path name
				DevNameFromLock(dir,path,256);
				AddPart(path,object->bdo_name,256);

				// New shortcut
				backdrop_create_shortcut(GUI->backdrop,path,-1,-1);

				// Unlock lock
				UnLock(dir);
			}
		}

		// Left-out object?
		else
		if (object->bdo_type==BDO_LEFT_OUT)
		{
			// Temporary leave-out?
			if (object->bdo_flags&BDOF_TEMP_LEFTOUT)
			{
				// Clear temporary flag
				object->bdo_flags&=~BDOF_TEMP_LEFTOUT;

				// Set flag to save leftouts
				save=1;
			}

			// Otherwise, only leave out if not already on main window
			else
			if (info!=GUI->backdrop)
			{
				BPTR dir;

				// Get icon lock
				if (dir=backdrop_icon_lock(object))
				{
					// New left-out
					if (backdrop_leave_out(
						GUI->backdrop,
						object->bdo_name,
						dir,
						BLOF_PERMANENT|BLOF_REFRESH,-1,-1)) save=1;

					// Unlock lock
					UnLock(dir);
				}
			}
		}
	}

	// Unlock list
	unlock_listlock(&info->objects);

	// Need to save?
	if (save) backdrop_save_leftouts(GUI->backdrop);
}


// Leave out something
BackdropObject *backdrop_leave_out(
	BackdropInfo *info,
	char *name,
	BPTR parent_lock,
	ULONG flags,
	short x,short y)
{
	BPTR lock, org_dir;
	struct FileInfoBlock __aligned fib;
	BackdropObject *object;
	struct List *search;
	char *path = 0;

	if (!parent_lock)
	    return 0;

	org_dir = CurrentDir(parent_lock);

	// Lock object
	if (!(lock=Lock(name,ACCESS_READ)))
	{
		// Try for icon
		StrCombine(info->buffer,name,".info",sizeof(info->buffer));
		if (!(lock=Lock(info->buffer,ACCESS_READ)))
			return 0;

		// Isolate filename
		stccpy(fib.fib_FileName,FilePart(name),108);
	}

	// Locked object, get some info
	else Examine(lock,&fib);

	if ((SameLock(parent_lock, GUI->desktop_dir_lock)) == LOCK_SAME)
	    path = CopyString(NULL, environment->env->desktop_location);

	if (!path)
	{
		// Get parent path
		path = PathFromLock(NULL, parent_lock, PFLF_END_SLASH, NULL);
	}

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// See if object is already in list
	search=&info->objects.list;
	while (object=(BackdropObject *)FindNameI(search,fib.fib_FileName))
	{
		// See if parents are the same
		if (object->bdo_type==BDO_LEFT_OUT &&
			stricmp(path,object->bdo_path)==0) break;

		// Keep searching from this object
		search=(struct List *)object;
	}

	// Unlock object
	UnLock(lock);

	// Was object not already in list?
	if (!object)
	{
	    if (lock = Lock(":", ACCESS_READ))
	    {
		CurrentDir(lock);

		if (object=backdrop_leftout_new(info,name,path,fib.fib_FileName))
		{
			struct FileLock *fl;
			struct DosList *volume;

			// Set temporary flag if not permanent
			if (!(flags&BLOF_PERMANENT)) object->bdo_flags|=BDOF_TEMP_LEFTOUT;

			// Get filelock pointer
			fl=(struct FileLock *)BADDR(lock);

			// Get volume entry
			if (volume=(struct DosList *)BADDR(fl->fl_Volume))
			{
			    object->bdo_date = volume->dol_misc.dol_volume.dol_VolumeDate;
			    object->bdo_vol_name_len = *(UBYTE *)BADDR(volume->dol_Name);
			}

			// Position supplied?
			if (x>-1 && y>-1)
			{
				// Set position
				object->bdo_custom_pos=(x<<16)|y;
				object->bdo_flags|=BDOF_LEFTOUT_POS|BDOF_BORDER_ADJUST;
				object->bdo_flags&=~BDOF_NO_POSITION;
			}

			// Get icon
			CurrentDir(parent_lock);
			backdrop_get_icon(info,object, NULL);

			// Refresh?
			if (flags&BLOF_REFRESH)
			{
				// Arrange new icon
				backdrop_place_object(info,object);

				// Show icons
				backdrop_show_objects(info,BDSF_RECALC);
			}
		}
		else
		    CurrentDir(parent_lock);

		UnLock(lock);
	    }
	}
	else object=0;

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Restore current-dir
	CurrentDir(org_dir);

	// Free path string
	if (path)
	    FreeMemH(path);

	return object;
}


// Save .backdrop file
void backdrop_save_leftouts(BackdropInfo *info)
{
	BackdropObject *object;
	BOOL save=0;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Lock position list
	lock_listlock(&GUI->positions,TRUE);

	// Go through objects
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Is this a permanent left-out?
		if (object->bdo_type==BDO_LEFT_OUT &&
			!(object->bdo_flags&(BDOF_TEMP_LEFTOUT|BDOF_DESKTOP_FOLDER)))
		{
		    struct Node *entry;
		    struct List *search;

		    char *fullpath;

		    // Get full path of object
		    if (fullpath = JoinString(info->memory, object->bdo_path, object->bdo_name, NULL, NULL))
		    {
			// See if it's already in the list
			search=(struct List *)&GUI->positions;
			while (entry=FindNameI(search,fullpath))
			{
				// Left-out?
				if (entry->ln_Type==PTYPE_LEFTOUT) break;
				search=(struct List *)entry;
			}

			// Not found?
			if (!entry)
			{
				leftout_record *left;

				// Create new entry
				if (left=AllocMemH(GUI->position_memory,sizeof(leftout_record)+strlen(fullpath)))
				{
					// Valid icon position?
					if (!(object->bdo_flags&BDOF_NO_POSITION))
					{
						short x,y;
						ULONG iflags;

						// Get position
						x=object->bdo_pos.Left;
						y=object->bdo_pos.Top;

						// Adjust for borders?
						if (!((iflags=GetIconFlags(object->bdo_icon))&ICONF_BORDER_OFF) &&
							(!(environment->env->desktop_flags&DESKTOPF_NO_BORDERS) || (iflags&ICONF_BORDER_ON)))
						{
							// Shift back by border size
							x-=ICON_BORDER_X;
							y-=ICON_BORDER_Y_TOP;
						}

						// Store position
						left->icon_x=x;
						left->icon_y=y;
					}

					// No position
					else left->flags=LEFTOUTF_NO_POSITION;

					// Fill out name and type
					stccpy(left->icon_label,FilePart(fullpath),sizeof(left->icon_label));
					strcpy(left->name,fullpath);
					left->node.ln_Name=left->name;
					left->node.ln_Type=PTYPE_LEFTOUT;
					left->node.ln_Pri=1;

					// Get volume unique-id.
					left->vol_date = object->bdo_date;
					left->vol_name_len = object->bdo_vol_name_len;

					// Set pointer in object
					object->bdo_misc_data=(ULONG)left;

					// Add to list
					AddTail((struct List *)&GUI->positions,(struct Node *)left);
					save=1;
				}
			}

			FreeMemH(fullpath);
		    }
		}
	}

	// Save positions?
	if (save) SavePositions(&GUI->positions.list,GUI->position_name);

	// Unlock position list
	unlock_listlock(&GUI->positions);

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}


// Put objects away
void backdrop_putaway(BackdropInfo *info,BackdropObject *only_one)
{
	BackdropObject *object;
	BOOL save=0;

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// Lock position list
	lock_listlock(&GUI->positions,TRUE);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;)
	{
		BackdropObject *next=(BackdropObject *)object->bdo_node.ln_Succ;

		// Is object left out and selected?
		if (object->bdo_type==BDO_LEFT_OUT &&
			!(object->bdo_flags&BDOF_DESKTOP_FOLDER) &&
			((only_one && object==only_one) || (!only_one && object->bdo_state)))
		{
			// Erase object
			backdrop_erase_icon(info,object,0);

			// If left-out wasn't temporary, delete from list
			if (!(object->bdo_flags&BDOF_TEMP_LEFTOUT))
			{
				if (backdrop_remove_leftout(info, object))
				{
					save=1;
				}
			}

			// Remove object
			backdrop_remove_object(info,object);
		}

		// Get next
		object=next;
	}

	// Recalc virtual
	backdrop_calc_virtual(info);

	// Save leftouts?
	if (save) SavePositions(&GUI->positions.list,GUI->position_name);

	// Unlock position list
	unlock_listlock(&GUI->positions);

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}


// Remove a left-out from the list
BOOL backdrop_remove_leftout(BackdropInfo *info, BackdropObject *object)
{
	char *buf;
	struct Node *node;
	struct List *search;

	if (buf = JoinString(info->memory, object->bdo_path, object->bdo_name, NULL, NULL))
	{
	    // Look for object in list
	    search=(struct List *)&GUI->positions;
	    while (node=FindNameI(search,buf))
	    {
		// Left-out?
		if (node->ln_Type==PTYPE_LEFTOUT)
		{
			// Remove and free
			Remove(node);
			FreeMemH(node);

			// Free string
			FreeMemH(buf);

			// Return success
			return 1;
		}

		// Keep looking
		search=(struct List *)node;
	    }

	    // Free string
	    FreeMemH(buf);
	}

	// Failed
	return 0;
}


BackdropObject *backdrop_make_volume_leftout(BackdropInfo *info, leftout_record *left,  BPTR volume_lock)
{
    BPTR parent_lock, tmp_lock, org_dir = 0;
    STRPTR path_ptr;
    BackdropObject *object = 0;

    if (left->node.ln_Pri == 1)
    {
	path_ptr = left->node.ln_Name + left->vol_name_len + 1;
	org_dir = CurrentDir(volume_lock);
    }
    else
	path_ptr = left->node.ln_Name;

    if (path_ptr && (tmp_lock = LockFromPath(path_ptr, NULL, FALSE)))
    {
	parent_lock = ParentDir(tmp_lock);
	UnLock(tmp_lock);

	// Get new left-out object
	if (object=backdrop_leftout_new(info,left->node.ln_Name,0,left->icon_label))
	{
	    // Valid position?
	    if (!(left->flags&LEFTOUTF_NO_POSITION))
	    {
		    // Store position
		    object->bdo_flags|=BDOF_LEFTOUT_POS;
		    object->bdo_custom_pos=(left->icon_x<<16)|left->icon_y;
	    }

	    object->bdo_date = left->vol_date;
	    object->bdo_vol_name_len = left->vol_name_len;

	    // Copy label, store entry pointer
	    //stccpy(object->bdo_device_name,left->icon_label,GUI->def_filename_length+1);
	    object->bdo_misc_data=(ULONG)left;

	    // Get icon
	    CurrentDir(parent_lock);

	    backdrop_get_icon(info,object,NULL);

	    CurrentDir(volume_lock);
	}
	UnLock(parent_lock);
    }
    if (org_dir)
	CurrentDir(org_dir);
    UnLock(volume_lock);

    return object;
}


leftout_record *backdrop_leftouts_upgrade(leftout_record *left, BPTR *volume_lock)
{
    struct DosList *volume = 0;
    leftout_record *record = 0;
    BPTR lock;
    STRPTR tmp_src = 0;
    ULONG dest_len = 0;

    if (lock = LockFromPath(left->node.ln_Name, NULL, FALSE))
    {
	BPTR org_dir;
	struct FileLock *fl;

	org_dir = CurrentDir(lock);
	if (*volume_lock = Lock(":", ACCESS_READ))
	{
	    // Get filelock pointer
	    fl=(struct FileLock *)BADDR(*volume_lock);

	    // Get volume entry
	    if (volume=(struct DosList *)BADDR(fl->fl_Volume))
	    {
	        tmp_src = strchr(left->node.ln_Name, ':');
	        dest_len = *(char *)BADDR(volume->dol_Name) + strlen(tmp_src);
	    }
	}
	UnLock(CurrentDir(org_dir));
    }

    // Create new entry
    if (volume && (record = AllocMemH(GUI->position_memory, sizeof(leftout_record) + dest_len)))
    {
	STRPTR tmp_dst, vol_name;

	vol_name = ((char *)BADDR(volume->dol_Name) + 1);

	// Copy name
	tmp_dst = stpcpy(record->name, vol_name);
	strcpy(tmp_dst, tmp_src);

	// Copy label
	strcpy(record->icon_label, ((leftout_record_old *)left)->icon_label);


	// Fill out name pointer and type
	record->node.ln_Name = record->name;
	record->node.ln_Type = PTYPE_LEFTOUT;
	record->node.ln_Pri = 1;

	// Get volume unique-id.
	record->vol_date = volume->dol_misc.dol_volume.dol_VolumeDate;
	record->vol_name_len = *(UBYTE *)BADDR(volume->dol_Name);

	// Store flags
	record->flags = ((leftout_record_old *)left)->flags;

	// Store position
	record->icon_x = ((leftout_record_old *)left)->icon_x;
	record->icon_y = ((leftout_record_old *)left)->icon_y;

	// Replace old record
	Insert((struct List *)&GUI->positions, (struct Node *)record, (struct Node *)left);
	Remove((struct Node *)left);
	FreeMemH(left);
    }

    return record;
}

// Add leftouts belonging to disk
void backdrop_add_volume_leftouts(BackdropInfo *info, char *device, char *volume, struct DateStamp *date, UBYTE len)
{
    leftout_record *left;
    BackdropObject *object;
    BYTE devlen;
    struct List *search;

    devlen = strlen(device) - 1;

    // Lock position list
    lock_listlock(&GUI->positions,FALSE);

    // Go through leftouts
    for (left=(leftout_record *)GUI->positions.list.lh_Head;
	 left->node.ln_Succ;
	 left=(leftout_record *)left->node.ln_Succ)
    {
	// Left-out?
	if (left->node.ln_Type==PTYPE_LEFTOUT)
	{
	    char *name;
	    BPTR volume_lock = 0;

	    search=&info->objects.list;

	    // Get filename
	    name=FilePart(left->node.ln_Name);

	    if ((left->node.ln_Pri == 1 && left->vol_name_len == len &&
		!memcmp(&left->vol_date, date, sizeof(struct DateStamp)) &&
		!memcmp(left->node.ln_Name, volume, len)) ||
		(left->node.ln_Name[devlen] == ':' && !memcmp(left->node.ln_Name, device, devlen)))
	    {
		while (object=(BackdropObject *)FindNameI(search,name))
		{
		    // Is this a left-out?
		    if (object->bdo_type==BDO_LEFT_OUT)
		    {
			// Match entry pointer
			if (object->bdo_misc_data==(ULONG)left) break;
		    }

		    // Continue search
		    search=(struct List *)object;
		}

		if (!object)
		{
		    if (left->node.ln_Pri == 1)
			volume_lock = LockFromVolIdPath(left->node.ln_Name, NULL, &left->vol_date, left->vol_name_len, NULL);
		    else
		    {
			leftout_record *new_left;

			if (new_left = backdrop_leftouts_upgrade(left, &volume_lock))
			    left = new_left;
		    }
		}

	        if (volume_lock)
	        {
		    if (object = backdrop_make_volume_leftout(info, left, volume_lock))
			    backdrop_place_object(info,object);
		}
	    }
	}
    }

    // Lock position list
    unlock_listlock(&GUI->positions);
}


// Add left-out icons (icon list must be locked)
void backdrop_add_leftouts(BackdropInfo *info)
{
	leftout_record *left;
	BackdropObject *object;

	// Lock position list
	lock_listlock(&GUI->positions,FALSE);

	// Go through leftouts
	for (left=(leftout_record *)GUI->positions.list.lh_Head;
		left->node.ln_Succ;
		left=(leftout_record *)left->node.ln_Succ)
	{
		// Left-out?
		if (left->node.ln_Type==PTYPE_LEFTOUT)
		{
			struct List *search;
			char *name;

			// Get filename
			name=FilePart(left->node.ln_Name);

			// Go through objects, see if this is already there
			search=&info->objects.list;
			while (object=(BackdropObject *)FindNameI(search,name))
			{
				// Is this a left-out?
				if (object->bdo_type==BDO_LEFT_OUT)
				{
					// Match entry pointer
					if (object->bdo_misc_data==(ULONG)left) break;
				}

				// Continue search
				search=(struct List *)object;
			}

			// Not there?
			if (!object)
			{
			    BPTR volume_lock;

			    // Need to upgrade leftout_record?
			    if (left->node.ln_Pri != 1)
			    {
				struct DosList *volume = 0;
				leftout_record *record;
				STRPTR tmp_src = 0;
				ULONG dest_len = 0;

				if (volume_lock = LockFromPath(left->node.ln_Name, NULL, LFPF_TRY_ICON))
				{
				    struct FileLock *fl;

				    // Get filelock pointer
				    fl=(struct FileLock *)BADDR(volume_lock);

				    // Get volume entry
				    if (volume=(struct DosList *)BADDR(fl->fl_Volume))
				    {
				        tmp_src = strchr(left->node.ln_Name, ':');
				        dest_len = *(char *)BADDR(volume->dol_Name) + strlen(tmp_src);
				    }
				}

				// Create new entry
				if (volume && (record = AllocMemH(GUI->position_memory, sizeof(leftout_record) + dest_len)))
				{
				    STRPTR tmp_dst, vol_name;

				    vol_name = ((char *)BADDR(volume->dol_Name) + 1);

				    // Copy name
				    tmp_dst = stpcpy(record->name, vol_name);
				    strcpy(tmp_dst, tmp_src);

				    // Copy label
				    strcpy(record->icon_label, ((leftout_record_old *)left)->icon_label);


				    // Fill out name pointer and type
				    record->node.ln_Name = record->name;
				    record->node.ln_Type = PTYPE_LEFTOUT;
				    record->node.ln_Pri = 1;

				    // Get volume unique-id.
				    record->vol_date = volume->dol_misc.dol_volume.dol_VolumeDate;
				    record->vol_name_len = *(UBYTE *)BADDR(volume->dol_Name);

				    // Store flags
				    record->flags = ((leftout_record_old *)left)->flags;

				    // Store position
				    record->icon_x = ((leftout_record_old *)left)->icon_x;
				    record->icon_y = ((leftout_record_old *)left)->icon_y;

				    // Replace old record
				    Insert((struct List *)&GUI->positions, (struct Node *)record, (struct Node *)left);
				    Remove((struct Node *)left);
				    FreeMemH(left);
				    left = record;
				}
			    }
			    // Get volume lock
			    else
				volume_lock = LockFromVolIdPath(left->node.ln_Name, NULL, &left->vol_date, left->vol_name_len, LFPF_TRY_ICON);

			    if (volume_lock)
			    {
				BPTR parent_lock, org_dir;

				if (parent_lock = ParentDir(volume_lock))
				{
				    // Get new left-out object
				    if (object=backdrop_leftout_new(info,left->node.ln_Name,0,left->icon_label))
				    {
					    // Valid position?
					if (!(left->flags&LEFTOUTF_NO_POSITION))
					{
						// Store position
						object->bdo_flags|=BDOF_LEFTOUT_POS;
						object->bdo_custom_pos=(left->icon_x<<16)|left->icon_y;
					}

					object->bdo_date = left->vol_date;
					object->bdo_vol_name_len = left->vol_name_len;

					// Store entry pointer
					object->bdo_misc_data=(ULONG)left;

					// Get icon
					org_dir = CurrentDir(parent_lock);

					backdrop_get_icon(info,object,NULL);

					CurrentDir(org_dir);
				    }

				    UnLock(parent_lock);
				}

				UnLock(volume_lock);
			    }
			}
		}
	}

	// Lock position list
	unlock_listlock(&GUI->positions);
}


// Create a shortcut
BackdropObject *backdrop_create_shortcut(
	BackdropInfo *info,
	char *name,
	short x,short y)
{
	BPTR lock;
	struct FileInfoBlock __aligned fib;

	// Lock object
	if (!(lock=Lock(name,ACCESS_READ)))
	{
		// Try for icon
		StrCombine(info->buffer,name,".info",sizeof(info->buffer));
		if (!(lock=Lock(info->buffer,ACCESS_READ)))
			return 0;

		// Isolate filename
		stccpy(fib.fib_FileName,FilePart(name),108);
	}

	// Locked object, get some info
	else Examine(lock,&fib);

	// Unlock object
	UnLock(lock);

	// Get destination (shortcut) pathname
	StrCombine(info->buffer,"PROGDIR:Desktop/",fib.fib_FileName,sizeof(info->buffer));

	// Make name unique
	backdrop_unique_name(info->buffer);

	// Make the link
	if (MakeLink(info->buffer,(long)name,TRUE))
	{
		struct DiskObject *icon;

		// Get icon for new link
		if (icon=GetProperIcon(name,0,GCDOF_NOCACHE))
		{
			// Set "position ok" flag
			SetIconFlags(icon,GetIconFlags(icon)|ICONF_POSITION_OK);

			// Update icon position
			SetIconPosition(icon,x,y);

			// Write icon for new link
			PutDiskObject(info->buffer,icon);

			// Free icon
			FreeCachedDiskObject(icon);
		}
	}

	return 0;
}


// Make a unique filename
void backdrop_unique_name(char *name)
{
	BPTR lock;
	short count=1,try;
	char *ptr=0;

	// Does name already have a count?
	if (name[strlen(name)-1]==')')
	{
		// Find start of number
		ptr=name+strlen(name)-1;
		while (ptr>name)
			if (!(is_digit(*ptr))) break;

		// Found it?
		if (ptr>name && is_digit(*(ptr+1)))
		{
			// Bump pointer
			++ptr;

			// Get initial count
			count=atoi(ptr);
		}
	}

	// Loop for a few tries
	for (try=0;try<100;try++)
	{
		// See if file exists
		if (!(lock=Lock(name,ACCESS_READ)))
			return;

		// Unlock the lock
		UnLock(lock);

		// No number yet?
		if (!ptr)
		{
			// Get end of name, check not too long
			if (strlen(FilePart(name))>26)
			{
				ptr=name+strlen(name)-4;
			}
			else
			{
				strcat(name," ");
				ptr=name+strlen(name)-1;
			}

			// Add opening bracket
			*ptr++='(';
			*ptr=0;
		}

		// Increment count
		++count;

		// Print into name
		lsprintf(ptr,"%ld)",count);
	}
}
