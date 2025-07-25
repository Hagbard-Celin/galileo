/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023-2024 Hagbard Celine

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
#include "replace.h"
#include "/Modules/modules.h"

APTR __asm __saveds HookConvertEntry(
	register __a0 FunctionEntry *entry)
{
	return (entry)?entry->fe_entry:0;
}

Lister *__asm __saveds HookGetLister(
	register __a0 PathNode *path)
{
	return (path)?path->pn_lister:0;
}

ULONG __asm __saveds HookExamineEntry(
	register __a0 FunctionEntry *entry,
	register __d0 long type)
{
	if (!entry) return 0;

	// Name?
	if (type==EE_NAME)
		return (ULONG)entry->fe_name;

	// Type
	else
	if (type==EE_TYPE)
		return (ULONG)entry->fe_type;

	return 0;
}


/*************************************************/

PathNode *__asm __saveds HookGetSource(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf)
{
	PathNode *path;

	// Clear buffer
	if (pathbuf) *pathbuf=0;

	// Valid source?
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Copy path name
	if (pathbuf) strcpy(pathbuf,path->pn_path);
	return path;
}

PathNode *__asm __saveds HookNextSource(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf)
{
	PathNode *path;

	// Clear buffer
	if (pathbuf) *pathbuf=0;

	// Finish with current path
	function_path_end(handle,&handle->func_source_paths,1);

	// Next valid source?
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Copy path name
	if (pathbuf) strcpy(pathbuf,path->pn_path);

	// Out of files?
	if (!function_current_entry(handle))
	{
		// Build new entry list
		function_build_list(handle,&path,handle->cur_instruction);
	}

	return path;
}

void __asm __saveds HookUnlockSource(
	register __a0 FunctionHandle *handle)
{
	// Unlock listers
	function_do_lister_changes(handle,&handle->func_source_paths);
	function_unlock_paths(handle,&handle->func_source_paths,1);
}

PathNode *__asm __saveds HookGetDest(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf)
{
	PathNode *path;

	// Clear buffer
	if (pathbuf) *pathbuf=0;

	// Valid source?
	if (!(path=function_path_current(&handle->func_dest_paths)))
		return 0;

	// Copy path name
	if (pathbuf) strcpy(pathbuf,path->pn_path);
	return path;
}

void __asm __saveds HookEndSource(
	register __a0 FunctionHandle *handle,
	register __d0 long complete)
{
	// Done with this path
	function_path_end(handle,&handle->func_source_paths,complete);
}

void __asm __saveds HookEndDest(
	register __a0 FunctionHandle *handle,
	register __d0 long complete)
{
	// Done with this path
	function_path_end(handle,&handle->func_dest_paths,complete);
}

APTR __asm __saveds HookGetEntry(
	register __a0 FunctionHandle *handle)
{
	return (APTR)function_get_entry(handle);
}

void __asm __saveds HookFirstEntry(
	register __a0 FunctionHandle *handle)
{
	// Initialise current entry pointer
	handle->current_entry=(FunctionEntry *)handle->entry_list.lh_Head;
}

void __asm __saveds HookEndEntry(
	register __a0 FunctionHandle *handle,
	register __a1 APTR entry,
	register __d0 BOOL deselect)
{
	// End entry
	function_end_entry(handle,entry,deselect);
}

void __asm __saveds HookRemoveEntry(
	register __a0 FunctionEntry *entry)
{
	if (entry) entry->fe_flags|=FUNCENTF_REMOVE;
}

long __asm __saveds HookEntryCount(
	register __a0 FunctionHandle *handle)
{
	return handle->entry_count;
}

void __asm __saveds HookReloadEntry(
	register __a0 FunctionHandle *handle,
	register __a1 FunctionEntry *entry)
{
	PathNode *path;

	// Get current path
	if (path=function_path_current(&handle->func_source_paths))
	{
		// Add for reload
		function_filechange_reloadfile(
			handle,
			path->pn_path,
			entry->fe_name,0);
	}
}

void __asm __saveds HookAddFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 struct FileInfoBlock *fib,
	register __a3 Lister *lister)
{
	// No date given?
	if (fib->fib_Date.ds_Days==0 && fib->fib_Date.ds_Minute==0 && fib->fib_Date.ds_Tick==0)
		DateStamp(&fib->fib_Date);

	// Add file
	function_filechange_addfile(handle,path,fib,0,lister);
}

void __asm __saveds HookDelFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __a3 Lister *lister)
{
	// Add file
	function_filechange_delfile(handle,path,name,lister,1);
}

void __asm __saveds HookLoadFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __d0 long flags,
	register __d1 BOOL reload)
{
	// Reload?
	if (reload)
		function_filechange_reloadfile(handle,path,name,flags);

	// Load
	else function_filechange_loadfile(handle,path,name,flags);
}

void __asm __saveds HookDoChanges(
	register __a0 FunctionHandle *handle)
{
	// Do changes
	function_filechange_do(handle,1);
}

BOOL __asm __saveds HookCheckAbort(
	register __a0 FunctionHandle *handle)
{
	return function_check_abort(handle);
}

struct Window *__asm __saveds HookGetWindow(
	register __a0 PathNode *path)
{
	// Valid lister?
	if (path && path->pn_lister)
		return path->pn_lister->window;
	return 0;
}

struct MsgPort *__asm __saveds HookGetPort(
	register __a0 char *portname)
{
	struct MsgPort *port;

	// Copy port name, return pointer to it
	if (portname) strcpy(portname,GUI->rexx_port_name);

	// Look for the port
	Forbid();
	port=FindPort(GUI->rexx_port_name);
	Permit();

	// Return port
	return port;
}

struct Screen *__asm __saveds HookGetScreen(
	register __a0 char *screenname)
{
	// Copy screen name, return pointer to it
	if (screenname) strcpy(screenname,get_our_pubscreen());
	return GUI->screen_pointer;
}

GalileoScreenData *__asm __saveds HookGetScreenData(void)
{
	GalileoScreenData *data;

	// Allocate
	if (data=AllocVec(sizeof(GalileoScreenData),MEMF_CLEAR))
	{
		// Fill it out
		get_screen_data(data);
	}

	return data;
}

void __asm __saveds HookFreeScreenData(register __a0 APTR data)
{
	FreeVec(data);
}

void __asm __saveds HookOpenProgress(
	register __a0 PathNode *path,
	register __a1 char *operation,
	register __d0 long total)
{
	ProgressPacket *prog;

	// No lister?
	if (!path || !path->pn_lister) return;

	// Allocate progress packet
	if (prog=AllocVec(sizeof(ProgressPacket),MEMF_CLEAR))
	{
		// Fill out packet
		prog->total=total;
		prog->operation=operation;
		prog->flags=PWF_FILENAME|PWF_GRAPH;

		// Open progress indicator in lister
		IPC_Command(path->pn_lister->ipc,LISTER_PROGRESS_ON,0,0,prog,0);
	}
}


void __asm __saveds HookUpdateProgress(
	register __a0 PathNode *path,
	register __a1 char *name,
	register __d0 long count)
{
	// Update progress indicator
	if (path && path->pn_lister)
		lister_command(
			path->pn_lister,
			LISTER_PROGRESS_UPDATE,
			count,
			name,
			0,
			0);
}

void __asm __saveds HookCloseProgress(
	register __a0 PathNode *path)
{
	// Close progress indicator
	if (path && path->pn_lister)
		lister_command(
			path->pn_lister,
			LISTER_PROGRESS_OFF,
			0,0,0,0);
}

long __asm __saveds HookReplaceReq(
	register __a0 struct Window *window,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 struct FileInfoBlock *file1,
	register __a4 struct FileInfoBlock *file2,
	register __d0 long flags)
{
	// Show requester
	return SmartAskReplace(
		window,
		screen,
		ipc,
		0,
		file1,
		0,
		file2,
		flags);
}

ULONG __asm __saveds HookGetPointer(
	register __a0 struct pointer_packet *ptr)
{
	// Which type?
	switch (ptr->type)
	{
		// Options
		case MODPTR_OPTIONS:

			// Fill out packet
			ptr->pointer=&environment->env->settings;
			ptr->flags=0;
			return (ULONG)&environment->env->settings;

		// Filetypes
		case MODPTR_FILETYPES:

			// Lock filetype list
			lock_listlock(&GUI->filetypes,FALSE);

			// Fill out packet
			ptr->pointer=&GUI->filetypes;
			ptr->flags=POINTERF_LOCKED;
			return (ULONG)&GUI->filetypes;

		// Handle
		case MODPTR_HANDLE:

			// Allocate a new handle
			if (ptr->pointer=function_new_handle((struct MsgPort *)ptr->pointer,0))
				ptr->flags=POINTERF_LOCKED;
			return (ULONG)ptr->pointer;

		// Default lister format
		case MODPTR_DEFFORMAT:

			// Allocate a format structure
			if (ptr->pointer=AllocVec(sizeof(ListFormat),0))
			{
				CopyMem((char *)&environment->env->list_format,(char *)ptr->pointer,sizeof(ListFormat));
				ptr->flags=POINTERF_LOCKED;
			}
			return (ULONG)ptr->pointer;

		// Command list
		case MODPTR_COMMANDS:
			{
				struct List *list;
				APTR mem_handle;
				CommandList *cmd;

				// Create memory handle and list structure
				if (!(mem_handle=NewMemHandle(2048,512,MEMF_CLEAR|MEMF_PUBLIC)) ||
					!(list=AllocMemH(mem_handle,sizeof(struct List)+sizeof(APTR))))
				{
					FreeMemHandle(mem_handle);
					return 0;
				}

				// Save memory handle pointer in list structure
				*((APTR *)(list+1))=mem_handle;
				NewList(list);

				// Lock command list and go through it
				lock_listlock(&GUI->command_list,FALSE);
				for (cmd=(CommandList *)GUI->command_list.list.lh_Head;cmd->node.mln_Succ;cmd=(CommandList *)cmd->node.mln_Succ)
				{
					struct GalileoCommandList *gcl;
					long size;

					// Skip private commands
					if (cmd->flags&FUNCF_PRIVATE)
						continue;

					// Calculate size needed
					size=sizeof(struct GalileoCommandList);
					size+=strlen(cmd->name)+1;
					if (cmd->desc) size+=strlen((char *)cmd->desc)+1;
					if (cmd->flags&FUNCF_EXTERNAL_FUNCTION && cmd->stuff.module_name)
						size+=strlen((char *)cmd->stuff.module_name)+1;
					if (cmd->template) size+=strlen(cmd->template)+1;
					if (cmd->help_name) size+=strlen(cmd->help_name)+1;

					// Allocate command list node
					if (gcl=AllocMemH(mem_handle,size))
					{
						char *ptr=(char *)(gcl+1);

						// Initialise node
						gcl->gcl_Node.ln_Name=ptr;
						strcpy(gcl->gcl_Node.ln_Name,cmd->name);
						ptr+=strlen(gcl->gcl_Node.ln_Name)+1;
						gcl->gcl_Flags=cmd->flags;

						// Store optional strings
						if (cmd->desc)
						{
							gcl->gcl_Description=ptr;
							strcpy(gcl->gcl_Description,(char *)cmd->desc);
							ptr+=strlen(gcl->gcl_Description)+1;
						}
						if (cmd->flags&FUNCF_EXTERNAL_FUNCTION && cmd->stuff.module_name)
						{
							gcl->gcl_Module=ptr;
							strcpy(gcl->gcl_Module,(char *)cmd->stuff.module_name);
							ptr+=strlen(gcl->gcl_Module)+1;
						}
						if (cmd->template)
						{
							gcl->gcl_Template=ptr;
							strcpy(gcl->gcl_Template,(char *)cmd->template);
							ptr+=strlen(gcl->gcl_Template)+1;
						}
						if (cmd->help_name)
						{
							gcl->gcl_Help=ptr;
							strcpy(gcl->gcl_Help,(char *)cmd->help_name);
						}

						// Add to list
						AddTail(list,&gcl->gcl_Node);
					}
				}
				unlock_listlock(&GUI->command_list);

				// Initialise return packet
				ptr->pointer=list;
				ptr->flags=POINTERF_LOCKED;
			}
			return (ULONG)ptr->pointer;


		// Script list
		case MODPTR_SCRIPTS:
			{
				Att_List *list;
				Att_Node *node;

				// Build script list	
				if (list=Att_NewList(LISTF_POOL))
				{
					for (node=(Att_Node *)script_list->list.lh_Head;node->node.ln_Succ;node=(Att_Node *)node->node.ln_Succ)
						Att_NewNode(list,node->node.ln_Name,node->att_data,ADDNODE_SORT);
				}
				ptr->pointer=list;
				ptr->flags=POINTERF_LOCKED;
			}
			return (ULONG)ptr->pointer;
	}

	return 0;
}

void __asm __saveds HookFreePointer(
	register __a0 struct pointer_packet *ptr)
{
    HookFreePointerDirect(ptr->pointer,ptr->type,ptr->flags);
}

void __asm __saveds HookFreePointerDirect(
	register __a0 APTR pointer,
    register __d0 ULONG type,
    register __d1 ULONG flags)
{
    	// Which type?
	switch (type)
	{
		// Filetypes
		case MODPTR_FILETYPES:

			// Check packet
			if (pointer==&GUI->filetypes)
			{
				// Unlock filetype list
				unlock_listlock(&GUI->filetypes);
			}
			break;

		// Handle
		case MODPTR_HANDLE:

		        if (flags&POINTERF_DELPORT)
		        {
			    FunctionHandle *handle;
			    struct Message *msg;

			    handle=(FunctionHandle *)pointer;

			    // Done message?
			    if (handle->done_msg)
			    {
				    handle->done_msg->mn_Node.ln_Pri=handle->ret_code;
				    PutMsg(handle->done_msg->mn_ReplyPort,handle->done_msg);
			    }

			    // Free reply port
			    while (msg=GetMsg(handle->reply_port))
					ReplyFreeMsg(msg);
			    DeleteMsgPort(handle->reply_port);
		        }

			// Free handle
			function_free((FunctionHandle *)pointer);
			break;

		// Default lister format
		case MODPTR_DEFFORMAT:
			FreeVec(pointer);
			break;

		// Command list
		case MODPTR_COMMANDS:
			{
				struct List *list;
				struct GalileoCommandList *gcl,*next=0;
				APTR mem_handle;

				// Get list and memory handle
				if (!(list=(struct List *)pointer) ||
					!(mem_handle=*((APTR *)(list+1))))
					break;

				// Free list nodes
				for (gcl=(struct GalileoCommandList *)list->lh_Head;gcl->gcl_Node.ln_Succ;gcl=next)
				{
					// Cache next pointer
					next=(struct GalileoCommandList *)gcl->gcl_Node.ln_Succ;

					// Free this entry
					FreeMemH(gcl);
				}

				// Free list and memory handle
				FreeMemH(list);
				FreeMemHandle(mem_handle);
			}
			break;

		// Script list
		case MODPTR_SCRIPTS:
			Att_RemList(pointer,0);
			break;
	}
}

/*
void __asm __saveds HookFreePointer(
	register __a0 struct pointer_packet *ptr)
{
	// Which type?
	switch (ptr->type)
	{
		// Filetypes
		case MODPTR_FILETYPES:

			// Check packet
			if (ptr->pointer==&GUI->filetypes)
			{
				// Unlock filetype list
				unlock_listlock(&GUI->filetypes);
			}
			break;

		// Handle
		case MODPTR_HANDLE:

			// Free handle
			function_free((FunctionHandle *)ptr->pointer);
			break;

		// Default lister format
		case MODPTR_DEFFORMAT:
			FreeVec(ptr->pointer);
			break;

		// Command list
		case MODPTR_COMMANDS:
			{
				struct List *list;
				struct GalileoCommandList *gcl,*next=0;
				APTR mem_handle;

				// Get list and memory handle
				if (!(list=(struct List *)ptr->pointer) ||
					!(mem_handle=*((APTR *)(list+1))))
					break;

				// Free list nodes
				for (gcl=(struct GalileoCommandList *)list->lh_Head;gcl->gcl_Node.ln_Succ;gcl=next)
				{
					// Cache next pointer
					next=(struct GalileoCommandList *)gcl->gcl_Node.ln_Succ;

					// Free this entry
					FreeMemH(gcl);
				}

				// Free list and memory handle
				FreeMemH(list);
				FreeMemHandle(mem_handle);
			}
			break;

		// Script list
		case MODPTR_SCRIPTS:
			Att_RemList(ptr->pointer,0);
			break;
	}
}
*/

ULONG __asm __saveds HookSendCommand(
	register __a0 FunctionHandle *handle,
	register __a1 char *command,
	register __a2 char **result,
	register __d0 ULONG flags)
{
	struct RexxMsg *msg;
	struct MsgPort *rexx_port;
	ULONG ret=0;

	// Clear result pointer
	if (result) *result=0;

	// Got ARexx?
	if (!RexxSysBase) return (ULONG)-1;

	// Just run script?
	if (flags&COMMANDF_RUN_SCRIPT)
	{
		rexx_send_command(command,0);
		return 1;
	}

	// Allocate message
	if (!(msg=BuildRexxMsgExTags(
				handle->reply_port,0,0,
				RexxTag_Arg0,command,
				TAG_END)))
		return (ULONG)RXERR_NO_MEMORY;

	// Want reply?
	if (flags&COMMANDF_RESULT) msg->rm_Action|=RXFF_RESULT;
	else msg->rm_Action&=~RXFF_RESULT;

	// Find rexx port
	Forbid();
	if (rexx_port=FindPort(GUI->rexx_port_name))
	{
		// Send message
		PutMsg(rexx_port,(struct Message *)msg);
		Permit();

		// Get reply
		WaitPort(handle->reply_port);
		GetMsg(handle->reply_port);

		// Wait for reply?
		if (flags&COMMANDF_RESULT)
		{
			// Got result string?
			if (msg->rm_Result2)
			{
				// Want result string?
				if (result)
				{
					// Copy result
					if (*result=AllocVec(strlen((char *)msg->rm_Result2)+1,0))
						strcpy(*result,(char *)msg->rm_Result2);
				}

				// Free result
				DeleteArgstring((STRPTR)msg->rm_Result2);
			}
		}

		// Store result code
		ret=msg->rm_Result1;
	}
	else Permit();

	// Free the message
	FreeRexxMsgEx(msg);

	return ret;
}


void __asm __saveds HookCheckDesktop(
	register __a0 char *path)
{
	// See if path matches desktop path
	if (HookMatchDesktop(path))
	{
		// Update the desktop folder
		misc_startup("galileo_desktop_update",MENU_UPDATE_DESKTOP,GUI->window,0,TRUE);
	}
}


BOOL __asm __saveds HookMatchDesktop(
	register __a0 char *path)
{
	BOOL ret=0;

	// See if path matches desktop path
	if (path)
	{
		BPTR lock1,lock2;

		// Lock both this path and the desktop
		if ((lock1=Lock(path,ACCESS_READ)) &&
			(lock2=Lock(environment->env->desktop_location,ACCESS_READ)))
		{
			// See if they're the same
			if (SameLock(lock1,lock2)==LOCK_SAME)
			{
				ret=1;
			}

			// Unlock the second lock
			UnLock(lock2);
		}

		// Unlock first lock
		UnLock(lock1);
	}

	return ret;
}


UWORD __asm __saveds HookGetDesktop(
	register __a0 char *path)
{
	BPTR lock;
	
	// Get full pathname
	strcpy(path,environment->env->desktop_location);
	if (lock=Lock(path,ACCESS_READ))
	{
		DevNameFromLock(lock,path,256);
		UnLock(lock);
	}

	// Add trailing slash
	AddPart(path,"",256);

	// Return flags
	if (!(environment->env->env_flags&ENVF_DESKTOP_FOLDER))
		return 0;
	else
		return (UWORD)(environment->env->desktop_popup_default+1);
}


short __asm __saveds HookDesktopPopup(register __a0 ULONG flags)
{
	UWORD res;
	PopUpItem *item;

	// Main window open?
	if (!GUI->window) return 0;

	// Make sure main window is active
	ActivateWindow(GUI->window);

	// Disable items
	if (flags&(1<<1) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_LEFTOUT)))
		item->flags|=POPUPF_DISABLED;
	if (flags&(1<<2) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_COPY)))
		item->flags|=POPUPF_DISABLED;
	if (flags&(1<<3) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_MOVE)))
		item->flags|=POPUPF_DISABLED;

	// Do popup menu
	res=DoPopUpMenu(GUI->window,&GUI->desktop_menu->ph_Menu,0,SELECTDOWN);

	// Enable items
	if (flags&(1<<1) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_LEFTOUT)))
		item->flags&=~POPUPF_DISABLED;
	if (flags&(1<<2) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_COPY)))
		item->flags&=~POPUPF_DISABLED;
	if (flags&(1<<3) && (item=GetPopUpItem(&GUI->desktop_menu->ph_Menu,MENU_DESKTOP_MOVE)))
		item->flags&=~POPUPF_DISABLED;

	// Map result
	if (res==MENU_DESKTOP_LEFTOUT)
		return 1;
	if (res==MENU_DESKTOP_COPY)
		return 2;
	if (res==MENU_DESKTOP_MOVE)
		return 3;

	// Cancelled
	return 0;
}


void __asm __saveds HookGetThemes(
	register __a0 char *path)
{
	BPTR lock;
	
	// Get full pathname
	strcpy(path,environment->env->themes_location);
	if (lock=Lock(path,ACCESS_READ))
	{
		DevNameFromLock(lock,path,256);
		UnLock(lock);
	}

	// Add trailing slash
	AddPart(path,"",256);
}

BOOL __asm __saveds HookIsSourceDestLock(
     register __a0 Lister *lister)
{
    BOOL islocked;

    if (lister->flags&LISTERF_SOURCEDEST_LOCK)
	islocked=TRUE;
    else
	islocked=FALSE;

    return islocked;
}

void __asm __saveds HookFakeDir(
	register __a0 Lister *lister,
    register __d0 BOOL fakedir)
{
    if (fakedir)
	lister->more_flags|=LISTERF_FAKEDIR;
    else
    	lister->more_flags&=~LISTERF_FAKEDIR;
}

BOOL __asm __saveds HookIsFakeDir(
     register __a0 Lister *lister)
{
    BOOL isfake;

    if (lister->more_flags&LISTERF_FAKEDIR)
	isfake=TRUE;
    else
	isfake=FALSE;

    return isfake;
}
