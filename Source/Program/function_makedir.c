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
#include "dirlist_protos.h"
#include "function_launch_protos.h"
#include "function_protos.h"
#include "icons.h"

enum
{
	MAKEDIR_NAME,
	MAKEDIR_NOICON,
	MAKEDIR_SELECT,
	MAKEDIR_NEW,
	MAKEDIR_READ,
};

// MAKEDIR internal function
GALILEOFM_FUNC(function_makedir)
{
	char *dir_path = 0, dir_name[108], *tmp_path = 0;
	PathNode *path;
	short ret=1,ask_flag=1,lister_flag=1,icon_flag=1,def_icon=0,select=0,new_flag=0,read_flag=0;
	short add_flag=1;
	FileChange *change;
	BPTR org_dir = 0, new_dir = 0, change_lock = NULL;
	BOOL first = TRUE;

	// Get current lister
	path=function_path_current(&handle->func_source_paths);

	// Default to icons?
	if (GUI->flags&GUIF_SAVE_ICONS ||
		handle->flags&FUNCF_ICONS ||
		(path && path->pn_lister && path->pn_lister->flags&LISTERF_VIEW_ICONS)) def_icon=1;

	// Parsed arguments?
	if (instruction->ipa_funcargs)
	{
		// No icons?
		if (instruction->ipa_funcargs->FA_Arguments[MAKEDIR_NOICON])
			icon_flag=0;

		// Select?
		if (instruction->ipa_funcargs->FA_Arguments[MAKEDIR_SELECT])
			select=1;

		// New?
		if (instruction->ipa_funcargs->FA_Arguments[MAKEDIR_NEW])
			new_flag=1;

		// Read?
		if (instruction->ipa_funcargs->FA_Arguments[MAKEDIR_READ])
			read_flag=1;

		// Directory?
		if (instruction->ipa_funcargs->FA_Arguments[MAKEDIR_NAME])
		{
			char *arg_path=(char *)instruction->ipa_funcargs->FA_Arguments[MAKEDIR_NAME];

			// Don't need to ask
			ask_flag=0;

			// See if it's a full pathname
			if (strchr(arg_path,'/') || strchr(arg_path,':'))
			{
				dir_path = arg_path;
				lister_flag=0;
			}

			// It's just a name
			else
			{
				if (path->pn_lock)
				    dir_path = arg_path;
				else
				{
				    // Build path
				    if (tmp_path = JoinString(0, handle->func_source_path, arg_path, 0, NULL))
					dir_path = tmp_path;
				}
			}
		}
	}

	// If no path, and we need one, return
	if (!function_valid_path(path) && lister_flag) return 0;

	// Clear name
	dir_name[0] = 0;

	// Loop until successful or aborted
	FOREVER
	{
		// Do we need to ask?
		if (ask_flag)
		{
			// Get directory name from user
			if (!(ret=function_request(
				handle,
				GetString(&locale,MSG_ENTER_DIRECTORY_NAME),
				SRF_BUFFER|SRF_PATH_FILTER,
				dir_name,GUI->def_filename_length,
				GetString(&locale,(def_icon)?MSG_MAKEDIR_ICON:MSG_MAKEDIR_NOICON),
				GetString(&locale,(def_icon)?MSG_MAKEDIR_NOICON:MSG_MAKEDIR_ICON),
				GetString(&locale,MSG_CANCEL),0)))
			{
				ret=-1;
				break;
			}

			// Return pressed on string gadget
			else
			if (ret==-1)
			{
				ret=1;
			}

			// If name is empty, abort
			if (!dir_name[0])
			{
				ret=-1;
				break;
			}

			if (path->pn_lock)
			    dir_path = dir_name;
			else
			{
			    // Build path name
			    if (tmp_path = JoinString(0, handle->func_source_path, dir_name, 0, NULL))
				dir_path = tmp_path;
			    else
			    {
				ret=-1;
				break;
			    }
			}
			// No icon?
			if (def_icon==ret-1) icon_flag=0;
		}

		// If we want an icon, check name isn't too long
		if (icon_flag)
		{
			char *name;

			// Get filename
			name=FilePart(dir_path);

			// Maximum 25 characters
			if (strlen(name)>GUI->def_filename_length-5)
				name[GUI->def_filename_length-5]=0;
		}

		if (first && path && path->pn_lock)
		    org_dir = CurrentDir(path->pn_lock);

		first = FALSE;

		// Create directory
		if (new_dir=OriginalCreateDir(dir_path))
		{
			// Examine the new drawer
			Examine(new_dir,handle->s_info);

			if ((lister_flag && read_flag) || new_flag)
			    ChangeMode(CHANGE_LOCK,new_dir,ACCESS_READ);
			// Unlock the new directory
			else
			    UnLock(new_dir);

			break;
		}

		// Put up error requester
		if (!ask_flag ||
			(function_error(handle,dir_path,MSG_ERROR_CREATING_DIRECTORY,-IoErr()))==-1)
		{
			if (!ask_flag) ret=0;
			else ret=-1;
			break;
		}
	}


	// Aborted?
	if (ret<1)
	{
		if (ret==-1) function_abort(handle);

		if (tmp_path)
		    FreeMemH(tmp_path);

		if (org_dir)
		    CurrentDir(org_dir);

		return 0;
	}

	// Write icon?
	if (icon_flag)
	{
		// Write icon
		if ((icon_write(ICONTYPE_DRAWER,dir_path,0,0,0,0))!=ICONWRITE_OK)
			icon_flag=0;
	}

	if (org_dir)
	    CurrentDir(org_dir);

	// Got a lister?
	if (lister_flag)
	{
		// Read directory into lister?
		if (read_flag)
		{
			// Read directory
			function_read_directory(handle,path->pn_lister,dir_path,new_dir);
			add_flag=0;
		}

		// Keep same lister
		else
		{
			// Tell lister to update it's stamp
			path->pn_flags|=LISTNF_UPDATE_STAMP;

			// Display status text
			function_text(handle,GetString(&locale,MSG_DIRECTORY_CREATED));
		}
	}

	// Read into new lister?
	if (new_flag)
	{
		// Create a new lister
		read_new_lister(dir_path,new_dir,0,IEQUALIFIER_LSHIFT);
	}

	// Not a lister-relative path?
	if (!lister_flag)
	{
		BPTR lock;

		// Lock path
		if (lock = LockFromPath(dir_path, NULL, NULL))
		{
			if (tmp_path)
			    FreeMemH(tmp_path);

			change_lock = ParentDir(lock);

			tmp_path = PathFromLock(NULL, change_lock, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL);

			dir_path = tmp_path;

			UnLock(lock);
		}
	}

	// Use path/lock from lister
	else
	{
	    dir_path = path->pn_path;
	    change_lock = path->pn_lock;
	}

	// Ok to add?
	if (add_flag)
	{
		// Add entry to the list
		if (change=function_filechange_addfile(handle,dir_path,change_lock,handle->s_info,0,0))
		{
			// Select/show it
			change->node.ln_Pri=(select)?FCF_SHOW|FCF_SELECT:FCF_SHOW;
		}

		// Need to add icon?
		if (icon_flag &&
			(change=function_filechange_loadfile(handle,dir_path,change_lock,handle->s_info->fib_FileName,FFLF_ICON)))
		{
			// Select/show it
			if (select && environment->env->settings.icon_flags&ICONFLAG_AUTOSELECT)
				change->node.ln_Pri=FCF_SELECT;
		}
	}

	if (!lister_flag)
	    UnLock(change_lock);

	// Free buffer
	if (tmp_path)
	    FreeMemH(tmp_path);

	return 1;
}
