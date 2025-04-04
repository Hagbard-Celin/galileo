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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofm.h"

enum
{
	SCANDIR_PATH,
	SCANDIR_NEW,
	SCANDIR_MODE,
	SCANDIR_SHOWALL,
	SCANDIR_CONTAINER,
};

// SCANDIR internal function
GALILEOFM_FUNC(function_scandir)
{
	Lister *lister,*dest=0;
	Cfg_Lister *cfg;
	short path_flag=0,new_flag=0,dest_flag=0,cont_flag=0;
	FunctionEntry *entry;
	unsigned long mode_flags=0;
	char *sel_file;

	// Get current lister
	lister=function_lister_current(&handle->func_source_paths);

	// Clear buffer
	if (!(sel_file=AllocVec(256,MEMF_CLEAR)))
		return 0;

	// Parsed arguments?
	if (instruction->ipa_funcargs)
	{
		// Get arguments
		new_flag=instruction->ipa_funcargs->FA_Arguments[SCANDIR_NEW];
		if (instruction->ipa_funcargs->FA_Arguments[SCANDIR_PATH])
		{
			path_flag=1;
			strcpy(handle->func_source_path,(char *)instruction->ipa_funcargs->FA_Arguments[SCANDIR_PATH]);
		}

		// Mode?
		if (instruction->ipa_funcargs->FA_Arguments[SCANDIR_MODE])
		{
			char *ptr=(char *)instruction->ipa_funcargs->FA_Arguments[SCANDIR_MODE];

			// Icon?
			if (stricmp(ptr,mode_keys[1])==0) mode_flags=GLSTF_ICON;

			// Action?
			else
			if (stricmp(ptr,mode_keys[3])==0) mode_flags=GLSTF_ICON|GLSTF_ICON_ACTION;
		}

		// Show all?
		if (instruction->ipa_funcargs->FA_Arguments[SCANDIR_SHOWALL])
			mode_flags|=GLSTF_SHOW_ALL;

		// Container?
		if (instruction->ipa_funcargs->FA_Arguments[SCANDIR_CONTAINER])
			cont_flag=1;
	}

	// No path yet?
	if (!path_flag)
	{
		// Get first directory (if any)
		while (entry=function_get_entry(handle))
		{
			// Directory?
			if (entry->fe_type>0)
			{
				// Build path name
				AddPart(handle->func_source_path,entry->fe_name,512);

				// Finish with entry
				function_end_entry(handle,entry,FALSE);
				path_flag=1;
				if (!new_flag) dest_flag=1;
				break;
			}

			// Get next
			function_end_entry(handle,entry,FALSE);
		}
	}

	// Container?
	if (cont_flag && *handle->func_source_path)
	{
		char *ptr;
		BPTR lock;

		// Try to lock path
		if (lock=Lock(handle->func_source_path,ACCESS_READ))
		{
			// Get full name
			DevNameFromLock(lock,handle->func_source_path,512);
			UnLock(lock);
		}

		// Get pointer to file name	
		ptr=FilePart(handle->func_source_path);

		// Directory?
		if (!ptr || !*ptr || ptr==handle->func_source_path)
		{
			short len;

			// Strip last /
			if (handle->func_source_path[(len=strlen(handle->func_source_path)-1)]=='/')
				handle->func_source_path[len]=0;

			// Try again
			ptr=FilePart(handle->func_source_path);
		}

		// Copy name and clear from path
		stccpy(sel_file,ptr,39);
		*ptr=0;
	}

	// Read into destination lister?
	if (dest_flag)
	{
		// Get destination lister
		if (dest=function_lister_current(&handle->func_dest_paths))
			lister=dest;
	}

	// No destination lister?
	if (!dest)
	{
		// Unlock destination lister
		function_unlock_paths(handle,&handle->func_dest_paths,0);
	}

	// Read into current lister?
	if (!new_flag && lister)
	{
		// Set flags to read directory
		if (path_flag) handle->flags=GETDIRF_CANCHECKBUFS|GETDIRF_CANMOVEEMPTY;
		else handle->flags=0;

		// Read directory
		function_read_directory(handle,lister,handle->func_source_path);
	}

	// Create a new lister
	else
	if (cfg=NewLister((path_flag)?handle->func_source_path:0))
	{
		// Initialise lister
		lister_init_new(cfg,(new_flag)?0:lister);

		// Set flags
		cfg->lister.flags|=mode_flags;

		// Open lister
		if (lister=lister_new(cfg))
		{
			BPTR lock;

			// Got a path and can lock it?
			if (path_flag && handle->func_source_path[0] &&
				(lock=Lock(handle->func_source_path,ACCESS_READ)))
			{
				short mode=0;

				// Get full path
				NameFromLock(lock,lister->work_buffer,512);
				UnLock(lock);

				// Check path is terminated
				AddPart(lister->work_buffer,"",512);

				// Get mode flags
				if (mode_flags&GLSTF_ICON) mode|=LISTERMODE_ICON;
				if (mode_flags&GLSTF_ICON_ACTION) mode|=LISTERMODE_ICON_ACTION;

				// Get position
				lister->pos_rec=
					GetListerPosition(
						lister->work_buffer,
						0,
						0,
						&cfg->lister.pos[0],
						&mode,
						&lister->format,
						0,
						0,
						GLPF_USE_MODE);
			}

			// Send initialise command
			IPC_Command(lister->ipc,LISTER_INIT,LISTERF_MAKE_SOURCE,GUI->screen_pointer,0,0);
		}
	}

	// File to select?
	if (lister && sel_file[0])
	{
		char *copy;

		// Make copy
		if (copy=AllocVec(strlen(sel_file)+1,0))
		{
			// Copy it
			strcpy(copy,sel_file);

			// Send selection command
			IPC_Command(lister->ipc,LISTER_SELSHOW,1,0,copy,0);
		}
	}

	FreeVec(sel_file);
	return 1;
}
