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

// Run a function on some icons
void icon_function(BackdropInfo *info,BackdropObject *only_one,char *data,Cfg_Function *func,ULONG flags)
{
	short count=0;
	struct ArgArray *array;
	BackdropObject *object;
	DirBuffer *buffer=0;
	char *source_path=0;

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Skip invalid icons
		if (object->bdo_type!=BDO_LEFT_OUT ||
			(!info->lister && !(object->bdo_flags&BDOF_DESKTOP_FOLDER))) continue;

		// Want this icon?
		if (!only_one || only_one==object)
		{
			// Is object selected?
			if ((only_one || object->bdo_state) && object->bdo_icon)
			{
				// Increment counts
				if (object->bdo_icon->do_Type==WBDRAWER ||
					object->bdo_icon->do_Type==WBGARBAGE ||
					object->bdo_icon->do_Type==WBPROJECT ||
					object->bdo_icon->do_Type==WBTOOL)
					++count;

				// Only doing one?
				if (only_one) break;
			}
		}
	}

	// Nothing to work on?
	if (count==0 ||
		!(array=NewArgArray()))
	{
		unlock_listlock(&info->objects);
		return;
	}

	// Got a lister?
	if (info->lister)
	{
		// Lock buffer
		buffer_lock((buffer=info->lister->cur_buffer),FALSE);
	}

	// Source path from icon?
	else
	if (only_one && only_one->bdo_path &&
		(source_path=AllocVec(strlen(only_one->bdo_path)+1,0)))
		strcpy(source_path,only_one->bdo_path);

	// Otherwise, assume this is the desktop folder
	else
	if (info->flags&BDIF_MAIN_DESKTOP &&
		(source_path=AllocVec(strlen(environment->env->desktop_location)+1,0)))
		strcpy(source_path,environment->env->desktop_location);

	// Go through backdrop list again
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Skip invalid icons
		if (object->bdo_type!=BDO_LEFT_OUT ||
			(!info->lister && !(object->bdo_flags&BDOF_DESKTOP_FOLDER))) continue;

		// Want this icon?
		if (!only_one || only_one==object)
		{
			// Is object selected?
			if ((only_one || object->bdo_state) && object->bdo_icon)
			{
				char name[80];
				BOOL dir=0,link=0,icon=0;
				DirEntry *entry=0;
				struct ArgArrayEntry *aae;

				// Build name
				stccpy(name,object->bdo_name,sizeof(name));

				// Got a buffer?
				if (buffer)
				{
					// See if we can find this entry
					if ((entry=find_entry(&buffer->entry_list,object->bdo_name,0,buffer->more_flags&DWF_CASE)) ||
						(entry=find_entry(&buffer->reject_list,object->bdo_name,0,buffer->more_flags&DWF_CASE)))
					{
						// Directory?
						if (entry->de_Node.dn_Type>=ENTRY_DEVICE) dir=1;

						// Link?
						if (entry->de_Flags&ENTF_LINK) link=1;

						// See if entry has icon
						if (find_entry(&buffer->entry_list,object->bdo_name,0,(buffer->more_flags&DWF_CASE)|FINDENTRY_ICON) ||
							find_entry(&buffer->reject_list,object->bdo_name,0,(buffer->more_flags&DWF_CASE)|FINDENTRY_ICON))
							icon=1;
					}

					// If not, use the .info name
					else
					{
						// Add .info
						strcat(name,".info");
					}
				}

				// Desktop folder?
				else
				if (info->flags&BDIF_MAIN_DESKTOP)
				{
					// Assume the file has an icon
					icon=1;
				}

				// Get type from icon
				if (!entry)
				{
					if (object->bdo_icon->do_Type==WBDRAWER ||
						object->bdo_icon->do_Type==WBGARBAGE) dir=1;
					if (object->bdo_flags&BDOF_LINK_ICON) link=1;
				}

				// Tack on a / for directories
				if (dir) strcat(name,"/");

				// Allocate array entry
				if (aae=NewArgArrayEntry(array,name))
				{
					// Dir?
					if (dir) aae->aae_Flags|=AAEF_DIR;

					// Link?
					if (link) aae->aae_Flags|=AAEF_LINK;

					// No icon?
					if (!icon) aae->aae_Flags|=AAEF_FAKE_ICON;
				}

				// Only doing one?
				if (only_one) break;
			}
		}
	}

	// Unlock buffer
	if (buffer) buffer_unlock(buffer);

	// Get flags we need
	flags|=(data && *data)?FUNCF_ICONS:FUNCF_ICONS|FUNCF_ASK_DEST;

	// Launch the function
	function_launch(
		FUNCTION_RUN_FUNCTION_EXTERNAL,
		func,
		0,
		flags,
		info->lister,0,
		(info->lister)?info->lister->cur_buffer->buf_Path:source_path,data,
		array,
		0,0);

	// Free source path
	FreeVec(source_path);

	// Unlock list
	unlock_listlock(&info->objects);
}
