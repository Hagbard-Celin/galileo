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

// LOADENVIRONMENT/LOADSETTINGS/EDITFILETYPE internal function
GALILEOFM_FUNC(function_loadenvironment)
{
	FunctionEntry *entry;
	env_packet *packet;

	// Get first entry
	if (entry=function_get_entry(handle))
	{
		BPTR lock;

		// Build full name
		function_build_source(handle,entry,handle->work_buffer);

		// See if file exists
		if (lock=Lock(handle->work_buffer,ACCESS_READ))
			UnLock(lock);

		// Load environment?
		if (command->function==FUNC_LOADENVIRONMENT)
		{
			// If file didn't exist, look in default directory
			if (!lock) lsprintf(handle->work_buffer,"PROGDIR:environment/%s",entry->fe_name);

			// Allocate packet
			if (packet=AllocVec(sizeof(env_packet)+strlen(handle->work_buffer),0))
			{
				// Fill out packet
				packet->type=-1;
				strcpy(packet->name,handle->work_buffer);

				// Launch process to open environment
				IPC_Launch(
					&GUI->process_list,
					0,
					"galileo_environment",
					(ULONG)environment_proc,
					STACK_LARGE,
					(ULONG)packet,
					(struct Library *)DOSBase);
			}
		}

		// Edit filetype
		else
		if (command->function==FUNC_EDITFILETYPE)
		{
			// Must not be iconified
			if (GUI->window)
			{
				char *name;

				// Allocate name copy
				if (name=AllocVec(strlen(handle->work_buffer)+1,0))
					strcpy(name,handle->work_buffer);

				// Launch process to configure filetypes
				if (!(misc_startup("galileo_config_filetypes",MENU_FILETYPES,GUI->window,name,1)))
					FreeVec(name);
			}
		}

		// End this entry
		function_end_entry(handle,entry,1);
	}

	return 0;
}
