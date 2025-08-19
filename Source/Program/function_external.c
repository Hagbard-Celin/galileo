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
#include "function_launch_protos.h"

// Run an external instruction
function_external_command(
	FunctionHandle *handle,
	InstructionParsed *instruction)
{
	char *func_string;
	struct Node *current_arg,*last_arg;
	short cont=PARSE_MORE_FILES,limit;
	PathNode *path;

	// See if recursive flag is set
	if (handle->func_parameters.flags&FUNCF_RECURSE_DIRS)
		handle->instruction_flags=INSTF_RECURSE_DIRS;
	else handle->instruction_flags=0;

	// Need a source path and don't have one?
	if (handle->func_flags&FUNCF_NEED_SOURCE &&
		!(handle->func_flags&FUNCF_GOT_SOURCE))
		return 0;

	// Need a destination path and don't have one?
	if (handle->func_flags&FUNCF_NEED_DEST &&
		!(handle->func_flags&FUNCF_GOT_DEST))
		return 0;

	// If we needs files, check there are some
	if ((instruction->ipa_flags&FUNCF_NEED_ENTRIES) &&
		!(function_current_entry(handle)))
		return 0;

	// Get first path
	path=function_path_current(&handle->func_dest_paths);

	// Get cll limit
	if ((limit=environment->env->settings.command_line_length)<256)
		limit=256;

	// Allocate buffer for command line
	if (!(func_string=AllocVec(limit+1,0)))
		return 0;

	// Go through destination paths
	do
	{
		// Store current argument pointer
		last_arg=handle->func_current_arg;
		current_arg=handle->func_current_arg;

		// Get destination path
		if (path)
		{
		    char *tmp_path;

		    if (tmp_path = CopyString(handle->memory, path->pn_path))
		    {
			if (handle->func_dest_path)
			    FreeMemH(handle->func_dest_path);

			handle->func_dest_path = tmp_path;
		    }
		}

		// Loop until this instruction is finished
		while (cont==PARSE_MORE_FILES)
		{
			// Build string
			if ((cont=function_build_instruction(handle,instruction,0,func_string))==PARSE_ABORT)
			{
				handle->inst_flags&=~INSTF_REPEAT;
				FreeVec(func_string);
				return 0;
			}

/*
			// If we needs files, check there are some
			if (instruction->flags&FUNCF_NEED_ENTRIES &&
				!(function_current_entry(handle)))
			{
				cont=PARSE_INVALID;
			}
*/

			// Valid?
			if (cont>=PARSE_OK)
			{
				// Write a line to the script file
				function_write_script(handle,func_string,instruction->ipa_type);

				// Store last argument
				last_arg=handle->func_current_arg;

				// Get next argument to use
				if (current_arg) handle->func_current_arg=handle->func_current_arg->ln_Succ;

				// If no valid argument, get previous or first
				if (!handle->func_current_arg || !handle->func_current_arg->ln_Succ)
				{
					if (current_arg) handle->func_current_arg=current_arg;
					else handle->func_current_arg=handle->func_arguments.lh_Head;
				}

				// Set repeat flag
				handle->inst_flags|=INSTF_REPEAT;
			}
		}

		// Clear repeat flag
		handle->inst_flags&=~INSTF_REPEAT;

		// Restore argument pointer
		handle->func_current_arg=last_arg;

		// Invalid?
		if (cont==PARSE_INVALID)
		{
			FreeVec(func_string);
			return -1;
		}

		// Done with this path
		if (path)
		{
			function_path_end(handle,&handle->func_dest_paths,0);
			path=function_path_next(&handle->func_dest_paths);
		}
	} while (path);

	FreeVec(func_string);
	return 1;
}
