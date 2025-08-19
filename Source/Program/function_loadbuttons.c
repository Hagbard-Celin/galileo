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
#include "buttons_protos.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "function_protos.h"
#include "start_menu_protos.h"

enum
{
	ARG_NAME,
	ARG_START,
	ARG_LABEL,
	ARG_IMAGE,
	ARG_UNDERMOUSE,
	ARG_TOGGLE,
	ARG_HIDDEN,
	ARG_SHOW,
};

// LOADBUTTONS internal function
GALILEOFM_FUNC(function_loadbuttons)
{
	FunctionEntry *entry;
	BOOL start=0,hidden=0;
	ULONG buttonflags=BUTTONF_FAIL;
	char label[40],image[256];

	// Clear buffers
	label[0]=0;
	image[0]=0;

	// Args?
	if (instruction->ipa_funcargs)
	{
		// Start?
		if (instruction->ipa_funcargs->FA_Arguments[ARG_START])
		{
			// Set flag
			start=1;

			// Get label
			if (instruction->ipa_funcargs->FA_Arguments[ARG_LABEL])
				stccpy(label,(char *)instruction->ipa_funcargs->FA_Arguments[ARG_LABEL],39);

			// Get image
			if (instruction->ipa_funcargs->FA_Arguments[ARG_IMAGE])
				stccpy(image,(char *)instruction->ipa_funcargs->FA_Arguments[ARG_IMAGE],255);
		}

		// Open under mouse?
		if (instruction->ipa_funcargs->FA_Arguments[ARG_UNDERMOUSE])
			buttonflags|=BUTTONF_UNDERMOUSE;

		// Open hidden
		if (instruction->ipa_funcargs->FA_Arguments[ARG_HIDDEN])
			hidden=1;

		// Toggle/Show?
		if (!start &&
			(instruction->ipa_funcargs->FA_Arguments[ARG_TOGGLE] ||
			 instruction->ipa_funcargs->FA_Arguments[ARG_SHOW]))
		{
			IPCData *ipc;
			BOOL matched=0;

			// Get first entry
			if (entry=function_get_entry(handle))
			{
				// Lock buttons list
				lock_listlock(&GUI->buttons_list,FALSE);

				// Look for bank in list
				for (ipc=(IPCData *)GUI->buttons_list.list.lh_Head;
					ipc->node.mln_Succ;
					ipc=(IPCData *)ipc->node.mln_Succ)
				{
					Buttons *buttons=(Buttons *)IPCDATA(ipc);
					BOOL match=0;

					// Or does it match the name?
					if (stricmp(entry->fe_name,buttons->bank->window.name)==0) match=1;

					// Or the filename?
					else
					if (stricmp(entry->fe_name,FilePart(buttons->buttons_file))==0) match=1;

					// Match bank to close?
					if (match)
					{
						// Send message
			 			if (instruction->ipa_funcargs->FA_Arguments[ARG_SHOW])
			 				IPC_Command(ipc,IPC_SHOW,0,GUI->screen_pointer,0,0);
			 			else
							IPC_Quit(ipc,0,FALSE);
						matched=1;
					}
				}

				// Unlock buttons list
				unlock_listlock(&GUI->buttons_list);
			}

			// If we closed a bank, return
			if (matched) return 1;
		}
	}

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BPTR parent_lock = 0;
		Buttons *buttons;

		// Get source-directory lock
	        if (entry->fe_lock)
		    parent_lock = entry->fe_lock;
	        else
	        if (handle->func_source_lock)
		    parent_lock = handle->func_source_lock;

		// Start menu?
		if (start)
		{
			IPCData *ipc;

			// Open as a start menu
			if (ipc=start_new(entry->fe_name,parent_lock,label,image,-1,-1))
			{
				// Not iconified?
				if (GUI->window && !hidden)
					IPC_Command(ipc,IPC_SHOW,0,GUI->screen_pointer,0,0);
			}
		}

		// Open button bank
		else
		if (buttons=buttons_new(entry->fe_name,0,parent_lock,0,0,0,buttonflags))
		{
			// Not iconified?
			if (GUI->window && !hidden)
				IPC_Command(buttons->ipc,BUTTONS_OPEN,0,GUI->screen_pointer,0,0);
		}

		// Get next entry
		function_end_entry(handle,entry,1);
	}

	return 1;
}
