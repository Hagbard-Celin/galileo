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
#include "lister_protos.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "callback_protos.h"
#include "function_protos.h"
#include "lsprintf_protos.h"

#define DELETE_UPDATE_TIME	2

typedef struct
{
	short	valid;
	short	confirm_each;
	short	unprotect_all;
	short	skip_all;
} DeleteData;

enum
{
	DELARG_NAME,
	DELARG_QUIET,
};

static char *delfile="DeleteFile";

// DELETE internal function
GALILEOFM_FUNC(function_delete)
{
	FunctionEntry *entry;
	short ret=1;
	long count=1;
	PathNode *path;
	DeleteData *data;
	BOOL change_info=0,del_trash=0;
	CommandList *delete_cmd;
	TimerHandle *timer=0;
	char *ptr;

	// Find the delete command
	ptr=delfile;
	delete_cmd=function_find_internal(&ptr,0);

	// Get data and initialise if necessary
	data=(DeleteData *)handle->inst_data;
	if (!data->valid)
	{
		data->valid=1;
		data->confirm_each=1;
	}

	// Quiet?
	if (instruction->ipa_funcargs &&
		instruction->ipa_funcargs->FA_Arguments[DELARG_QUIET]) data->confirm_each=0;

	// Run from icon mode?
	if (handle->flags&FUNCF_ICONS)
	{
		// Get first entry
		if (entry=function_get_entry(handle))
		{
			// Is is the trashcan?
			if (stricmp(entry->fe_name,"trashcan")==0)
			{
				struct DiskObject *icon;

				// Get icon
				if (icon=GetDiskObject(entry->fe_name))
				{
					// Is it really the trashcan?
					if (icon->do_Type==WBGARBAGE)
					{
						// Set flag
						del_trash=1;
					}

					// Free icon
					FreeDiskObject(icon);
				}
			}
		}
	}

	// Ask before commencing?
	if (del_trash ||
		(environment->env->settings.delete_flags&DELETE_ASK && data->confirm_each))
	{
		short ret;

		// Deleting trashcan?
		if (del_trash)
		{
			// Get requester text
			strcpy(handle->temp_buffer,GetString(&locale,MSG_REALLY_EMPTY_TRASH));
		}

		// Or, just deleting a shortcut?
		else
		if (handle->file_count==handle->link_file_count &&
			handle->dir_count==handle->link_dir_count)
		{
			// Build text
			strcpy(handle->temp_buffer,
				GetString(&locale,
					(handle->file_count+handle->dir_count<2)?MSG_DELETE_LINK_CONFIRM_SINGLE:
															 MSG_DELETE_LINK_CONFIRM_MULTI));
		}

		// Build requester text
		else
		lsprintf(
			handle->temp_buffer,
			GetString(&locale,MSG_DELETE_CONFIRM),
			handle->file_count,
			handle->dir_count);

		// Display requester
		if (!(ret=function_request(
			handle,
			handle->temp_buffer,
			0,
			GetString(&locale,MSG_PROCEED),
			GetString(&locale,MSG_DELETE_ALL),
			GetString(&locale,MSG_CANCEL),0)))
		{
			function_abort(handle);
			return 0;
		}

		// Delete all?
		if (ret==2) data->confirm_each=0;
	}

	// Get current path
	if (path=function_path_current(&handle->func_source_paths))
	{
		// Tell this lister to update it's datestamp at the end
		path->pn_flags|=LISTNF_UPDATE_STAMP;

		// Check desktop
		if (HookMatchLockDesktop(path->pn_lock))
			handle->flags|=FUNCF_RESCAN_DESKTOP;
	}

	// Set flags
	handle->instruction_flags=INSTF_RECURSE_DIRS|INSTF_WANT_DIRS|INSTF_WANT_DIRS_END|INSTF_DIR_CLEAR_SIZES;

	// Turn progress indicator on
	function_progress_on(
		handle,
		GetString(&locale,MSG_PROGRESS_OPERATION_DELETING),
		handle->entry_count,
		PWF_FILENAME|PWF_GRAPH|PWF_INFO);

	// Build info string
	function_build_info(handle,0,0,0);

	// Update free space?
	if (environment->env->settings.copy_flags)
	{
		// Valid lister?
		if (path && path->pn_lister && lister_valid_window(path->pn_lister))
		{
			// Create timer for regular updates
			if (timer=AllocTimer(UNIT_VBLANK,0))
				StartTimer(timer,DELETE_UPDATE_TIME,0);
		}
	}

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BOOL file_ok=1;
		BPTR org_dir = 0;

		// Update?
		if (timer && CheckTimer(timer))
		{
			// Send update message
			IPC_Command(path->pn_lister->ipc,LISTER_REFRESH_NAME,0,0,0,0);

			// Restart timer
			StartTimer(timer,DELETE_UPDATE_TIME,0);
		}

		if (!(entry->fe_type>0 && !(entry->fe_flags&FUNCENTF_EXITED) && !(entry->fe_flags&FUNCENTF_LINK)))
		{
		    // Update progress indicator
		    if (function_progress_update(handle,entry,count))
		    {
			    function_abort(handle);
			    ret=0;
			    break;
		    }

		    // Change info string?
		    if (change_info || entry->fe_flags&FUNCENTF_EXITED)
		    {
			    // Build info string
			    if (entry->fe_flags&FUNCENTF_RECURSE)
			        // Build info string
			        function_build_info(handle,handle->anchor_path,0,0);
			    else
			        function_build_info(handle,handle->func_source_path,0,0);

			    change_info=0;
		    }
		}

		// Check this isn't an exited directory
		if (!(entry->fe_flags&FUNCENTF_EXITED))
		{
			// Confirm each is on?
			if (data->confirm_each)
			{
				BOOL ask=1;

				// Got a directory?
				if (entry->fe_type>0) change_info=1;

				// Is entry a top-level directory?
				if (entry->fe_flags&FUNCENTF_TOP_LEVEL &&
					entry->fe_type>0 &&
					!(entry->fe_flags&FUNCENTF_LINK) &&
					environment->env->settings.delete_flags&DELETE_DIRS)
				{
					// Skip if the trashcan
					if (del_trash && stricmp(entry->fe_name,"trashcan")==0) ask=0;

					// Build requester text
					else
					lsprintf(
						handle->func_work_buf,
						GetString(&locale,MSG_DELETE_DIR_CONFIRM),
						entry->fe_name);
				}

				// Ask for confirmation for normal file?
				else
				if ((entry->fe_flags&FUNCENTF_LINK || entry->fe_type<0) &&
					!(entry->fe_flags&FUNCENTF_ICON) &&
					environment->env->settings.delete_flags&DELETE_FILES)
				{
					// Build requester text
					lsprintf(
						handle->func_work_buf,
						GetString(&locale,MSG_DELETE_FILE_CONFIRM),
						entry->fe_name);
				}

				// Otherwise don't need to ask
				else ask=0;

				// Display requester if necessary
				if (ask)
				{
					// Show requester
					if ((ret=function_request(
						handle,
						handle->func_work_buf,
						0,
						GetString(&locale,MSG_DELETE),
						GetString(&locale,MSG_DELETE_ALL),
						GetString(&locale,MSG_SKIP),
						GetString(&locale,MSG_ABORT),0))==0 || ret==-1)
					{
						// Abort
						ret=-1;
					}

					// Delete all?
					else
					if (ret==2)
					{
						ret=1;
						data->confirm_each=0;
					}

					// Skip?
					else
					if (ret==3) ret=0;
				}
			}
		}

		// It was a directory
		else
		{
			// Set flag to change info display
			change_info=1;

			// If this is the trashcan, don't really delete the directory
			if (del_trash && stricmp(entry->fe_name,"trashcan")==0)
				ret=0;
		}

		// Ok to delete?
		if (ret==1)
		{
			short try=0;

			// Directory?
			if (entry->fe_type>0 && !(entry->fe_flags&FUNCENTF_EXITED) && !(entry->fe_flags&FUNCENTF_LINK))
			{
				// Skip this entry
				ret=2;
				file_ok=1;
			}
			else
			{
			    // CD to source-file directory
			    if (entry->fe_lock)
				org_dir = CurrentDir(entry->fe_lock);
			    else
			    if (path->pn_lock)
				org_dir = CurrentDir(path->pn_lock);
			    else
			    {
				ret = -1;
				goto aborting;
			    }

			    // Loop while unsuccessful
			    while (ret)
			    {
				    short err_code;

				    // Delete file
				    ret=GalileoDeleteFile(entry->fe_name,handle,delete_cmd,(entry->fe_entry)?1:0);

				    // Increment lmsg count
				    ++try;

				    // Get error code
				    err_code=IoErr();

				    // Successful?
				    if (ret || err_code==ERROR_OBJECT_NOT_FOUND)
				    {
					    ret=1;
					    break;
				    }

				    // Object in use, less than 3 tries?
				    if (err_code==ERROR_OBJECT_IN_USE && try<3)
				    {
					    if (try == 1 && entry->fe_type>0 && !(entry->fe_flags&FUNCENTF_LINK))
					    {
					        BPTR lock;
					        DirBuffer *tmp_buf;

					        if (lock = Lock(entry->fe_name,ACCESS_READ))
					        {
						    // Get buffer list lock
						    lock_listlock(&GUI->buffer_list,TRUE);

						    // Locked by chached lister-buffer?
						    if (tmp_buf = lister_find_buffer(0,0,0,
										     lock,
										     0,0,
										     LISTER_BFPF_DONT_LOCK|LISTER_BFPF_ONLY_CACHE|LISTER_BFPF_DONT_MOVE|LISTER_BFPF_DONT_TEST))
						    {
						        // Free the buffer and try again
						        lister_free_buffer(tmp_buf);
						    }

						    // Free list lock
						    unlock_listlock(&GUI->buffer_list);

						    UnLock(lock);
					        }
					    }
					    else
					        // Wait for a tick and then try again
					        Delay(15);
					    ret=1;
					    continue;
				    }

				    // If we failed, skip all?
				    if (data->skip_all) break;

				    // Delete protected?
				    if (err_code==ERROR_DELETE_PROTECTED)
				    {
					    // Ask before unprotecting?
					    if (!data->unprotect_all)
					    {
						    // Build requester text
						    lsprintf(handle->func_work_buf,
							    GetString(&locale,MSG_DELETE_PROTECTED),
							    entry->fe_name);

						    // Display request
						    if (!(ret=function_request(
							    handle,
							    handle->func_work_buf,
							    0,
							    GetString(&locale,MSG_UNPROTECT),
							    GetString(&locale,MSG_UNPROTECT_ALL),
							    GetString(&locale,MSG_SKIP),
							    GetString(&locale,MSG_SKIP_ALL),
							    GetString(&locale,MSG_ABORT),0)) || ret==-1)
						    {
							    // Abort
							    ret=-1;
							    break;
						    }

						    // Skip?
						    else
						    if (ret==3)
						    {
							    ret=0;
							    break;
						    }

						    // Skip all
						    else
						    if (ret==4)
						    {
							    ret=0;
							    data->skip_all=1;
							    break;
						    }

						    // Unprotect all?
						    else
						    if (ret==2) data->unprotect_all=1;
					    }

					    // Try to unprotect file
					    if (ret=SetProtection(entry->fe_name,0)) ret=1;
				    }

				    // Another error
				    else
				    {
					    // Directory not empty with recursive filter on
					    if (err_code==ERROR_DIRECTORY_NOT_EMPTY &&
						    handle->got_filter==2)
					    {
						    // Skip
						    ret=0;
					    }

					    // Otherwise
					    else
					    if ((ret=function_error(handle,entry->fe_name,MSG_DELETING,err_code))==-1)
						    break;
				    }
			    }

			    // Restore current directory
			    if (org_dir)
				CurrentDir(org_dir);
			}

			// Successful?
			if (ret==1)
			{
			    if (path)
				entry->fe_flags|=FUNCENTF_REMOVE;
			    else
			    if (entry->fe_lock)
				function_filechange_delfile(handle, NULL, entry->fe_lock, entry->fe_name, NULL, 1);
			}
			else
			if (ret!=2) file_ok=0;
		}

		// Skip
		else file_ok=0;

		// Aborted?
		if (ret==-1)
		{
aborting:
			function_abort(handle);
			ret=0;
			break;
		}

		// Get next entry, increment count
		count+=function_end_entry(handle,entry,file_ok);

		// Reset result code
		ret=1;
	}

	// Free timer
	FreeTimer(timer);

	return ret;
}


// Call the internal delete file function
long GalileoDeleteFile(char *name,FunctionHandle *handle,CommandList *cmd,BOOL orig)
{
	char *ptr;
	CommandList *command;
	ULONG flags=0;
	short ret;

	// Got handle?
	if (handle)
	{
		// Backup flags
		flags=handle->instruction_flags;

		// Set 'original' flag
		if (orig) handle->special_flags|=1;
		else handle->special_flags&=~1;
	}

	// Find the command if needed
	ptr=delfile;
	if (!(command=cmd) &&
		!(command=function_find_internal(&ptr,0)))
	{
		// Failed; call internal function directory
		ret=function_deletefile(0,handle,name,0);
	}

	// Got command
	else
	{
		// Run the command
		ret=function_internal_command(command,name,handle,0);
	}

	// Restore flags if we have a handle
	if (handle) handle->instruction_flags=flags;
	return ret;
}


// Standard internal delete function
GALILEOFM_FUNC(function_deletefile)
{
	char *ptr;
	short ret;

	// .info file?
	if ((ptr=sufcmp(args,".info")) && strlen(FilePart(args))<31)
	{
		// Strip .info
		*ptr=0;

		// Delete icon
		ret=DeleteDiskObject(args);

		// Restore .info
		*ptr='.';
	}

	// Call original function?
	else
	if (handle && handle->special_flags&1)
	{
		// Delete with original function
		ret=OriginalDeleteFile(args);
	}

	// Delete normal file, allow patch to be used
	else ret=DeleteFile(args);

	return ret;
}
