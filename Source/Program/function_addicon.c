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
#include "icons.h"
#include "lsprintf_protos.h"

typedef struct
{
	short	valid;
	short	confirm_each;
	ULONG	flags;
	ULONG	mask;
	short	change;
/*
	char	*source_file;
	short	savepos;
*/
} AddIconData;

enum
{
	ARG_FILE,
	ARG_BORDER,
	ARG_NOBORDER,
	ARG_LABEL,
	ARG_NOLABEL,
	ARG_CHANGE,
/*
	ARG_SOURCE,
	ARG_SAVEPOS,
*/
};

// ADDICON internal function
GALILEOFM_FUNC(function_addicon)
{
	FunctionEntry *entry;
	PathNode *path;
	long count=1;
	short ret=1;
	AddIconData *data;

	// Get data
	data=(AddIconData *)handle->inst_data;

	// First time through?
	if (!data->valid)
	{
		// Start by always confirming replace
		data->confirm_each=1;
		data->valid=1;
		data->flags=0;
		data->mask=0;
/*
		data->source_file=0;
		data->savepos=0;
*/
		// Parsed arguments?
		if (instruction->ipa_funcargs)
		{
			// Only change flags?
			if (instruction->ipa_funcargs->FA_Arguments[ARG_CHANGE])
			{
				data->change=1;
				data->confirm_each=0;
			}

			// Border/No border?
			if (instruction->ipa_funcargs->FA_Arguments[ARG_BORDER])
			{
				data->mask|=ICONF_BORDER_OFF|ICONF_BORDER_ON;
				data->flags|=ICONF_BORDER_ON;
			}
			else
			if (instruction->ipa_funcargs->FA_Arguments[ARG_NOBORDER])
			{
				data->mask|=ICONF_BORDER_OFF|ICONF_BORDER_ON;
				data->flags|=ICONF_BORDER_OFF;
			}

			// Label/No label?
			if (instruction->ipa_funcargs->FA_Arguments[ARG_LABEL])
			{
				data->mask|=ICONF_NO_LABEL;
			}
			else
			if (instruction->ipa_funcargs->FA_Arguments[ARG_NOLABEL])
			{
				data->flags|=ICONF_NO_LABEL;
				data->mask|=ICONF_NO_LABEL;
			}

/*
			// Source file supplied?
			if (instruction->funcargs->FA_Arguments[ARG_SOURCE])
				data->source_file=(char *)instruction->funcargs->FA_Arguments[ARG_SOURCE];

			// Save position?
			if (instruction->funcargs->FA_Arguments[ARG_SAVEPOS])
				data->savepos=1;
*/
		}
	}

	// Get current path
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Tell this path to update it's datestamp at the end
	path->pn_flags|=LISTNF_UPDATE_STAMP;

	// Turn progress indicator on
	function_progress_on(
		handle,
		GetString(&locale,MSG_PROGRESS_ADDING_ICONS),
		handle->entry_count,
		PWF_FILENAME|PWF_GRAPH);

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BOOL file_ok=0,replace_image=0;
		BPTR org_dir = 0, tmplock = 0;

		// Update progress indicator
		if (function_progress_update(handle,entry,count))
		{
			function_abort(handle);
			ret=0;
			break;
		}

	        // CD to source-file directory
		if (entry->fe_lock)
		    org_dir = CurrentDir(entry->fe_lock);
	        else
	        if (path->pn_lock)
		    org_dir = CurrentDir(path->pn_lock);

		// Ignore icons
		if (!(isicon(entry->fe_name)))
		{
			// Build icon name
			strcpy(handle->func_work_buf,entry->fe_name);
			strcat(handle->func_work_buf,".info");

			// Need confirmation?
			if (data->confirm_each)
			{
				BPTR lock;

				// Does icon exist?
				if (lock=Lock(handle->func_work_buf,ACCESS_READ))
				{
					UnLock(lock);

					// Build message
					lsprintf(handle->func_work_buf+200,
						GetString(&locale,MSG_ICON_ALREADY_EXISTS),
						entry->fe_name);

					// Display requester
					if (!(ret=function_request(
						handle,
						handle->func_work_buf+200,
						0,
						GetString(&locale,MSG_REPLACE),
						GetString(&locale,MSG_ALL),
						GetString(&locale,MSG_ICON_REPLACE_IMAGE),
						GetString(&locale,MSG_SKIP),
						GetString(&locale,MSG_CANCEL),0)))
					{
						// Aborted?
					        if (org_dir)
						    CurrentDir(org_dir);

					        if (tmplock)
						    UnLock(tmplock);

						function_abort(handle);
						break;
					}

					// Skip?
					else
					if (ret==4) ret=0;

					// Replace all?
					else
					if (ret==2) data->confirm_each=0;

					// Replace image?
					else
					if (ret==3) replace_image=1;
				}
			}

			// Ok to write?
			if (ret)
			{
				// Changing flags?
				if (data->change && data->mask)
				{
					struct DiskObject *icon;

					// Get icon

					// gjp V44 icn stuff
					if (IconBase->lib_Version>=44)
					{
					        icon=GetIconTags(entry->fe_name,
							         ICONGETA_FailIfUnavailable,TRUE,
							         ICONGETA_RemapIcon,FALSE,
							         TAG_DONE);
					}
					else
						icon=GetDiskObject(entry->fe_name);

					if (icon)
					{
						unsigned long flags;

						// Get current flags
						flags=GetIconFlags(icon);

						// Clear mask
						flags&=~data->mask;

						// Set new flags
						flags|=data->flags;

						// Set flags
						SetIconFlags(icon,flags);

						// Write icon
						if	(PutDiskObject(entry->fe_name,icon))
						{
							// Load icon file into listers
							function_filechange_loadfile(handle,path->pn_path,path->pn_lock,entry->fe_name,FFLF_ICON);
							file_ok=1;
						}

						// Free icon
						FreeDiskObject(icon);
					}
				}

				// Write icon
				else
				{
					short ok,err;
					ret=1;
					while (!(ok=icon_write(
								(entry->fe_type>0)?ICONTYPE_DRAWER:ICONTYPE_PROJECT,
								entry->fe_name,
								replace_image,
								data->flags,
								data->mask,
								&err)))
					{
						// Display error requester
						if ((ret=function_error(
							handle,
							entry->fe_name,
							MSG_ADDICONING,
							err))==-1 || ret==0) break;
					}

					// Success?
					if (ok)
					{
						// Load icon file into listers
						function_filechange_loadfile(handle,path->pn_path,path->pn_lock,entry->fe_name,FFLF_ICON);
						file_ok=1;
					}
					else
					if (ret==-1)
					{
					    UnLock(CurrentDir(org_dir));
					    break;
					}
				}
			}
		}

	        // Restore current directory
	        if (org_dir)
		    CurrentDir(org_dir);

	        if (tmplock)
		    UnLock(tmplock);

		// Get next entry, increment count
		count+=function_end_entry(handle,entry,file_ok);

		// Reset result code
		ret=1;
	}

	return ret;
}
