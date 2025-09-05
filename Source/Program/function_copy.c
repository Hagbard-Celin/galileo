/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023,2025 Hagbard Celine

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

#include "function_copy_include.h"

// COPY, etc internal function
GALILEOFM_FUNC(function_copy)
{
	FunctionEntry *entry;
	PathNode *path,*source;
	char *old_name=0,*old_name_edit,*new_name_edit;
	char *password=0,*password_buf;
	BOOL move_flag=0,rename_flag=0,link_flag=0,no_move_rename=0,source_same=0,dragdrop=0;
	BOOL change_info=0;
	ULONG copy_flags,rec_size=0;
	CopyData *data;
	BPTR org_dir = 0;
	char dest_file[108], source_no_icon[108], new_name[108];
	short ret=1,dest_count=0,function,count;

	// Get function
	function=command->function;
	copy_flags=environment->env->settings.copy_flags;

	// Icon copy?
	if (environment->env->desktop_flags&DESKTOPF_SMART_ICONCOPY)
		copy_flags|=COPYF_ICON_COPY;

	// Set flags to allow recursive directories (except for makelink)
	if (function!=FUNC_MAKELINK && function!=FUNC_MAKELINKAS &&
    	function!=FUNC_MMAKELINK && function!=FUNC_MMAKELINKAS)
		handle->instruction_flags=INSTF_RECURSE_DIRS|INSTF_WANT_DIRS|INSTF_WANT_DIRS_END;

	// Making links
	else
		link_flag=1;

	// If moving, clear sizes
	if (function==FUNC_MOVE || function==FUNC_MOVEAS)
	{
		handle->instruction_flags|=INSTF_DIR_CLEAR_SIZES;
		move_flag=1;
	}

	// Get source path
	source=function_path_current(&handle->func_source_paths);

	// From icon drag & drop?
	if (handle->flags&FUNCF_DRAG_DROP && handle->flags&FUNCF_ICONS && !(handle->flags&FUNCF_COPY_NO_MOVE))
	{
		// Set flag
		dragdrop=1;

		// See if paths are the same device
		if (SameLock(handle->func_source_lock, handle->func_dest_lock) == LOCK_SAME_VOLUME)
		{
			// Change operation to move
			handle->instruction_flags|=INSTF_DIR_CLEAR_SIZES;
			move_flag=1;
		}

		// Icon copy
		copy_flags|=COPYF_ICON_COPY;
	}

	// Copy or CopyAs, with Move flag set?
	else
	if ((function==(FUNC_COPY || FUNC_MCOPY) && instruction->ipa_funcargs && instruction->ipa_funcargs->FA_Arguments[COPY_MOVESAME]) ||
		(function==(FUNC_COPYAS || FUNC_MCOPYAS) && instruction->ipa_funcargs && instruction->ipa_funcargs->FA_Arguments[COPYAS_MOVESAME]))
	{
		// See if paths are the same device
		if (SameLock(handle->func_source_lock, handle->func_dest_lock) == LOCK_SAME_VOLUME)
		{
			// Change operation to move
			handle->instruction_flags|=INSTF_DIR_CLEAR_SIZES;
			move_flag=1;
		}
	}		

	// Go through destination paths
	while (path=function_path_next(&handle->func_dest_paths))
	{
		short invalid=0,ret;

		if (!path->pn_lock)
		{
		    if (path->pn_path && path->pn_path[0] && (path->pn_lock = LockFromPath(path->pn_path, NULL, FALSE)))
		    {
			path->pn_flags |= LISTNF_MADE_LOCK;
		    }
		    else
		    {
			function_error_text(handle,-1);
			path->pn_flags |= LISTNF_INVALID;
		    }
		}

		// See if the paths are the same
		if ((ret = SameLock(handle->func_source_lock, handle->func_dest_lock)) == LOCK_SAME)
		{
			// Mark as the same
			path->pn_flags|=LISTNF_SAME;

			// If not clone or *as, we can't do this
			if (function!=FUNC_CLONE &&
			    function!=FUNC_MOVEAS &&
			    function!=FUNC_COPYAS &&
			    function!=FUNC_MCOPYAS &&
			    function!=FUNC_MAKELINKAS &&
			    function!=FUNC_MMAKELINKAS)
			{
				// Don't show requester for drag & drop
				if (!dragdrop)
				{
					// Put up error requester
					function_request(
						handle,
						GetString(&locale,(function==FUNC_MAKELINK||function==FUNC_MMAKELINK)?MSG_CANT_OVERLINK_FILES:MSG_CANT_OVERCOPY_FILES),
						0,
						GetString(&locale,MSG_CONTINUE),0);
				}

				// Flag this lister as invalid
				invalid=1;
			}

			// Otherwise, set flag
			else source_same=1;
		}

		// If valid
		if (!invalid)
		{
			// Tell this lister to update it's datestamp at the end
			path->pn_flags|=LISTNF_UPDATE_STAMP;

			// Increment count
			++dest_count;
		}

		// Done with this path
		function_path_end(handle,&handle->func_dest_paths,invalid);
	}

	// If we don't have any destinations, return
	if (function!=FUNC_CLONE && dest_count==0)
	{
		// DisplayBeep for drag & drop
		if (dragdrop) DisplayBeep(GUI->screen_pointer);
		return 0;
	}

	// Turn progress indicator on
	function_progress_on(
		handle,
		GetString(&locale,
			((function==FUNC_ENCRYPT)||(function==FUNC_MENCRYPT))?MSG_PROGRESS_OPERATION_ENCRYPTING:
				((link_flag)?MSG_PROGRESS_OPERATION_LINKING:
					((move_flag)?MSG_PROGRESS_OPERATION_MOVING:
					((function==FUNC_CLONE)?MSG_PROGRESS_OPERATION_CLONING:MSG_PROGRESS_OPERATION_COPYING)))),
		handle->entry_count,
		PWF_FILENAME|PWF_FILESIZE|PWF_INFO|PWF_GRAPH);

	// Get data pointers
	data=(CopyData *)handle->inst_data;
	new_name_edit=data->func.copy.new_name_edit;
	old_name_edit=data->func.copy.old_name_edit;
	password_buf=data->func.encrypt.password;

	// First time through?
	if (!data->valid)
	{
		data->confirm_each=0;
		data->valid=1;
	}

	// Args?
	if (instruction->ipa_funcargs)
	{
		short arg=0;

		// Check for quiet
		if (function==FUNC_CLONE)
			arg=CLONE_QUIET;
		else
		if (function==FUNC_COPY || function==FUNC_MCOPY || function==FUNC_MOVE || function==FUNC_MAKELINK || function==FUNC_MMAKELINK)
			arg=COPY_QUIET;
		else
		if (function==FUNC_COPYAS || function==FUNC_MCOPYAS || function==FUNC_MOVEAS || function==FUNC_MAKELINKAS || function==FUNC_MMAKELINKAS)
			arg=COPYAS_QUIET;
		else
		if (function==FUNC_ENCRYPT || function==FUNC_ENCRYPT)
			arg=ENCRYPT_QUIET;

		// Quiet?		
		if (instruction->ipa_funcargs->FA_Arguments[arg])
			data->confirm_each=COPYF_DELETE_ALL|COPYF_UNPROTECT_ALL;

		// Copy?
		if (function==(FUNC_COPY || FUNC_MCOPY))
		{
			// Update?
			if (instruction->ipa_funcargs->FA_Arguments[COPY_UPDATEC]) copy_flags|=COPY_UPDATE_COPY;

			// Newer?
			else
			if (instruction->ipa_funcargs->FA_Arguments[COPY_NEWER]) copy_flags|=COPY_NEWER_COPY;
		}
	}

	// Source=destination or Move?
	if (move_flag || source_same)
	{
		// Tell source to update its datestamp
		source->pn_flags|=LISTNF_UPDATE_STAMP;
	}

	// Initialise file count
	count=1;

	// Moving?
	if (move_flag)
	{
		// Initialise progress bar for move
		function_progress_file(handle,2,0);
		function_progress_file(handle,0,2);

		// See if source is the desktop
		if (source && source->pn_lock && HookMatchLockDesktop(source->pn_lock))
			handle->flags|=FUNCF_RESCAN_DESKTOP;
	}

	// Build info string
	function_build_info(handle,0,0,3);

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BOOL file_ok=1;
		BPTR source_lock = 0;

		// Get source dir lock
		if (entry->fe_lock)
		    source_lock = entry->fe_lock;
	        else
		if (source->pn_lock)
		    source_lock	= source->pn_lock;
	        else
		{
		    ret = -1;
		    break;
		}

		if (handle->flags&FUNCF_DRAG_DROP)
		{
		    char *tmp_path;
		    BPTR tmp;

		    // Examine
		    Examine(source_lock, handle->s_info);

		    if (tmp = ParentDir(source_lock))
			UnLock(tmp);

		    if (tmp_path = JoinString(handle->memory, handle->s_info->fib_FileName, (tmp)?"":":", NULL, NULL))
		    {
			if (handle->func_source_path)
			    FreeMemH(handle->func_source_path);

			handle->func_source_path = tmp_path;
		    }
		}

		// Aborted?
		if (function_check_abort(handle))
		{
		    ret = -1;
		    break;
		}

		// CD to source dir
		org_dir = CurrentDir(source_lock);

///		Is source a device (in drag'n'drop operation)?
		if (!(entry->fe_flags&FUNCENTF_EXITED) &&
		    entry->fe_type == 0 && handle->flags&FUNCF_DRAG_DROP)
		{
			BPTR lock;

			// See if source is the same device
			if (move_flag &&
			    SameLock(source_lock,handle->func_dest_lock) == LOCK_SAME_VOLUME)
			{
				// Skip over this entry
				DisplayBeep(GUI->screen_pointer);
				count+=function_end_entry(handle,entry,0);
				continue;
			}

			// Duplicate examine source path lock
			if (lock = DupLock(source_lock))
			{
				struct DiskObject *icon;

				// Examine
				Examine(lock,handle->d_info);

				UnLock(lock);

				// Get disk icon
				if (!(icon=GetDiskObject("Disk")))
				{
					// Use default drawer icon
					icon=GetDefDiskObject(WBDRAWER);
				}

				// Got icon?
				if (icon)
				{
					// Change icon type
					icon->do_Type=WBDRAWER;

					// Try and get position
					copy_icon_position(handle,entry->fe_name,icon);
				}

				// Get destination path
				if (path=function_path_current(&handle->func_dest_paths))
				{
					char *tmp;

					CurrentDir(path->pn_lock);

					tmp = stpcpy(handle->func_work_buf,entry->fe_name);
					tmp--;
					*tmp = 0;

					// Create directory
					if (!(lock=OriginalCreateDir(handle->func_work_buf)))
					{   LONG err;
					    err = IoErr();

					    if (err == ERROR_OBJECT_EXISTS || err == ERROR_OBJECT_IN_USE)
						lock = Lock(handle->func_work_buf,ACCESS_READ);
					}
					else
					    ChangeMode(CHANGE_LOCK,lock,ACCESS_READ);

					if (lock)
					{
						// Examine this directory
						Examine(lock,handle->s_info);

						// Write icon
						if (icon)
						{
							BPTR dest_icon;
							ULONG len;
							BOOL dest_info = FALSE;

							len = strlen(handle->func_work_buf);
							strcpy(handle->func_work_buf + len,".info");
							if (dest_icon = Lock(handle->func_work_buf,ACCESS_READ))
							{
							    dest_info = TRUE;
							    UnLock(dest_icon);
							}
							handle->func_work_buf[len] = 0;
							//  feil requester
							//if (dest_info)
							//if (!(ret=function_error(handle,FilePart(handle->func_work_buf),MSG_COPYING,ERROR_OBJECT_EXISTS)) ||
							//		  ret==-1) break;

							if (!dest_info)
							{
							    // Write icon
							    if (PutDiskObject(handle->func_work_buf,icon))
							    {
								    // Add to listers
								    function_filechange_loadfile(
									    handle,
									    path->pn_path,
									    path->pn_lock,
									    handle->func_work_buf,
									    FFLF_ICON);
							    }
							}
						}

						// Add new directory
						function_filechange_addfile(handle,path->pn_path,path->pn_lock,handle->s_info,0,0);
						{
						    char *tmp_path;

						    if (path->pn_path_buf && path->pn_path_buf[0])
							tmp_path = JoinString(handle->memory, path->pn_path_buf, handle->func_work_buf, NULL, JSF_FS_SLASH|JSF_E_SLASH);
						    else
							tmp_path = JoinString(handle->memory, handle->func_work_buf, "/", NULL, NULL);

						    if (tmp_path)
						    {
							if (path->pn_path_buf)
							    FreeMemH(path->pn_path_buf);

							path->pn_path_buf = tmp_path;
						    }
						}

						{
						    char *tmp_path;

						    if (tmp_path = CopyString(handle->memory,path->pn_path_buf))
						    {
							if (handle->func_dest_path)
							    FreeMemH(handle->func_dest_path);

							handle->func_dest_path = tmp_path;
						    }
						}

						path->pn_lock = lock;
						path->pn_flags|=LISTNF_CHANGED;
						change_info = 1;
					}

					CurrentDir(source_lock);
				}

				// Free icon
				FreeDiskObject(icon);
			}
		}
///

		// Update progress indicator
		if (function_progress_update(handle,entry,count))
		{
			ret = -1;
			break;
		}

		if (entry->fe_type == 0)
		{
		    char *tmp_path;

		    if (entry->fe_flags&FUNCENTF_MULTI_ASSIGN && !(entry->fe_flags&FUNCENTF_EXITED))
		    {

			if (tmp_path = JoinString(handle->memory, handle->d_info->fib_FileName, ":", NULL, NULL))
		        {
			    if (handle->func_source_path)
				FreeMemH(handle->func_source_path);

			    handle->func_source_path = tmp_path;
		        }
		    }
		    // This is the new source path
		    else
		    if (tmp_path = CopyString(handle->memory, entry->fe_name))
	            {
		        if (handle->func_source_path)
			    FreeMemH(handle->func_source_path);

		        handle->func_source_path = tmp_path;
	            }
		}

		{
		    char *tmp;

		    // Get destination filename
		    tmp = stpcpy(dest_file,entry->fe_name);

		    // Strip ':' from end of name
		    if (entry->fe_type == ENTRY_DIRECTORY && entry->fe_flags&FUNCENTF_ASSIGN)
		    {
			tmp--;
			*tmp = 0;
		    }
		}

		// Check this isn't an exited directory
		if (!(entry->fe_flags&FUNCENTF_EXITED))
		{
///			Copy as/Move as/MakeLink as/Clone?
			if (function==FUNC_COPYAS || function==FUNC_MCOPYAS ||
				function==FUNC_MOVEAS ||
				function==FUNC_MAKELINKAS || function==FUNC_MMAKELINKAS ||
				function==FUNC_CLONE)
			{
				// Not recurse entry?
				if (!(entry->fe_flags&FUNCENTF_RECURSE))
				{
					// An icon entry?
					if (entry->fe_flags&FUNCENTF_ICON)
					{
						// Get new name, tack a .info to the end
						strcpy(dest_file,new_name);
						strcat(dest_file,".info");
					}

					// Normal entry
					else
					{
						// Do we have to ask for a name?
						if (!data->func.copy.wild_copy)
						{
							// Name supplied in arguments?
							if (instruction->ipa_funcargs &&
								instruction->ipa_funcargs->FA_Arguments[COPYAS_NAME] &&
								instruction->ipa_funcargs->FA_Arguments[COPYAS_NEWNAME])
							{
								// Get names
								strcpy(old_name_edit,(char *)instruction->ipa_funcargs->FA_Arguments[COPYAS_NAME]);
								strcpy(new_name_edit,(char *)instruction->ipa_funcargs->FA_Arguments[COPYAS_NEWNAME]);
								ret=1;
							}

							// Otherwise
							else
							{
								// Ask for a new name
								strcpy(old_name_edit,entry->fe_name);
								strcpy(new_name_edit,entry->fe_name);
								if ((ret=function_request(
									handle,
									GetString(&locale,MSG_ENTER_NEW_NAME),
									SRF_BUFFER|SRF_BUFFER2|SRF_PATH_FILTER,
									new_name_edit,GUI->def_filename_length,
									old_name_edit,GUI->def_filename_length,
									GetString(&locale,
										(move_flag)?MSG_MOVE:
										((link_flag)?MSG_MAKELINK:
											((function==FUNC_CLONE)?MSG_DUPLICATE:MSG_COPY))),
									GetString(&locale,MSG_ABORT),
									GetString(&locale,MSG_SKIP),0))==2)
								{
									if (org_dir)
									    CurrentDir(org_dir);

									function_abort(handle);
									ret=0;
									break;
								}

								// Turn -1 into 1
								else
								if (ret==-1) ret=1;
							}

							// If name is the same (for clone), skip
							if (function==FUNC_CLONE &&
								stricmp(old_name_edit,new_name_edit)==0)
							{
								ret=0;
							}

							// Skip?
							if (ret==0) file_ok=0;

							// Otherwise
							else
							{
								// Does the new name contain wildcards?
								if (strchr(new_name_edit,'*'))
								{
									// If the old name doesn't, make it a *
									if (!(strchr(old_name_edit,'*')))
										strcpy(old_name_edit,"*");

									// Set wildcard flag
									data->func.copy.wild_copy=1;
								}

								// No wildcards
								else
								{
									// Store new name
									strcpy(dest_file,new_name_edit);
								}
							}
						}

						// Wildcard copy?
						if (data->func.copy.wild_copy)
						{
							// Get new name
							file_ok=rename_get_wild(
								old_name_edit,
								new_name_edit,
								entry->fe_name,
								dest_file);
						}

						// Still ok?
						if (file_ok)
						{
							// Get old and new names
							old_name=entry->fe_name;
							strcpy(new_name,dest_file);
						}
					}
				}

				// Otherwise, make change to first part of destination
				else
				if (old_name) strreplace(dest_file,old_name,new_name,0);
			}
///

///			Encrypt?
			else
			if (function==FUNC_ENCRYPT || function==FUNC_MENCRYPT)
			{
				// Have to ask for a password?
				if (!password)
				{
					// Password supplied in arguments?
					if (instruction->ipa_funcargs &&
						instruction->ipa_funcargs->FA_Arguments[ENCRYPT_PASSWORD])
					{
						// Copy password
						stccpy(password_buf,(char *)instruction->ipa_funcargs->FA_Arguments[ENCRYPT_PASSWORD],24);
					}

					// Ask for a password
					else
					if (!(function_request(
						handle,
						GetString(&locale,MSG_ENTER_PASSWORD),
						SRF_BUFFER|SRF_CHECKMARK,
						password_buf,24,
						GetString(&locale,MSG_DECRYPT),&data->func.encrypt.decrypt,
						GetString(&locale,MSG_OKAY),
						GetString(&locale,MSG_ABORT),0)) || !password_buf[0])
					{
						if (org_dir)
						    CurrentDir(org_dir);

						if (path->pn_recurse_lock)
						    UnLock(path->pn_recurse_lock);

						if (entry->fe_flags&FUNCENTF_RECURSE)
						    UnLock(entry->fe_lock);

						function_abort(handle);
						ret=0;
						break;
					}

					// Get password pointer
					password=password_buf;

					// Decrypt?
					if (data->func.encrypt.decrypt) copy_flags|=ENCRYPT_DECRYPT;
					else copy_flags&=~ENCRYPT_DECRYPT;
				}
			}
///
		}

		// Exited directory
		else
		{
			// Exited a device (in drag'n'drop operation)?
			if (entry->fe_type == 0 && handle->flags&FUNCF_DRAG_DROP)
			{
				// Get destination path
				if (path=function_path_current(&handle->func_dest_paths))
				{
					if (path->pn_flags&LISTNF_CHANGED)
					{
					    UnLock(path->pn_lock);
					    path->pn_lock = 0;
					}

					// Clear path flag
					path->pn_flags&=~LISTNF_CHANGED;
				}
			}
		}

///		Top-level entry?
		if (entry->fe_flags&FUNCENTF_TOP_LEVEL)
		{
			// Exited directory?
			if (source && entry->fe_flags&FUNCENTF_EXITED)
			{
				// Add size update
				function_filechange_modify(
					handle,
					source->pn_path,
					source->pn_lock,
					entry->fe_name,
					FM_Size,rec_size,
					TAG_END);
				rec_size=0;
			}
		}
///

		// File within a directory?
		else
		if (entry->fe_type<0)
		{
			// Increment size count
			rec_size+=entry->fe_size;
		}

		// Ok to do this file?
		if (file_ok)
		{
			// If clone, use source path
			if (function==FUNC_CLONE) path=source;

			// Otherwise, get destination path
			else path=function_path_next(&handle->func_dest_paths);

			// Go through destination paths
			while (path)
			{
				BPTR dest_lock = 0;
				if (path->pn_recurse_lock)
				    dest_lock = path->pn_recurse_lock;
				else
				if (path->pn_lock)
				    dest_lock = path->pn_lock;
				else
				{
				    ret = -1;
				    break;
				}

				// Get destination path
				if (path && path->pn_path && path->pn_path[0])
				{
				    char *tmp_path;

				    if (tmp_path = CopyString(handle->memory, path->pn_path))
				    {
					if (handle->func_dest_path)
					    FreeMemH(handle->func_dest_path);

					handle->func_dest_path = tmp_path;
				    }
				}
				// If there's multiple destinations, or flagged, change info
				if (dest_count>1 || change_info)
				{
					if (entry->fe_flags&FUNCENTF_RECURSE)
					    function_build_info(handle,handle->anchor_path,handle->func_dest_path,3);
					else
					    function_build_info(handle,handle->func_source_path,handle->func_dest_path,3);

					change_info=0;
				}

				// Check this isn't an exited directory
				if (!(entry->fe_flags&FUNCENTF_EXITED))
				{
					BOOL ok=1;

					// Is this a directory?
					if (entry->fe_type>=0)
					{
						// Set flag to change info
						change_info=1;

						// Is this a top-level directory?
						if (entry->fe_flags&FUNCENTF_TOP_LEVEL)
						{
						    BPTR lock;

						    if (lock = Lock(entry->fe_name,ACCESS_READ))
						    {
							// Check we're not trying to copy it into itself
							if(SameLock(dest_lock,lock) == LOCK_SAME)
							{
								// Put up error requester
								if (!(function_request(
									handle,
									GetString(&locale,(link_flag)?MSG_CANT_LINK_DIR_INTO_ITSELF:MSG_CANT_COPY_DIR_INTO_ITSELF),
									0,
									GetString(&locale,MSG_CONTINUE),
									GetString(&locale,MSG_ABORT),0))) ret=-1;
								else ret=0;
								ok=0;
							}
							UnLock(lock);
						    }
						}
					}

					// Update/Newer?
					else
					if (copy_flags&(COPY_UPDATE_COPY|COPY_NEWER_COPY))
					{
						struct FileInfoBlock __aligned fib;

						// See if destination exists
						if (GetFileInfo(dest_file,&fib))
						{
							// If copying newer, check date
							if (copy_flags&COPY_NEWER_COPY)
							{
								struct FileInfoBlock __aligned fib2;

								// Get source info
								if (GetFileInfo(entry->fe_name,&fib2))
								{
									// Compare file dates
									if (CompareDates(&fib2.fib_Date,&fib.fib_Date)>=0)
									{
										// Don't copy
										ok=0;
										ret=0;
									}
								}
							}

							// If copy update, don't copy at all if file exists
							else
							{
								// Clear ok flag so file won't be copied
								ok=0;
								ret=0;
							}
						}
					}

					// Check destination is ok to write to
					if (ok &&
						(ret=check_file_destination(handle,entry,dest_file,dest_lock,&data->confirm_each))==1)
					{
						long error=0;

						// If we're moving, try rename, unless this is an icon
						if (move_flag && (!(entry->fe_flags&FUNCENTF_ICON) || !(copy_flags&COPYF_ICON_COPY)))
						{
							// Can only rename if a file, or no recursive filter set
							if (!no_move_rename && (entry->fe_type<0 || handle->got_filter!=2))
							{
								if (source_lock && dest_lock)
								{
								    BPTR assign_lock_source, assign_lock_dest;
								    char rename_source[124], rename_dest[124];

								    sprintf(rename_source, "gars_%08p",entry);
								    sprintf(rename_dest, "gard_%08p",entry);

								    assign_lock_source = DupLock(source_lock);
								    assign_lock_dest = DupLock(dest_lock);

								    AssignLock(rename_source,assign_lock_source);
								    AssignLock(rename_dest,assign_lock_dest);

								    rename_source[12] = ':';
								    rename_source[13] = 0;
								    rename_dest[12] = ':';
								    rename_dest[13] = 0;

								    AddPart(rename_source,entry->fe_name,123);
								    AddPart(rename_dest,dest_file,123);

								    if (error=OriginalRename(rename_source,rename_dest))
								    {
									    BPTR lock;

									    CurrentDir(dest_lock);

									    if (lock=Lock(dest_file,ACCESS_READ))
									    {
										    Examine(lock,handle->d_info);
										    UnLock(lock);
									    }

									    CurrentDir(source_lock);

								    }

								    // Different devices?
								    else
								    if (IoErr()==ERROR_RENAME_ACROSS_DEVICES)
									    no_move_rename=1;

								    rename_source[12] = 0;
								    rename_dest[12] = 0;
								    AssignLock(rename_source,NULL);
								    AssignLock(rename_dest,NULL);

								}
								else
								{
								    // Try rename
								    if (error=OriginalRename(entry->fe_name,dest_file))
								    {
									    BPTR lock;

									    if (lock=Lock(dest_file,ACCESS_READ))
									    {
										    Examine(lock,handle->d_info);
										    UnLock(lock);
									    }
								    }

								    // Different devices?
								    else
								    if (IoErr()==ERROR_RENAME_ACROSS_DEVICES)
									    no_move_rename=1;
								}
							}
						}

						// Or linking?
						else
						if (link_flag)
						{
							BPTR lock=0;

							// Loop while unsuccessful
							while (1)
							{
								// Clear error code
								error=0;

								// No source lock?
								if (!lock)
								{
									// Lock source
									if (!(lock=Lock(entry->fe_name,ACCESS_READ)))
										error=IoErr();
								}

								CurrentDir(dest_lock);

								// Try to make link
								if (lock &&
									!(MakeLink(dest_file,lock,FALSE))) error=IoErr();

								// Successful?
								if (!error)
								{
									BPTR dest;

									// Lock new destination
									if (dest=Lock(dest_file,ACCESS_READ))
									{
										// Examine it
										Examine(dest,handle->d_info);
										UnLock(dest);

										// Fix type to indicate link
										if (handle->d_info->fib_DirEntryType<0)
											handle->d_info->fib_DirEntryType=ST_LINKFILE;
										else handle->d_info->fib_DirEntryType=ST_LINKDIR;

										// Use new filename
										strcpy(handle->d_info->fib_FileName,FilePart(dest_file));
										ret=1;
									}

									CurrentDir(source_lock);

									break;
								}

								// Failed
								else
								{
									// Show error
									if (!(ret=function_error(
										handle,
										entry->fe_name,
										MSG_LINKING,
										IoErr())) || ret==-1)
									{
									    CurrentDir(source_lock);
									    break;
									}

									CurrentDir(source_lock);
								}
							}

							// Unlock lock if we had one
							UnLock(lock);
							error=0;
						}

						// Was rename successful?
						if (error)
						{
							ret=1;

							// Did we move a whole directory tree?
							if (entry->fe_flags&FUNCENTF_ENTERED)
							{
								entry->fe_flags|=FUNCENTF_REMOVE;
								rename_flag=1;
							}
						}

						// Try to copy the file
						else
						if (!link_flag)
						{
							BOOL assign_icon = FALSE;
							
							if (entry->fe_type == ENTRY_FILE && entry->fe_flags&FUNCENTF_ASSIGN)
							    assign_icon = TRUE;

							// Top-level entry?
							if (entry->fe_flags&FUNCENTF_TOP_LEVEL)
								copy_flags|=COPY_TOP_LEVEL;
							else copy_flags&=~COPY_TOP_LEVEL;

							// Try to copy file
							while (!(ret=function_copy_file(
								handle->s_info,
								handle->d_info,
								handle,
								(assign_icon)?handle->last_filename:entry->fe_name,
								dest_file,
								source_lock,
								dest_lock,
								&error,
								password,
								(assign_icon)?copy_flags|COPY_ASSIGN_ICON:copy_flags)))
							{
								// Not found?
								if (error==ERROR_OBJECT_NOT_FOUND)
								{
									// If this is an icon, just ignore it
									if (entry->fe_flags&FUNCENTF_ICON)
									{
										ret=-2;	// 0
										break;
									}

									// Icon operation?
									if (handle->flags&FUNCF_ICONS && !(isicon(entry->fe_name)))
									{
										BPTR lock;

										// See if file has an icon on disk
										strcpy(handle->func_work_buf, entry->fe_name);
										strcat(handle->func_work_buf, ".info");
										if (lock=Lock(handle->func_work_buf,ACCESS_READ))
											UnLock(lock);

										// If icon exists, skip over main file
										if (lock)
										{
											ret=-2;	
											break;
										}
									}
								}

								// Display error
								if (!(ret=function_error(handle,entry->fe_name,MSG_COPYING,error)) ||
									ret==-1)
								{
									break;
								}
							}
							if (error == -42)
							{
								BPTR tmp_dest_lock;

								if (entry->fe_type == ENTRY_DIRECTORY && entry->fe_flags&FUNCENTF_ASSIGN)
								{
								    char *tmp_name;

								    if (tmp_name = JoinString(handle->memory, handle->s_info->fib_FileName, ".info", NULL, NULL))
								    {
									if (handle->last_filename)
									    FreeMemH(handle->last_filename);

									handle->last_filename = tmp_name;
								    }
								}

								CurrentDir(dest_lock);

								tmp_dest_lock = Lock(handle->d_info->fib_FileName, ACCESS_READ);

								CurrentDir(source_lock);

								if (path->pn_recurse_lock)
								    UnLock(path->pn_recurse_lock);

								path->pn_recurse_lock = tmp_dest_lock;
							}
							else
							if (assign_icon)
							{
							    if (handle->last_filename)
								FreeMemH(handle->last_filename);
							    handle->last_filename = 0;
							}
						}
					}

					// Aborted?
					if (ret==-1) break;

					// Success?
					if (ret>0)
					{
						// If a top-level entry, add to destination list
						if (entry->fe_flags&FUNCENTF_TOP_LEVEL)
						{
							// Add file
							function_filechange_addfile(handle,path->pn_path,dest_lock,handle->d_info,0,path->pn_lister);

							// Do changes
							function_filechange_do(handle,1);
						}

						// Update free space?
						if (copy_flags&COPY_UPDATE)
						{
							// Valid lister?
							if (path->pn_lister && path->pn_lister->window)
							{
								// Send update message
								IPC_Command(path->pn_lister->ipc,LISTER_REFRESH_NAME,0,0,0,0);
							}
						}

						// Update archive bit status in lister entry?
						if (copy_flags&COPY_ARC &&
							!(entry->fe_flags&FUNCENTF_ICON_ACTION) &&
							entry->fe_entry)
						{
							// Get new protection bits
							entry->fe_entry->de_Protection=handle->s_info->fib_Protection;
							protect_get_string(entry->fe_entry->de_Protection,entry->fe_entry->de_ProtBuf);
						}
					}

					// Skipped over file, still want to do icon?
					else
					if (ret==-2) ret=0;

					// File has failed (as far as deselecting goes)
					else file_ok=0;

					// Aborted?
					if (function_check_abort(handle))
					{
					    ret = -1;
					    break;
					}
				}

				// Exited directory
				else
				{
				    if (entry->fe_flags&FUNCENTF_RECURSE)
				    {
					BPTR tmp_lock = path->pn_recurse_lock;

					path->pn_recurse_lock = ParentDir(tmp_lock);
					UnLock(tmp_lock);
				    }
				    else
				    {
					if (path->pn_recurse_lock)
					{
					    UnLock(path->pn_recurse_lock);
					    path->pn_recurse_lock = 0;
					}
				    }

				    // Set flag to change info
				    change_info=1;
				}

				// If clone, break out
				if (function==FUNC_CLONE) break;

				// Aborted?
				if (function_check_abort(handle))
				{
				    ret = -1;
				    break;
				}

				// Done with this path, get next
				function_path_end(handle,&handle->func_dest_paths,0);
				path=function_path_next(&handle->func_dest_paths);
			}

			// Aborted?
			if (ret==-1)
			{
				function_abort(handle);
				ret=0;
				break;
			}
		}

		// Need to delete source?
		if (move_flag &&
			file_ok &&
			!rename_flag &&
			(entry->fe_type<0 || entry->fe_flags&FUNCENTF_EXITED))
		{
			BOOL icon=0;
			char *ptr;
			short suc = 0, try = 0;

			// Copy name
			strcpy(source_no_icon,entry->fe_name);

			// See if file is an icon
			if (ptr=isicon(source_no_icon))
			{
				// Strip .info
				*ptr=0;
				icon=1;
			}

			// Set result code
			ret=1;

			// Loop while unsuccessful
			while (!suc)
			{
				short err_code;

				// Delete file (or icon)
				if (icon) suc=DeleteDiskObject(source_no_icon);
				else suc=DeleteFile(entry->fe_name);

				// Successful?
				if (suc) break;

				try++;

				// Get error code
				err_code=IoErr();

				// If object not found, it's already deleted
				if (err_code==ERROR_OBJECT_NOT_FOUND)
					break;

			        // Object in use, less than 3 tries?
				else
				if (err_code==ERROR_OBJECT_IN_USE && try<3)
			        {
				        if (try == 1 && entry->fe_type>0 && !(entry->fe_flags&FUNCENTF_LINK))
				        {
				            BPTR lock;
				            DirBuffer *tmp_buf;

				            if (lock = Lock(entry->fe_name,ACCESS_READ))
				            {
					        // Locked by cached lister-buffer?
					        if (tmp_buf = lister_find_buffer(0,0,0,
									         lock,
									         0,0,
									         LISTER_BFPF_ONLY_CACHE|LISTER_BFPF_DONT_MOVE|LISTER_BFPF_DONT_TEST))
					        {
					            // Free the buffer and try again
					            lister_free_buffer(tmp_buf);
					        }

					        UnLock(lock);
				            }
				        }
					// Wait for a tick and then try again
				        else
				            Delay(15);

					continue;
			        }

				// Delete protected
				else
				if (err_code==ERROR_DELETE_PROTECTED)
				{
					// Ask before unprotecting?
					if (!(handle->instruction_flags&INSTF_DELETE_UNPROTECT))
					{
						// Build requester text
						lsprintf(handle->func_work_buf,
							GetString(&locale,MSG_DELETE_PROTECTED),
							FilePart(entry->fe_name));

						// Display request
						if (!(ret=function_request(
							handle,
							handle->func_work_buf,
							0,
							GetString(&locale,MSG_UNPROTECT),
							GetString(&locale,MSG_UNPROTECT_ALL),
							GetString(&locale,MSG_ABORT),
							GetString(&locale,MSG_SKIP),0))) break;

						// Abort?
						if (ret==3)
						{
							ret=-1;
							break;
						}

						// Unprotect all?
						if (ret==2)
						{
							handle->instruction_flags|=INSTF_DELETE_UNPROTECT;
							ret=1;
						}
					}

					// Try to unprotect file
					if (!(SetProtection(entry->fe_name,0)))
					{
						ret=0;
						break;
					}
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
						break;
					}

					// Otherwise
					else
					if (!(ret=function_error(handle,entry->fe_name,MSG_DELETING,err_code)) ||
						ret==-1) break;
				}
			}

			// Successful?
			if (ret==1) entry->fe_flags|=FUNCENTF_REMOVE;
			else file_ok=0;
		}

		if (org_dir)
		{
		    CurrentDir(org_dir);
		    org_dir = 0;
		}

		// Aborted?
		if (ret==-1 || function_check_abort(handle))
		{
			ret = -1;
			break;
		}

		// Get next entry, increment count
		count+=function_end_entry(handle,entry,file_ok);

		// Reset result code
		ret=1;
		rename_flag=0;
	}

	// Aborted?
	if (ret==-1)
	{
		if (org_dir)
		    CurrentDir(org_dir);

		if (path && path->pn_recurse_lock)
		{
		    UnLock(path->pn_recurse_lock);
		    path->pn_recurse_lock = 0;
		}

		function_abort(handle);
		ret=0;
	}

	return ret;
}


// Copy a file, with optional encryption
static int function_copy_file(
	struct FileInfoBlock *s_info,
	struct FileInfoBlock *d_info,
	FunctionHandle *handle,
	char *source_file,
	char *dest_file,
	BPTR source_path_lock,
	BPTR dest_path_lock,
	long *err_code,
	char *password,
	ULONG copy_flags)
{
	BPTR lock;
	BPTR in_file,out_file=0;
	char *file_buffer,*initial_buffer;
	unsigned long buffer_size,total_size=0;
	long file_size;
	short ret_code=COPY_FAILED;
	char decrypt_flag=0;
	char got_dest_info=0;
	char *source_ptr,*dest_ptr;
	BOOL done=0;

	// Initialise error code
	*err_code=0;

	// Lock and examine source
	if (!(lock=Lock(source_file,ACCESS_READ)) ||
		!(Examine(lock,s_info)))
	{
		*err_code=IoErr();
		UnLock(lock);

		return COPY_FAILED;
	}

	// Unlock lock
	UnLock(lock);

	// Is a password specified?
	if (password)
	{
		int a,seed=1;

		// Decrypting?
		if (password[0]=='-')
		{
			++password;
			decrypt_flag=1;
		}
		else
		if (copy_flags&ENCRYPT_DECRYPT) decrypt_flag=1;

		// Go through password, build seed
		for (a=0;password[a];a++) seed*=password[a];

		// Seed random number generator
		Seed(seed);
	}

	// Copying an icon?
	else
	if (copy_flags&COPYF_ICON_COPY &&
		(source_ptr=isicon(source_file)) &&
		(dest_ptr=isicon(dest_file)))
	{
		struct DiskObject *icon;

		// Strip .info suffixes
		*source_ptr=0;
		*dest_ptr=0;

		// Set file size
		function_progress_file(handle,10,0);

		// Try to get icon
		if (icon=GetDiskObject(source_file))
		{
			if (copy_flags&COPY_ASSIGN_ICON)
			{
			    char *tmp;

			    tmp = stpcpy(handle->func_work_buf, dest_file);
			    *tmp = ':';
			    tmp++;
			    *tmp = 0;
			}

			// Update progress
			function_progress_file(handle,0,5);

			// Adjust position for top-level files
			if (copy_flags&COPY_TOP_LEVEL)
				copy_icon_position(handle,(copy_flags&COPY_ASSIGN_ICON)?handle->func_work_buf:(char *)FilePart(source_file),icon);

			// Check abort
			if (function_check_abort(handle))
				ret_code=COPY_ABORTED;

			// Try to write icon
			else
			{
		            CurrentDir(dest_path_lock);

			    if (PutDiskObject(dest_file,icon)) //(putdiskobject(&current_dir,dest_path_lock,&org_dir,dest_file,icon))
			    {
				function_progress_file(handle,0,10);
				ret_code=COPY_OK;
			    }

			    // Get error
			    else
				*err_code=IoErr();
			}

			// Free icon
			FreeDiskObject(icon);
		}

		// Restore .info's
		*source_ptr='.';
		*dest_ptr='.';

		// Did we have an icon?
		if (icon)
		{
			// Success?
			if (ret_code==COPY_OK)
			{
				// Lock destination file
				if (lock=Lock(dest_file,ACCESS_READ))
				{
					// Examine file
					if (Examine(lock,d_info))
						got_dest_info=1;
					UnLock(lock);
				}
			}

			// Mark as done
			done=1;
		}
	}

	// Not copied yet?
	if (!done)
	{
		// Is it a directory?
		if (s_info->fib_DirEntryType>0)
		{
		        CurrentDir(dest_path_lock);
			// Try to create destination directory
			if ((lock=Lock(dest_file,ACCESS_READ)) ||
				(lock=CreateDir(dest_file)))
			{
				// Get directory information
				Examine(lock,d_info);
				UnLock(lock);

				// Is destination a directory?
				if (d_info->fib_DirEntryType>0)
				{
					// Copy protection?
					if (copy_flags&COPY_PROT)
					{
						ULONG prot=s_info->fib_Protection&~FIBF_ARCHIVE;
						if (SetProtection(dest_file,prot))
							d_info->fib_Protection=prot;
					}

					// Copy comment
					if (copy_flags&COPY_NOTE)
					{
						if (SetComment(dest_file,s_info->fib_Comment))
							strcpy(d_info->fib_Comment,s_info->fib_Comment);
					}

					// Indicate a directory
					*err_code=-42;

					CurrentDir(source_path_lock);

					// Set success code
					return COPY_OK;
				}

				// Wrong type
				else *err_code=ERROR_OBJECT_WRONG_TYPE;
			}

			// Couldn't create
			else
			{
			    *err_code=IoErr();

			    CurrentDir(source_path_lock);
			}

			return COPY_FAILED;
		}

		CurrentDir(source_path_lock);

		// Check source isn't read-protected
		if (s_info->fib_Protection&FIBF_READ)
		{
			// It is; fail
			*err_code=ERROR_READ_PROTECTED;

			return COPY_FAILED;
		}

		// Open source file
		if (!(in_file=Open(source_file,MODE_OLDFILE)))
		{
			// Store error code
			*err_code=IoErr();

			return COPY_FAILED;
		}

		// Get file size
		file_size=s_info->fib_Size;

		// Set file size
		function_progress_file(handle,file_size*2,0);

		// Start with an 8K buffer
		buffer_size=COPY_INITIAL_BUFFER;

		// Allocate initial buffer
		initial_buffer=AllocVec(COPY_INITIAL_BUFFER,0);
		file_buffer=initial_buffer;

		// Got buffer?
		if (file_buffer)
		{
			CurrentDir(dest_path_lock);

			// Open output file; if top level, use original function
			if (copy_flags&COPY_TOP_LEVEL)
				out_file=OriginalOpen(dest_file,MODE_NEWFILE);

			// Otherwise allow patched function to be used
			else
				out_file=Open(dest_file,MODE_NEWFILE);

			// Got file?
			if (out_file)
			{
				ULONG copytime;

				// Initial copy time setting
				copytime=750000;

				// Loop while data remains to process
				while (file_size>0)
				{
					LONG read_size,write_size,old_buffersize,size;
					struct timeval start,end;
					BOOL new_buf=0;

					// Check abort
					if (function_check_abort(handle))
					{
						ret_code=COPY_ABORTED;
						break;
					}

					// Remember old buffer size
					old_buffersize=buffer_size;

					// Get smaller buffer?
					if (copytime>COPY_HIGH_THRESH)
					{
						// Halve buffer size
						buffer_size>>=1;

						// Minimum 2K
						if (buffer_size<COPY_MIN_BUFFER)
							buffer_size=COPY_MIN_BUFFER;

						// Try for smaller buffer
						if (buffer_size!=old_buffersize) new_buf=1;
					}

					// Or a larger one
					else
					if (copytime<COPY_LOW_THRESH)
					{
						// Double buffer size
						buffer_size<<=1;

						// Try for larger buffer
						new_buf=1;
					}

					// Want to try for new buffer?
					if (new_buf)
					{
						// Free existing buffer (unless it's the initial one)
						if (file_buffer!=initial_buffer)
							FreeVec(file_buffer);

						// Allocate new buffer
						if (!(file_buffer=AllocVec(buffer_size,0)))
						{
							// Failed, fall back to initial buffer
							file_buffer=initial_buffer;
							buffer_size=COPY_INITIAL_BUFFER;
						}
					}

					// Get size to read
					read_size=(file_size>buffer_size)?buffer_size:file_size;

					// Get current time
					GetSysTime(&start);

					// Read data
					if ((size=Read(in_file,file_buffer,read_size))<1)
						break;

					// Did we get back less than we asked for?
					if (size<read_size)
					{
						// Reduce file size by the difference
						file_size-=read_size-size;
					}

					// Save read size
					read_size=size;

					// Add to total
					total_size+=size;

					// Update file progress
					function_progress_file(handle,0,total_size);

					// Check abort
					if (function_check_abort(handle))
					{
						ret_code=COPY_ABORTED;
						break;
					}

					// Password supplied?
					if (password)
					{
						int a;
						char enbyte;

						// Encrypting?
						if (!decrypt_flag)
						{
							for (a=0;a<read_size;a++)
							{
								enbyte=Random(9999);
								file_buffer[a]+=enbyte;
							}
						}

						// Decrypting
						else
						{
							for (a=0;a<read_size;a++)
							{
								enbyte=Random(9999);
								file_buffer[a]-=enbyte;
							}
						}
					}

					// If top level, write with original function
					if (copy_flags&COPY_TOP_LEVEL)
						write_size=OriginalWrite(out_file,file_buffer,read_size);

					// Otherwise, use patched function
					else
						write_size=Write(out_file,file_buffer,read_size);

					// Failed?
					if (write_size<read_size) break;

					// Get current time
					GetSysTime(&end);

					// Calculate time that portion of the copy took
					SubTime(&end,&start);
					copytime=(end.tv_secs*1000000)+end.tv_micro;

					// Subtract from remaining size
					file_size-=write_size;

					// Add to total
					total_size+=write_size;

					// Update file progress
					function_progress_file(handle,0,total_size);
				}

				// Successful?
				if (file_size==0) ret_code=COPY_OK;
			}
		}

		// Store error code
		*err_code=IoErr();

		// Close in file
		if (in_file) Close(in_file);

		// Get destination file information
		if (out_file)
		{
			// Close out file; if top level, use original function
			if (copy_flags&COPY_TOP_LEVEL)
				OriginalClose(out_file);

			// Otherwise, allow patched function to be used
			else
				Close(out_file);

			// Re-lock out-file
			if (lock=Lock(dest_file,ACCESS_READ))
			{
				// Examine file
				if (Examine(lock,d_info))
					got_dest_info=1;
				UnLock(lock);
			}
		}

		// Free buffer memory
		if (file_buffer!=initial_buffer) FreeVec(file_buffer);
		FreeVec(initial_buffer);
	}

	// If successful, copy other things
	if (ret_code==COPY_OK)
	{
		// Copy date?
		if (copy_flags&COPY_DATE)
		{
			short res;

			// Set date; use original function if at top-level
			if (copy_flags&COPY_TOP_LEVEL)
				res=OriginalSetFileDate(dest_file,&s_info->fib_Date);

			// Otherwise, allow patched function to be used
			else
				res=SetFileDate(dest_file,&s_info->fib_Date);

			// Successful?
			if (res && got_dest_info) d_info->fib_Date=s_info->fib_Date;
		}

		// Copy protection?
		if (copy_flags&COPY_PROT)
		{
			short res;
			ULONG prot=s_info->fib_Protection&~FIBF_ARCHIVE;

			// Set protection; use original function if at top-level
			if (copy_flags&COPY_TOP_LEVEL)
				res=OriginalSetProtection(dest_file,prot);

			// Otherwise, allow patched function to be used
			else
				res=SetProtection(dest_file,prot);

			// Successful?
			if (res && got_dest_info) d_info->fib_Protection=prot;
		}

		// Copy comment
		if (copy_flags&COPY_NOTE)
		{
			short res;

			// Set comment; use original function if at top-level
			if (copy_flags&COPY_TOP_LEVEL)
				res=OriginalSetComment(dest_file,s_info->fib_Comment);

			// Otherwise, allow patched function to be used
			else
				res=SetComment(dest_file,s_info->fib_Comment);

			// Successful?
			if (res && got_dest_info) strcpy(d_info->fib_Comment,s_info->fib_Comment);
		}

		// Did we fail to get destination info from the file handle?
		if (!got_dest_info) ret_code=COPY_FAILED;

		// Set source archive?
		if (copy_flags&COPY_ARC)
		{
			if (!(s_info->fib_Protection&FIBF_ARCHIVE))
			{
				// Get new bits
				s_info->fib_Protection|=FIBF_ARCHIVE;

			        CurrentDir(source_path_lock);

				// Set protection; use original function if at top-level
				if (copy_flags&COPY_TOP_LEVEL)
					OriginalSetProtection(source_file,s_info->fib_Protection);

				// Otherwise, allow patched function to be used
				else
					SetProtection(source_file,s_info->fib_Protection);
			}
		}
	}

	// Otherwise, delete failed file
	else
	if (out_file)
	{
	    CurrentDir(dest_path_lock);

	    DeleteFile(dest_file);
	}

	CurrentDir(source_path_lock);

	return ret_code;
}


// Set position for an icon
static void copy_icon_position(FunctionHandle *handle,char *name,struct DiskObject *icon)
{
	ULONG flags;

	// Get icon flags
	flags=GetIconFlags(icon);

	// Reset position
	icon->do_CurrentX=NO_ICON_POSITION;
	icon->do_CurrentY=NO_ICON_POSITION;
	flags&=~ICONF_POSITION_OK;

	if (handle->lister_app_msg)
	{
		short num;

		// Find arg number
		if ((num=FindGLArg(
			handle->lister_app_msg->glam_ArgList,
			handle->lister_app_msg->glam_NumArgs,
			name))>-1)
		{
			short x,y;

			// Fix icon position
			x=handle->lister_app_msg->glam_DragOffset.x+
				handle->lister_app_msg->glam_MouseX+
				handle->lister_app_msg->glam_ArgData[num].glad_DropPos.x;
			y=handle->lister_app_msg->glam_DragOffset.y+
				handle->lister_app_msg->glam_MouseY+
				handle->lister_app_msg->glam_ArgData[num].glad_DropPos.y;

			// Store position, unless removing Galileo positions
			if (!(environment->env->display_options&DISPOPTF_REMGALILEOPOS))
			{
				SetIconPosition(icon,x,y);
				flags|=ICONF_POSITION_OK;
			}

			// Remove Galileo position
			else
			{
				flags&=~ICONF_POSITION_OK;
			}

			// Using Workbench positions?
			if (environment->env->display_options&DISPOPTF_ICON_POS)
			{
				// Set position
				icon->do_CurrentX=x;
				icon->do_CurrentY=y;
			}
		}
	}

	// Set flags
	SetIconFlags(icon,flags);
}
