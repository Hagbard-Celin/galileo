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
#include <gfm/paths_protos.h>
#include "environment.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "function_protos.h"
#include "menu_data.h"
#include "lsprintf_protos.h"

// LOADENVIRONMENT/LOADSETTINGS/EDITFILETYPE internal function
GALILEOFM_FUNC(function_loadenvironment)
{
	FunctionEntry *entry;
	env_packet *packet;

	// Get first entry
	if (entry=function_get_entry(handle))
	{
		BPTR lock, org_dir = 0;
		char *path = 0;

		if (entry->fe_lock)
		    org_dir = CurrentDir(entry->fe_lock);
		else
		if (handle->func_source_lock)
		    org_dir = CurrentDir(handle->func_source_lock);

		// See if file exists
		if (lock=Lock(entry->fe_name,ACCESS_READ))
		{
			path = PathFromLock(NULL, lock, PFLF_USE_DEVICENAME, NULL);
			UnLock(lock);
		}

		// Load environment?
		if (command->function==FUNC_LOADENVIRONMENT)
		{
			// If file didn't exist, look in default directory
			if (!path)
			{
			    path = AllocMemH(0,strlen(entry->fe_name) + 21);
			    lsprintf(path,"PROGDIR:environment/%s",entry->fe_name);
			}
			// Allocate packet
			if (path && (packet=AllocVec(sizeof(env_packet)+strlen(path),0)))
			{
				// Fill out packet
				packet->type=-1;
				strcpy(packet->name,path);

				// Launch process to open environment
				IPC_Launch(
					&GUI->process_list,
					0,
					"galileo_environment",
					(ULONG)environment_proc,
					STACK_LARGE,
					(ULONG)packet);
			}
		}

		// Edit filetype
		else
		if (path && command->function==FUNC_EDITFILETYPE)
		{
			// Must not be iconified
			if (GUI->window)
			{
				char *name;

				// Allocate name copy
				if (name=AllocVec(strlen(path)+1,0))
					strcpy(name,path);

				// Launch process to configure filetypes
				if (!(misc_startup("galileo_config_filetypes",MENU_FILETYPES,GUI->window,name,1)))
					FreeVec(name);
			}
		}

		if (path)
		    FreeMemH(path);

		if (org_dir)
		    CurrentDir(org_dir);

		// End this entry
		function_end_entry(handle,entry,1);
	}

	return 0;
}
