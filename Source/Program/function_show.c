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
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "function_protos.h"
#include "filetypes_protos.h"
#include "menu_data.h"
#include "lsprintf_protos.h"
#include "callback_protos.h"
#include "/Modules/modules.h"
#include "/Modules/modules_protos.h"
#include "/Modules/modules_internal_protos.h"

extern CONST GalileoCallbackInfo CallBackInfo;

// SHOW/PLAY/READ/PRINT/ICONINFO internal function
GALILEOFM_FUNC(function_show)
{
	FunctionEntry *entry;
	Att_List *list;
	BOOL sync_flag=0;
	short ret=0,count=0,funcid;

	// Got arguments? See if 'wait' flag is set
	if (instruction->ipa_funcargs &&
		instruction->ipa_funcargs->FA_Arguments[1]) sync_flag=1;

	// Or, set in function
	else
	if (handle->flags&FUNCF_SYNC) sync_flag=1;

	// Called by default filetype?
	if ((funcid=command->function)==FUNC_DEFFTYPE)
	{
		// Flag set for create filetype?
		if (environment->env->settings.general_flags&GENERALF_FILETYPE_SNIFFER)
		{
			Cfg_Filetype *type;
			struct Library *ModuleBase;

			// Get first entry
			entry=function_get_entry(handle);

			// See if file really can't be identified
			if (!(type=filetype_identify(entry->fe_name,handle->func_source_lock,FTTYPE_ANY,0,0)) ||
				is_default_filetype(type))
			{
				// Build requester text
				lsprintf(
					handle->func_work_buf,
					GetString(&locale,MSG_SNIFF_CONFIRMATION),
					entry->fe_name);

				// Ask for confirmation
				if ((ret=function_request(
					handle,
					handle->func_work_buf,
					0,
					GetString(&locale,MSG_SNIFF),
					GetString(&locale,MSG_SEARCH_READ),
					GetString(&locale,MSG_CANCEL),0))==1)
				{
					// Open filetype module
					if (ModuleBase=OpenModule("filetype.gfmmodule"))
					{
						// Call module
						ret=Module_Entry(
							0,
							GUI->screen_pointer,
							handle->ipc,
							&main_ipc,
							0,	// FindFileType
							&CallBackInfo);

						// Close module
						CloseLibrary(ModuleBase);
						return ret;
					}

					// Fail
					else DisplayBeep(0);
				}

				// Cancel?
				else
				if (!ret) return 0;

				// Restore flag
				ret=0;
			}
		}

		// Change to smartread
		funcid=FUNC_SMARTREAD;
	}

	// Create source list
	if (!(list=Att_NewList(LISTF_POOL))) return 0;

	// Build list of sources
	while (entry=function_get_entry(handle))
	{
		Att_LockNode *node;
		BPTR entry_lock = 0;

		if (entry->fe_lock)
		    entry_lock = DupLock(entry->fe_lock);
		else
		if (handle->func_source_lock)
		{
		    entry_lock = DupLock(handle->func_source_lock);
		}

		// Add entry to list
		if (node = (Att_LockNode *)Att_NewNode(list,entry->fe_name,(ULONG)entry,ADDNODE_LOCKNODE))
		    node->att_lock = entry_lock;
		else
		if (entry_lock)
		    UnLock(entry_lock);

		// Get next entry
		function_end_entry(handle,entry,1);
		++count;
	}

	// Read?
	if (funcid==FUNC_READ ||
		funcid==FUNC_HEXREAD ||
		funcid==FUNC_ANSIREAD ||
		funcid==FUNC_SMARTREAD)
	{
		struct read_startup *startup;

		// Allocate startup
		if (startup=AllocVec(sizeof(struct read_startup),MEMF_CLEAR))
		{
			// Set files pointer
			startup->files=(struct List *)list;

			// Synchronous?
			if (sync_flag)
			{
				struct Library *InternalModuleBase;

				// Get read module
				if (InternalModuleBase=OpenModule("read.gfmmodule"))
				{
					// Read files
					Module_Entry_Internal(
						startup->files,
						GUI->screen_pointer,
						handle->ipc,
						&main_ipc,
						(ULONG)startup,
						funcid-FUNC_READ);

					// Close module
					CloseLibrary(InternalModuleBase);
					return 1;
				}
			}

			// Otherwise, start async
			else
			if (misc_startup("galileo_read",funcid,0,startup,FALSE))
				return 1;

			// Failed; free startup
			FreeVec(startup);
		}
	}

	// Print
	else
	if (funcid==FUNC_PRINT)
	{
		if (misc_startup("galileo_print",MENU_PRINT,0,list,FALSE))
			return 1;
	}

	// Play?
	else
	if (funcid==FUNC_PLAY)
	{
		long func=FUNC_PLAY,volume=64;
		Lister *lister;
		struct Window *window=0;

		// Got arguments?
		if (instruction->ipa_funcargs)
		{
			// Quiet?
			if (instruction->ipa_funcargs->FA_Arguments[2]) func=FUNC_PLAY_QUIET;

			// Iconified?
			else
			if (instruction->ipa_funcargs->FA_Arguments[3]) func=FUNC_PLAY_ICON;

			// Volume?
			if (instruction->ipa_funcargs->FA_Arguments[4]) volume=*((ULONG *)instruction->ipa_funcargs->FA_Arguments[4]);
		}

		// Current lister?
		if (lister=function_lister_current(&handle->func_source_paths))
			window=lister->window;

		// Synchronous?
		if (sync_flag)
		{
			struct Library *InternalModuleBase;

			// Open module
			if (InternalModuleBase=OpenModule("play.gfmmodule"))
			{
				// Play files
				if (Module_Entry_Internal(
					(struct List *)list,
					GUI->screen_pointer,
					handle->ipc,
					&main_ipc,
					(ULONG)window,
					(volume<<8)|((func==FUNC_PLAY_QUIET)?(1<<0):0))==1) ret=1;

				// Close module
				CloseLibrary(InternalModuleBase);
			}
		}
					
		// Detach
		else
		{
			// Store volume in list
			list->list.lh_Type=volume;

			// Start background player
			misc_startup("galileo_detached_player",func,window,list,0);
			list=0;
			ret=1;
		}
	}

	// IconInfo
	else
	if (funcid==FUNC_ICONINFO)
	{
		Lister *lister;
		struct Window *window=0;

		// Current lister?
		if (lister=function_lister_current(&handle->func_source_paths))
			window=lister->window;

		// Synchronous?
		if (sync_flag)
		{
			struct Library *InternalModuleBase;

			// Open module
			if (InternalModuleBase=OpenModule("icon.gfmmodule"))
			{
				// Show IconInfo
				if (Module_Entry_Internal(
					(struct List *)list,
					GUI->screen_pointer,
					handle->ipc,
					&main_ipc,
					0,
					(environment->env->desktop_flags&DESKTOPF_NO_REMAP)?1:0)) ret=1;

				// Close module
				CloseLibrary(InternalModuleBase);

				// Do update
				function_iconinfo_update(handle,list);
			}
		}
					
		// Detach
		else
		{
			misc_startup("galileo_detached_iconinfo",FUNC_ICONINFO,window,list,0);
			list=0;
			ret=1;
		}
	}

	// Show
	else
	{
		struct Library *InternalModuleBase;

		// Can we detach?
		if (count==1 && handle->instruction_count==1 && !sync_flag)
		{
			misc_startup("galileo_detached_function",FUNC_SHOW,0,list,0);
			list=0;
		}
		
		// Open module
		else
		if (InternalModuleBase=OpenModule("show.gfmmodule"))
		{
			Att_Node *node;

			// Show files
			ret=Module_Entry_Internal(
				(struct List *)list,
				GUI->screen_pointer,
				handle->ipc,
				&main_ipc,
				0,0);

			// Close show module
			CloseLibrary(InternalModuleBase);

			// Clear external list
			NewList((struct List *)&handle->external_list);

			// Go through nodes
			for (node=(Att_Node *)list->list.lh_Head;
				node->node.ln_Succ;
				node=(Att_Node *)node->node.ln_Succ)
			{
				// Leave selected?
				if (node->node.lve_Flags&(SHOWF_SELECTED|SHOWF_DELETE))
				{
					// Clear 'unselect' flag
					((FunctionEntry *)node->att_data)->fe_flags&=~FUNCENTF_UNSELECT;

					// Delete?
					if (node->node.lve_Flags&SHOWF_DELETE)
					{
						ExternalEntry *entry;

						// Create entry
						if (entry=new_external_entry(handle,node->node.ln_Name,handle->func_source_lock))
						{
							// Add to external entry list
							AddTail((struct List *)&handle->external_list,(struct Node *)entry);
						}
					}
				}
			}

			// Anything in the external list?
			if (!(IsListEmpty((struct List *)&handle->external_list)))
			{
				// Run function
				function_user_run(handle,def_function_delete);
			}
		}
	}

	// Free file list
	if (list) Att_RemList(list,REMLIST_UNLOCK);
	return ret;
}


void function_iconinfo_update(FunctionHandle *handle,Att_List *list)
{
	Att_Node *node;

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
			if (lister=function_lister_current(&handle->func_source_paths))
			{
				FunctionEntry *entry;

				// Get entry pointer
				entry=(FunctionEntry *)node->att_data;

				// Reload file and icon
				function_filechange_reloadfile(handle,handle->func_source_path,handle->func_source_lock,entry->fe_name,0);
				function_filechange_reloadfile(handle,handle->func_source_path,handle->func_source_lock,entry->fe_name,FFLF_ICON);
			}
		}
	}
}
