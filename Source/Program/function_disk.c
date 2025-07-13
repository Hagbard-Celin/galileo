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
#include "/Modules/modules_internal_protos.h"

// DISKCOPY, FORMAT internal functions
GALILEOFM_FUNC(function_disk)
{
	struct Library *InternalModuleBase=0;
	struct List list;
	struct Node *node;
	short ret=0;

	// Initialise list
	NewList(&list);

	// Any arguments?
	if (handle->args)
	{
		struct ArgArrayEntry *arg;
	
		// Add arg nodes
		for (arg=(struct ArgArrayEntry *)handle->args->aa_List.mlh_Head;
			arg->aae_Node.mln_Succ;
			arg=(struct ArgArrayEntry *)arg->aae_Node.mln_Succ)
		{
			if (node=AllocMemH(handle->memory,sizeof(struct Node)))
			{
				node->ln_Name=arg->aae_String;
				AddTail(&list,node);
			}
		}
	}

	// Parsed arguments?
	else
	if (instruction->ipa_funcargs)
	{
		// Got a disk?
		if (instruction->ipa_funcargs->FA_Arguments[0])
		{
			// Add a node
			if (node=AllocMemH(handle->memory,sizeof(struct Node)))
			{
				node->ln_Name=(char *)instruction->ipa_funcargs->FA_Arguments[0];
				AddTail(&list,node);
			}
		}
	}

	// Open disk module
	switch (command->function)
	{
		// Diskcopy
		case FUNC_DISKCOPY:
			InternalModuleBase=OpenModule("diskcopy.gfmmodule");
			break;

		// Format
		case FUNC_FORMAT:
			InternalModuleBase=OpenModule("format.gfmmodule");
			break;
	}

	if (InternalModuleBase)
	{
		// Call module
		ret=Module_Entry_Internal(&list,GUI->screen_pointer,handle->ipc,&main_ipc,0,0);
		CloseLibrary(InternalModuleBase);
	}

	return ret;
}
