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
#include "lister_protos.h"
#include "misc_protos.h"
#include "requesters.h"
#include "buffers_protos.h"
#include "backdrop_protos.h"
#include "lsprintf_protos.h"
#include "replace.h"
#include "status_text_protos.h"
#include "file_select.h"

// Check that source and destination paths are different
short function_check_same_path(
	char *source_path,
	char *dest_path)
{
	BPTR lock1,lock2;
	short ret;

	// Compare the strings first
	if (stricmp(source_path,dest_path)==0)
		return LOCK_SAME;

	// Lock paths
	if (!(lock1 = LockFromPath(source_path, NULL, NULL)) ||
		!(lock2 = LockFromPath(dest_path, NULL, NULL)))
	{
		UnLock(lock1);
		return LOCK_DIFFERENT;
	}

	// Compare locks
	ret=SameLock(lock1,lock2);

	// Unlock locks
	UnLock(lock1);
	UnLock(lock2);

	// Return success code
	return ret;
}


// Check that a destination file is ok to write to
check_file_destination(
	FunctionHandle *handle,
	FunctionEntry *entry,
	char *destination,
	BPTR destination_lock,
	short *confirm_flags)
{
	BPTR lock,dest_lock=0, org_dir = 0;
	short confirm;
	char name[35];

	// Get confirm flags
	confirm=*confirm_flags;

	// If entry is an icon, it's ok
	if (entry->fe_flags&FUNCENTF_ICON) return 1;

	if (destination_lock)
	    org_dir = CurrentDir(destination_lock);

	// If destination doesn't exist it's ok
	if (!(lock=Lock(destination,ACCESS_READ)))
	{
	    if (org_dir)
		CurrentDir(org_dir);

	    return 1;
	}

	// Get destination info
	Examine(lock,handle->d_info);

	// Get parent directory if we need it
	if (handle->d_info->fib_DirEntryType<0 &&
		entry->fe_type<0 &&
		!(confirm&COPYF_SKIP_ALL) &&
		!(confirm&COPYF_DELETE_ALL))
	{
	    if (destination_lock)
		dest_lock = destination_lock;
	    else
		dest_lock = ParentDir(lock);
	}

	// Unlock lock
	UnLock(lock);

	if (org_dir)
	{
	    CurrentDir(org_dir);
	    org_dir = 0;
	}

	// Get truncated filename
	get_trunc_filename(FilePart(destination),name);

	// If destination is a directory
	if (handle->d_info->fib_DirEntryType>0)
	{
		// If source is also a directory, that's fine, we can return
		if (entry->fe_type>=0) return 1;

		// Can't copy a file over a directory
		lsprintf(handle->func_work_buf,
			GetString(&locale,MSG_ERROR_DEST_IS_DIRECTORY),
			name);

		// Put up requester
		if (function_request(
			handle,
			handle->func_work_buf,
			0,
			GetString(&locale,MSG_SKIP),
			GetString(&locale,MSG_ABORT),0)) return 0;
		return -1;
	}

	// Destination must be a file; is source a directory?
	if (entry->fe_type>0)
	{
		int ret;

		// Build requester text
		lsprintf(handle->func_work_buf,
			GetString(&locale,MSG_ERROR_DEST_IS_FILE),
			name);

		// Put up requester
		if ((ret=function_request(
			handle,
			handle->func_work_buf,
			0,
			GetString(&locale,MSG_SKIP),
			GetString(&locale,MSG_REPLACE),
			GetString(&locale,MSG_ABORT),0))==0) return -1;

		// Skip?
		if (ret==1) return 0;
	}

	// Skip all?
	else
	if (confirm&COPYF_SKIP_ALL)
		return 0;

	// Do we need to ask?
	else
	if (!(confirm&COPYF_DELETE_ALL))
	{
		short ret;
		PathNode *path;
		struct Window *win=0;
		BPTR lock = 0, source_lock;

		if (handle->func_source_lock)
		{
		    lock = Lock(entry->fe_name,ACCESS_READ);
		}
		else
		{
		    if (destination_lock)
			lock = Lock(entry->fe_name,ACCESS_READ);
		}

		// Get source information
		if (!lock)
		{
		    if (!handle->func_dest_lock)
			UnLock(dest_lock);

		    return 0;
		}

		// Get information
		Examine(lock,handle->s_info);

		if (handle->func_source_lock)
		    source_lock = handle->func_source_lock;
		else
		{
		    // Get parent dir
		    source_lock=ParentDir(lock);
		}
		UnLock(lock);

		// Get window
		if ((path=function_path_current(&handle->func_source_paths)) && path->pn_lister)
			win=path->pn_lister->window;

		// Show requester
		ret=SmartAskReplace(
			win,
			GUI->screen_pointer,
			handle->ipc,
			dest_lock,
			handle->d_info,
			source_lock,
			handle->s_info,
			handle->replace_option);

		// Unlock locks
		if (!handle->func_source_lock)
		    UnLock(source_lock);
		if (!destination_lock)
		    UnLock(dest_lock);

		// Abort?
		if (ret==REPLACE_ABORT) return -1;

		// Remember setting
		handle->replace_option=ret&~REPLACE_ALL;

		// Skip
		if (ret==REPLACE_LEAVE) return 0;

		// Skip all?
		if (ret==(REPLACE_LEAVE|REPLACE_ALL))
		{
			*confirm_flags|=COPYF_SKIP_ALL;
			return 0;
		}

		// Replace all?
		else
		if (ret==(REPLACE_REPLACE|REPLACE_ALL)) *confirm_flags|=COPYF_DELETE_ALL;
	}

	// Is destination file delete protected?
	if (handle->d_info->fib_Protection&FIBF_DELETE)
	{
		// Unprotect all not set?
		if (!(confirm&COPYF_UNPROTECT_ALL))
		{
			short ret;

			// Build requester text
			lsprintf(handle->func_work_buf,
				GetString(&locale,MSG_DELETE_PROTECTED),
				name);

			// Put up requester
			if ((ret=function_request(
				handle,
				handle->func_work_buf,
				0,
				GetString(&locale,MSG_UNPROTECT),
				GetString(&locale,MSG_UNPROTECT_ALL),
				GetString(&locale,MSG_ABORT),
				GetString(&locale,MSG_SKIP),0))==3) return -1;

			// Skip?
			if (ret==0) return 0;

			// Unprotect all?
			if (ret==2) *confirm_flags|=COPYF_UNPROTECT_ALL;
		}
		if (destination_lock)
		    org_dir = CurrentDir(destination_lock);

		// Clear delete bit
		SetProtection(destination,handle->d_info->fib_Protection&(~FIBF_DELETE));
	}
	if (destination_lock && !org_dir)
	    org_dir = CurrentDir(destination_lock);

	// Delete destination file
	while (!(DeleteFile(destination)))
	{
		short ret;

		// Display error requester
		if ((ret=function_error(
			handle,
			handle->d_info->fib_FileName,
			MSG_DELETING,
			IoErr()))!=1)
		{
		    if (org_dir)
		    {
			CurrentDir(org_dir);
		    }
		    return ret;
		}
	}

	if (org_dir)
	    CurrentDir(org_dir);

	// Okay to proceed
	return 1;
}


// Handle an error
function_error(
	FunctionHandle *handle,
	char *entry_name,
	int action_msg,
	int error_code)
{
	BOOL skip=1;
	char name[35];
	int ret;

	// Get truncated file name
	get_trunc_filename(entry_name,name);
		
	// If error code is negative it indicates we don't want a "skip" button
	if (error_code<0)
	{
		error_code=-error_code;
		skip=0;
	}

	// Get error message
	if (error_code>0)
	{
		Fault(error_code,GetString(&locale,MSG_DOS_ERROR),handle->func_work_buf,60);
		lsprintf(handle->func_work_buf+768,handle->func_work_buf,error_code);
	}
	else strcpy(handle->func_work_buf+768,GetString(&locale,MSG_ERROR_UNKNOWN_CODE));

	// Build requester text
	lsprintf(handle->func_work_buf,
		GetString(&locale,MSG_ERROR_OCCURED),
		GetString(&locale,action_msg),
		name,
		handle->func_work_buf+768);

	// Display requester
	ret=function_request(
		handle,
		handle->func_work_buf,
		0,
		GetString(&locale,MSG_TRY_AGAIN),
		(skip)?GetString(&locale,MSG_SKIP):(unsigned char *)-1,
		GetString(&locale,MSG_ABORT),0);

	// Return appropriate value
	if (ret==0) return -1;
	if (ret==2) return 0;
	return ret;
}


// See if there's any directories in the entry list
BOOL function_check_dirs(FunctionHandle *handle)
{
	FunctionEntry *entry;

	// Go through entry list
	for (entry=(FunctionEntry *)handle->entry_list.lh_Head;
		entry->fe_node.mln_Succ;
		entry=(FunctionEntry *)entry->fe_node.mln_Succ)
	{
		// Is this a directory?
		if (entry->fe_type>0) return 1;
	}

	return 0;
}


// Handle abort for a function
void function_abort(FunctionHandle *handle)
{
	PathNode *node;

	// Get current lister
	if ((node=handle->func_source_paths.current) &&
		node->pn_node.mln_Succ)
	{
		// Display abort text
		status_abort(node->pn_lister);

		// Set flag to save title
		node->pn_flags|=LISTNF_NO_TITLE;
	}
}


// Display an error message in the title bar of a lister
void function_error_text(FunctionHandle *handle,int code)
{
	PathNode *node;

	// Get current lister
	if ((node=handle->func_source_paths.current) &&
		node->pn_node.mln_Succ)
	{
		// Display abort text
		status_display_error(node->pn_lister,code);

		// Set flag to save title
		node->pn_flags|=LISTNF_NO_TITLE;
	}
}


// Display text in the title bar
void function_text(FunctionHandle *handle,char *text)
{
	PathNode *node;

	// Get current lister
	if ((node=handle->func_source_paths.current) &&
		node->pn_node.mln_Succ)
	{
		// Display text
		status_text(node->pn_lister,text);

		// Set flag to save title
		node->pn_flags|=LISTNF_NO_TITLE;
	}
}


// Display a requester for a function
function_request(
	FunctionHandle *handle,
	char *message,
	ULONG flags,...)
{
	PathNode *path;
	APTR parent;
	short ret;

	// Get window
	if ((path=function_path_current(&handle->func_source_paths)) && path->pn_lister)
		parent=path->pn_lister->window;

	// Use screen
	else
	{
		parent=GUI->window;
		flags|=SRF_MOUSE_POS;
	}

	// Do requester
	ret=super_request(parent,message,handle->ipc,&flags);

	// Let lister update itself
	if (path && path->pn_lister)
		IPC_Command(path->pn_lister->ipc,LISTER_CHECK_REFRESH,0,0,0,REPLY_NO_PORT);

	return ret;
}


// Cleanup after a function
void function_cleanup(FunctionHandle *handle,PathNode *node,BOOL full)
{
	// Valid node?
	if (node)
	{
		FunctionEntry *entry;
		DirBuffer *buffer=0;
		Lister *lister;

		// Get lister
		if (lister=node->pn_lister)
		{
			// Get source buffer and lock it
			buffer=lister->cur_buffer;
			buffer_lock(buffer,TRUE);
		}

		// Go through entry list
		for (entry=(FunctionEntry *)handle->entry_list.lh_Head;
			entry->fe_node.mln_Succ;)
		{
			FunctionEntry *next=(FunctionEntry *)entry->fe_node.mln_Succ;

			// Does entry need to be removed?
			if (entry->fe_flags&FUNCENTF_REMOVE)
			{
				// Add change for remove
				function_filechange_delfile(handle,node->pn_path,node->pn_lock,entry->fe_name,0,0);

				// Remove entry
				Remove((struct Node *)entry);
			}

			// Deselected?
			else
			if (entry->fe_flags&FUNCENTF_UNSELECT)
			{
				// Valid buffer and entry?
				if (buffer && buffer->buf_CurrentLister && entry->fe_entry)
				{
					// Icon?
					if (entry->fe_flags&FUNCENTF_ICON_ACTION)
					{
						// Deselect icon
						((BackdropObject *)entry->fe_entry)->bdo_state=0;

						// If icon is borderless, show it immediately
						if (!backdrop_icon_border((BackdropObject *)entry->fe_entry))
						{
							// Erase it
							backdrop_erase_icon(lister->backdrop_info,(BackdropObject *)entry->fe_entry,0);
						}

						// Fix selection count
						backdrop_fix_count(lister->backdrop_info,0);
					}

					// Normal file
					else
					if (entry->fe_entry->de_Flags&ENTF_SELECTED)
						deselect_entry(buffer,entry->fe_entry);
				}

				// Clear flag
				entry->fe_flags&=~FUNCENTF_UNSELECT;
			}

			// Get next
			entry=next;
		}

		// Unlock buffer
		if (buffer) buffer_unlock(buffer);

		// Got a lister?
		if (lister)
		{
			// Full cleanup?
			if (full)
			{
				// Do any other changes
				function_perform_changes(handle,node);

				// Turn progress indicator off
				IPC_Command(
					lister->ipc,
					LISTER_PROGRESS_OFF,
					0,0,0,0);

				// Was it the main progress indicator?
				if (handle->progress_lister==lister)
					handle->progress_lister=0;
			}
		}
	}
}



// Do changes to listers
void function_do_lister_changes(
	FunctionHandle *handle,
	PathList *list)
{
	PathNode *lister;

	// Go through listers
	for (lister=(PathNode *)list->list.mlh_Head;
		lister->pn_node.mln_Succ;
		lister=(PathNode *)lister->pn_node.mln_Succ)
	{
		// Do changes for this lister
		function_cleanup(handle,lister,1);
	}
}


// Do changes to a lister
void function_perform_changes(
	FunctionHandle *handle,
	PathNode *path)
{
	// Rescan directory?
	if (path->pn_flags&LISTNF_RESCAN && path->pn_lister)
	{
		// Rescan
		if (path->pn_lister)
			IPC_Command(path->pn_lister->ipc,LISTER_RESCAN,0,0,0,0);

		// Clear flag
		path->pn_flags&=~LISTNF_RESCAN;
	}

	// Update datestamp?
	else
	if (path->pn_flags&LISTNF_UPDATE_STAMP)
	{
		// Valid lister?
		if (path->pn_lister)
			IPC_Command(path->pn_lister->ipc,LISTER_UPDATE_STAMP,0,0,0,0);

		// Otherwise, do global lister update
		else update_lister_global(path->pn_path, path->pn_lock);

		// Clear flag
		path->pn_flags&=~LISTNF_UPDATE_STAMP;
	}
}


// Build information string
void function_build_info(FunctionHandle *handle,char *src,char *dst,short which)
{
	PathNode *source=0,*dest=0;

	// Get current source and destination
	if (!src) source=function_path_current(&handle->func_source_paths);
	if (!dst && (which&2)) dest=function_path_current(&handle->func_dest_paths);

	// If no dest, use source
	if (!dst && !dest && which&2)
	{
		if (!(dest=source)) dst=src;
	}

	// Get string pointers
	if (!src && source) src=source->pn_path;
	if (!dst && dest) dst=dest->pn_path;

	// Get paths
	if (src) final_path(src,handle->func_work_buf+512);
	if (dst) final_path(dst,handle->func_work_buf+768);

	// Destination?
	if (which&2)
	{
		// Build information string
		lsprintf(handle->func_work_buf,
			GetString(&locale,MSG_FROM_TO),
			handle->func_work_buf+512,
			handle->func_work_buf+768);
	}

	// Only source
	else
	{
		// Build information string
		lsprintf(handle->func_work_buf,
			GetString(&locale,MSG_FROM),
			handle->func_work_buf+512);
	}

	// Set information string
	function_progress_info(handle,handle->func_work_buf);
}


void get_trunc_filename(char *source,char *dest)
{
	if (!source)
		*dest=0;
	else
	{
		stccpy(dest,source,31);
		if (strlen(source) > 30)
			strcpy(dest+27,"...");
	}
}

void get_trunc_path(char *source,char *dest)
{
	if (!source)
		*dest=0;
	else
	{
		ULONG src_len;

		src_len = strlen(source);

		if (src_len < 31)
		{
		    strcpy(dest, source);
		    return;
		}

		stccpy(dest, source, 14);
		strcpy(dest + 13, "...");
		strcpy(dest + 16, source + src_len - 15);
		dest[30] = 0;
	}
}
