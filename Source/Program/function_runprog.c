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
#include "misc_protos.h"
#include "function_launch_protos.h"
#include "clock_task.h"
#include "lsprintf_protos.h"
#include "function_protos.h"

// RUN internal function
GALILEOFM_FUNC(function_runprog)
{
	FunctionEntry *entry;
	BPTR old;

	if (!(handle->func_source_lock))
		return 1;

	// Change directory
	old=CurrentDir(handle->func_source_lock);

	// Build list of sources
	while (entry=function_get_entry(handle))
	{
		struct DiskObject *icon;
		BPTR file;

		// Does file have an icon? (don't check if not allowed to)
		if (!(handle->flags&FUNCF_RUN_NO_ICONS) && (icon=GetDiskObject(entry->fe_name)))
		{
			Lister *lister;
			char *tool=0;
			BOOL done=0;

			// Does icon have a tool?
			if (icon->do_Type==WBPROJECT &&
				icon->do_DefaultTool &&
				icon->do_DefaultTool[0])
			{
				// Test 'more' trap
				if (file_trap_more(entry->fe_name,icon->do_DefaultTool)) done=1;

				// Get pointer to tool name
				else tool=FilePart(icon->do_DefaultTool);
			}

			// Get pointer to file name
			else tool=FilePart(entry->fe_name);

			// Not run trapped more?
			if (!done)
			{
				// Try to get current lister
				if (lister=function_lister_current(&handle->func_source_paths))
				{
					// Set screen title
					if (lister->window)
					{
						char buf[144];

						if (tool) lsprintf(buf,GetString(&locale,MSG_LAUNCHING_PROGRAM),tool);
						title_error(buf,0);
					}
				}

				// Build name with quotes
				lsprintf(handle->func_work_buf,"\"%s\"",entry->fe_name);

				// Launch program
				WB_Launch(handle->func_work_buf,GUI->screen_pointer,0,environment->env->default_stack,0,0,0,0);
			}

			// Free icon
			FreeDiskObject(icon);
		}

		// No; try to open file
		else
		if (file=Open(entry->fe_name,MODE_OLDFILE))
		{
			char *command,*output,*args;
			long test;

			// Read first long
			Read(file,(char *)&test,sizeof(long));
			Close(file);

			// Pointer to command buffer
			command=handle->func_work_buf;
			*command=0;

			// Pointer to argument buffer
			args=handle->func_work_buf+824;
			*args=0;

			// Pointer to output buffer
			output=handle->func_work_buf+904;
			lsprintf(output,"%s%s/AUTO/CLOSE/WAIT/SCREEN %s",
				environment->env->output_device,
				environment->env->output_window,
				get_our_pubscreen());

			// Is file executable?
			if (test==0x000003f3)
			{
				// Build requester text
				lsprintf(handle->func_work_buf+512,
					GetString(&locale,MSG_ENTER_ARGUMENTS_FOR),
					entry->fe_name);

				// Display requester
				if (function_request(
					handle,
					handle->func_work_buf+512,
					SRF_BUFFER,
					args,79,
					GetString(&locale,MSG_OKAY),
					GetString(&locale,MSG_CANCEL),0))
				{
					// Build command command
					strcpy(command,"\"");
					strcat(command,entry->fe_name);
					strcat(command,"\"");

					// Add arguments
					if (*args)
					{
						*(args-1)=' ';
						strcat(command,args-1);
					}
				}
			}

			// Otherwise try to lock file
			else
			if (file=Lock(entry->fe_name,ACCESS_READ))
			{
				// Examine file
				Examine(file,handle->s_info);
				UnLock(file);

				// Is S bit set?
				if (handle->s_info->fib_Protection&FIBF_SCRIPT)
				{
					// Execute script
					strcpy(command,"execute \"");
					strcat(command,entry->fe_name);
					strcat(command,"\"");
				}
			}

			// Have we a command?
			if (*command)
			{
				// Open output
				if (!(file=Open(output,MODE_OLDFILE)))
					file=Open("nil:",MODE_OLDFILE);

				// Run command
				CLI_Launch(command,(struct Screen *)-1,0,file,0,LAUNCHF_USE_STACK,environment->env->default_stack, NULL);
			}
		}

		// Get next entry
		function_end_entry(handle,entry,1);
	}

	// Restore directory
	CurrentDir(old);

	return 1;
}
