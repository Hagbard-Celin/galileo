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
#include "/Modules/modules.h"

// ICONINFO internal function
GALILEOFM_FUNC(function_iconinfo)
{
	PathNode *path;
	FunctionEntry *entry;
	Att_List *list;
	Att_Node *node;
	struct Library *ModuleBase;
	short ret;

	// Create source list
	if (!(list=Att_NewList(0))) return 0;

	// Open show module
	if (!(ModuleBase=OpenModule("icon.gfmmodule")))
	{
		Att_RemList(list,0);
		return 0;
	}

	// Get current path
	if (path=function_path_current(&handle->source_paths))

		// Tell this lister to update it's datestamp at the end
		path->flags|=LISTNF_UPDATE_STAMP;

	// Build list of sources
	while (entry=function_get_entry(handle))
	{
		// Build full name
		function_build_source(handle,entry,handle->work_buffer);

		// Add entry to list
		Att_NewNode(list,handle->work_buffer,(ULONG)entry,0);

		// Get next entry
		function_end_entry(handle,entry,1);
	}

	// Do IconInfo
	ret=
		Module_Entry(
			(struct List *)list,
			GUI->screen_pointer,
			handle->ipc,
			&main_ipc,
			0,0,
			(environment->env->desktop_flags&DESKTOPF_NO_REMAP)?1:0);

	// Go through list of files
	for (node=(Att_Node *)list->list.lh_Head;
		node->node.ln_Succ;
		node=(Att_Node *)node->node.ln_Succ)
	{
		// Was this icon saved?
		if (node->node.ln_Type==2)
		{
			Lister *lister;

			// Do we have a lister?
			if (lister=function_lister_current(&handle->source_paths))
			{
				// Get entry pointer
				entry=(FunctionEntry *)node->data;

				// Reload file and icon
				function_filechange_reloadfile(handle,handle->source_path,entry->name,0);
				function_filechange_reloadfile(handle,handle->source_path,entry->name,FFLF_ICON);
			}
		}
	}

	// Free file list
	Att_RemList(list,0);

	// Close show module
	CloseLibrary(ModuleBase);

	return ret;
}
