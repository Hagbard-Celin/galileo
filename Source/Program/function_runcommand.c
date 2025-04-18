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

// RUNCOMMAND internal function
GALILEOFM_FUNC(function_runcommand)
{
	FunctionEntry *entry;
	Cfg_Function *func;

	// Get first entry
	if (!(entry=function_get_entry(handle)))
		return 0;

	// Build full name
	function_build_source(handle,entry,handle->func_work_buf);

	// Load command
	func=function_load_function(handle->func_work_buf);

	// Got function?
	if (func)
	{
		// Set flag to free function when done
		func->function.flags2|=FUNCF2_FREE_FUNCTION;

		// Launch function
		if (function_launch(
				FUNCTION_RUN_FUNCTION_EXTERNAL,
				func,
				0,
				0,
				0,0,
				0,0,
				handle->arg_passthru,
				0,
				0))
		{
			// Clear arg passthru since we passed it on
			handle->arg_passthru=0;
		}
	}

	return 1;
}


// EDITCOMMAND internal function
GALILEOFM_FUNC(function_editcommand)
{
	FunctionEntry *entry;

	// Get first entry
	if (!(entry=function_get_entry(handle)))
		return 0;

	// Build full name
	function_build_source(handle,entry,handle->func_work_buf);

	// Edit the command
	command_new(GUI->backdrop,handle->ipc,handle->func_work_buf);
	return 1;
}


// Load a command file
Cfg_Function *function_load_function(char *name)
{
	APTR iff;
	Cfg_Function *func=0;

	// Open as IFF
	if (!(iff=IFFOpen(name,IFF_READ,ID_GILO)))
		return 0;

	// Find function chunk
	if (IFFNextChunk(iff,ID_FUNC)==ID_FUNC)
	{
		// Read function
		func=ReadFunction(iff,global_memory_pool,0,0);
	}

	// Close IFF file
	IFFClose(iff);
	return func;
}
