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
#include "/Modules/modules_internal_protos.h"

// CONFIGURE a lister
GALILEOFM_FUNC(function_configure)
{
	Lister *lister;
	struct Library *InternalModuleBase;
	ListFormat format;
	ListFormat *pass_format=0;
	ULONG ret;

	// Get current lister
	if (!(lister=function_lister_current(&handle->func_source_paths)))
		return 0;

	// Get current format
	format=lister->cur_buffer->buf_ListFormat;

	// Open lister format module
	if (!(InternalModuleBase=OpenModule("listerformat.gfmmodule")))
		return 0;

	// Send message to say hello
	IPC_Command(lister->ipc,LISTER_CONFIGURE,1,0,0,0);

	// Edit list format
	ret=Module_Entry_Internal(
		(struct List *)&format,
		(struct Screen *)lister->window,
		handle->ipc,
		&main_ipc,
		1,
		(ULONG)&environment->env->list_format);

	// Close library
	CloseLibrary(InternalModuleBase);

	// Need to refresh the lister?
	if (ret)
	{
		// Allocate format
		if (pass_format=AllocVec(sizeof(ListFormat),0))
		{
			// Copy format in
			CopyMem((char *)&format,(char *)pass_format,sizeof(ListFormat));
		}
	}

	// Send message to say goodbye
	IPC_Command(lister->ipc,LISTER_CONFIGURE,0,(APTR)ret,pass_format,0);
}
