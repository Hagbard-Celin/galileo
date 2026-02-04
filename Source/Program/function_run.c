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
#include <gfm/strings_protos.h>
#include <gfm/selectionlist.h>
#include <gfm/function_flags.h>
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "lister_protos.h"
#include "status_text_protos.h"
#include "function_data.h"

// Run a function
void function_run_function(FunctionHandle *handle)
{
	InstructionParsed *instruction;

	// Got a valid function?
	if (!handle || !handle->function) return;

	// Parse function
	if (function_parse_function(handle))
	{
		// Run the function
		handle->ret_code=function_run(handle);

		// Clean up recurse data
		if (handle->recurse_return!=-1)
		{
			// End recurse stuff
			MatchEnd(handle->anchor);

			if (handle->recurse_entry_data->fe_lock)
			    UnLock(handle->recurse_entry_data->fe_lock);

			// Fix CD
			if (handle->recurse_cd)
			{
				BPTR lock=CurrentDir(handle->recurse_cd);
				UnLock(lock);	
				handle->recurse_cd=0;
			}
		}

		// Do final changes to lists
		function_do_lister_changes(handle,&handle->func_source_paths);
		function_do_lister_changes(handle,&handle->func_dest_paths);

		// Display final message (if appropriate)
		if (handle->final_message[0] &&
			handle->func_source_paths.list.mlh_Head->mln_Succ)
		{
			status_text(
				((PathNode *)handle->func_source_paths.list.mlh_Head)->pn_lister,
				handle->final_message);
		}
	}

	// Go through instruction list
	for (instruction=(InstructionParsed *)handle->func_instructions.mlh_Head;
		instruction->ipa_node.mln_Succ;
		instruction=(InstructionParsed *)instruction->ipa_node.mln_Succ)
	{
		// Free ReadArgs stuff?
		DisposeArgs(instruction->ipa_funcargs);
	}
}


// Run a function
short function_run(FunctionHandle *handle)
{
	InstructionParsed *instruction;
	PathNode *path;
	BOOL done_paths=0;
	short run_okay;

	// Parameters supplied?
	if (handle->function)
	{
		// Copy function parameters
		handle->func_parameters=handle->function->function;
	}

	// Add additional flags
	handle->func_parameters.flags|=handle->func_additional_flags;

	// Initialise output handle
	strcpy(handle->output_handle,environment->env->output_device);
	StrConcat(handle->output_handle,environment->env->output_window,sizeof(handle->output_handle)-1);
	StrConcat(handle->output_handle,"/AUTO/SCREEN ",sizeof(handle->output_handle)-1);
	StrConcat(handle->output_handle,(handle->func_parameters.flags&FUNCF_WORKBENCH_OUTPUT)?"Workbench":get_our_pubscreen(),sizeof(handle->output_handle)-1);

	// Reset source count
	handle->func_source_num=0;
    
	// Check source and destination
	if (!(function_check_paths(handle)))
		return 0;

	// Get first path
	path=function_path_next(&handle->func_source_paths);

	// Count the instructions
	handle->instruction_count=Att_NodeCount((Att_List *)&handle->func_instructions);

	// Go through source lists
	do
	{
		BOOL first=1;

		// Reset ok flag
		run_okay=1;

		if (handle->func_source_path)
		    FreeMemH(handle->func_source_path);

		// Get source path
		if (path && path->pn_path)
		{
		    char *tmp_path;

		    if (tmp_path = CopyString(handle->memory, path->pn_path))
		    {

			handle->func_source_path = tmp_path;
		    }

		    handle->func_source_lock = path->pn_lock;
		}
		else
		    handle->func_source_path=0;

		// Reset iteration count
		handle->func_iterations=0;

		// If we're reloading files, update stamp in source
		if (handle->func_parameters.flags&FUNCF_RELOAD_FILES && path)
			path->pn_flags|=LISTNF_UPDATE_STAMP;

		// Loop until finished
		FOREVER
		{
			// Go through instruction list
			for (instruction=(InstructionParsed *)handle->func_instructions.mlh_Head;
				instruction->ipa_node.mln_Succ;
				instruction=(InstructionParsed *)instruction->ipa_node.mln_Succ)
			{
				// Once only?
				if (instruction->ipa_command && instruction->ipa_count>0)
				{
					// Check count; skip if already used
					if (instruction->ipa_count>1) continue;

					// Increment count
					++instruction->ipa_count;
				}
				else if (instruction->ipa_string && *instruction->ipa_string=='#')
				{
					// Check count; skip if already used
					if (instruction->ipa_count>0) continue;

					// Increment count
					++instruction->ipa_count;
				}

				// Set instruction data pointer
				handle->inst_data=instruction->ipa_inst_data;
				handle->inst_flags=instruction->ipa_inst_flags;

				// Are we out of files?
				if (!function_current_entry(handle))
				{
					// Build new entry list
					if ((function_build_list(handle,&path,instruction))==-1)
					{
						run_okay=0;
						break;
					}

					// Check entries
					if (instruction->ipa_flags&FUNCF_NEED_ENTRIES &&
						(!first || !(instruction->ipa_flags&FUNCF_WANT_ENTRIES)) &&
						!(function_current_entry(handle)))
					{
						run_okay=0;
						break;
					}
				}

				// Check instruction path
				function_check_ins_path(handle,instruction,&handle->func_dest_paths,'d',0);

				// Haven't done path check yet?
				if (!done_paths)
				{
					// Check valid destination if needed
					if (handle->func_flags&FUNCF_NEED_DEST &&
						handle->func_flags&FUNCF_ASK_PATH &&
						(IsListEmpty((struct List *)&handle->func_dest_paths)))
					{
						// If we fail on source, don't ask for dest                  *****
						if ((handle->func_flags&FUNCF_NEED_SOURCE &&
							!(handle->func_flags&FUNCF_GOT_SOURCE)) ||
							!(function_check_single(handle,LISTERF_DEST,0,0)))
						{
							run_okay=0;
							break;
						}

						// Got destination
						handle->func_flags|=FUNCF_GOT_DEST;
					}

					// Set flag
					done_paths=1;
				}

				// Run this instruction
				run_okay=function_run_instruction(handle,instruction);

				// Pending quit or abort?
				if (GUI->flags&GUIF_PENDING_QUIT || function_check_abort(handle))
					run_okay=0;

				// Failed?
				if (run_okay!=1) break;

				// Store flags in case they've changed
				instruction->ipa_inst_flags=handle->inst_flags;

				// Do cleanup for this instruction
				function_cleanup(handle,path,0);
			}

			// Aborted?
			if (run_okay!=1 || function_check_abort(handle))
				break;

			// Do all files not set, or no more files?
			if (!(handle->func_parameters.flags&FUNCF_DO_ALL_FILES) ||
				!function_current_entry(handle))
				break;

			// Start again at first argument
			handle->func_current_arg=handle->func_arguments.lh_Head;

			// Increment iteration count
			++handle->func_iterations;
			first=0;
		}

		// Close the script file (will run function if successful)
		function_close_script(handle,run_okay);

		// Done with this path
		function_path_end(handle,&handle->func_source_paths,1);

		// Check abort
		if (!run_okay || function_check_abort(handle))
			break;

		// Increment source count
		++handle->func_source_num;

		// Get next path
		path=function_path_next(&handle->func_source_paths);
	} while (path);

	return run_okay;
}


// Run an instruction in a function
function_run_instruction(
	FunctionHandle *handle,
	InstructionParsed *instruction)
{
	short ret=1;

	// Save instruction
	handle->cur_instruction=instruction;

	// Is the instruction an internal function?
	if (instruction->ipa_type==INST_COMMAND)
	{
		char *args;
		short limit;

		// Invalid command?
		if (!instruction->ipa_command) return 1;

		// Run preceding external commands
		if (handle->script_file)
		{
			function_close_script(handle,1);
		}

		// Get cll limit
		if ((limit=environment->env->settings.command_line_length)<256)
			limit=256;

		// Allocate space for arguments
		if (args=AllocMemH(handle->memory,limit+1))
		{
			ret=0;

			// Build function string
			if ((function_build_instruction(handle,instruction,0,args))!=PARSE_ABORT)
			{
				// Check arguments
				function_parse_arguments(handle,instruction);

				// If we needs files, check there are some
				if (!(instruction->ipa_flags&FUNCF_NEED_ENTRIES) ||
					instruction->ipa_flags&FUNCF_WANT_ENTRIES ||
					(function_current_entry(handle)))
				{
					// Don't need a source path, or have one already?
					if (!(handle->func_flags&FUNCF_NEED_SOURCE) ||
						(handle->func_flags&FUNCF_GOT_SOURCE))
					{
						// Run the command
						ret=function_internal_command(
							instruction->ipa_command,
							args,
							handle,
							instruction);
					}
				}
			}

			// Free args
			FreeMemH(args);
		}
	}

	// Otherwise, it's an external function
	else
	{
		return function_external_command(handle,instruction);
	}

	return ret;
}


// Check a function has the right paths
BOOL function_check_paths(FunctionHandle *handle)
{
	// Do we need a source?
	if (handle->func_flags&(FUNCF_NEED_SOURCE|FUNCF_WANT_SOURCE|FUNCF_NEED_ENTRIES|FUNCF_WANT_ENTRIES))
	{
		// Don't already have source?
		if (!(handle->func_flags&FUNCF_GOT_SOURCE))
		{
			// Build list of sources
			if (function_get_paths(
				handle,
				&handle->func_source_paths,
				LISTERF_SOURCE,
				handle->func_flags&FUNCF_SINGLE_SOURCE))
			{
				handle->func_flags|=FUNCF_GOT_SOURCE;
			}

			// No sources; can we make our own?
			else
			if (handle->func_flags&FUNCF_CREATE_SOURCE)
			{
				Lister *lister;

				// Create a new lister
				if (lister=lister_new(0))
				{
					// We want lister to be available for functions
					lister->flags2&=~LISTERF2_UNAVAILABLE;

					// Open lister
					IPC_Command(lister->ipc,LISTER_OPEN,0,GUI->screen_pointer,0,0);

					// Add this lister
					function_add_path(handle,&handle->func_source_paths,lister,0,0);

					// Set flag to say we made our own
					handle->result_flags|=FRESULTF_MADE_LISTER;
				}
			}
		}

		// If we got source, lock it
		if (handle->func_flags&FUNCF_GOT_SOURCE &&
			!(function_lock_paths(handle,&handle->func_source_paths,1)))
			handle->func_flags&=~FUNCF_GOT_SOURCE;
	}

	// Do we need a destination?
	if (handle->func_flags&(FUNCF_NEED_DEST|FUNCF_WANT_DEST))
	{
		// Don't already have destination?
		if (!(handle->func_flags&FUNCF_GOT_DEST))
		{
			// Build list of destinations and lock it
			if (!(handle->flags&FUNCF_ASK_DEST) &&
				function_get_paths(
					handle,
					&handle->func_dest_paths,
					LISTERF_DEST,
					handle->func_flags&FUNCF_SINGLE_DEST))
			{
				handle->func_flags|=FUNCF_GOT_DEST;
			}
		}

		// If we got destination, lock it
		if (handle->func_flags&FUNCF_GOT_DEST &&
			!(function_lock_paths(handle,&handle->func_dest_paths,0)))
			handle->func_flags&=~FUNCF_GOT_DEST;

		// Do single destination check
		else
		// Do not ask user for destination if drag-and-drop, we already got it.
		if (!(handle->flags&FUNCF_DRAG_DROP) &&
		    handle->func_flags&FUNCF_GOT_DEST &&
		    handle->func_flags&FUNCF_SINGLE_DEST)
		{   
			if (!(function_check_single(handle,LISTERF_DEST,LISTERF_DEST,0)))
			{
				handle->func_flags&=~FUNCF_GOT_DEST;

				return 0;
			}
		}
	}

	return 1;
}


// Do checking for single path
short function_check_single(
	FunctionHandle *handle,
	long type,
	long usetype,
	short locker)
{
	IPCData *ipc;
	Lister *lister;
	Att_List *list;
	Att_Node *node=0;
	short sel=-1,ret=1;
	PathList *path_list;
	PathNode *current=0;

	// Get pathlist pointer
	if (type==LISTERF_SOURCE) path_list=&handle->func_source_paths;
	else path_list=&handle->func_dest_paths;

	// Got a current lister?
	if (usetype &&
		(!(current=function_path_current(path_list)) || !current->pn_lister))
		return 1;

	// Create list
	if (!(list=Att_NewList(0)))
		return 1;

	// Want to build list, or go straight to filereq?
	if (type==LISTERF_SOURCE || !(handle->flags&FUNCF_ASK_DEST))
	{
		// Lock lister list
		lock_listlock(&GUI->lister_list,FALSE);

		// Go through lister list
		for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
			ipc->node.mln_Succ;
			ipc=(IPCData *)ipc->node.mln_Succ)
		{
			BOOL ok=0;

			// Get lister
			lister=IPCDATA(ipc);

			// Is this lister the right type and not busy, or an icon view?
			if ((!usetype || lister->flags&usetype) &&
				lister->cur_buffer->buf_Path &&
				lister->cur_buffer->buf_Path[0] &&
				!(lister->flags&LISTERF_BUSY) &&
				(!(lister->flags&LISTERF_VIEW_ICONS) || lister->flags&LISTERF_ICON_ACTION)) ok=1;

/* ACTION_CHANGE */
			// Or, is it the default icon view?
			else
			if (lister==GUI->current_lister &&
				lister->flags&LISTERF_VIEW_ICONS &&
				!(lister->flags&LISTERF_ICON_ACTION)) ok=1;

			// Ok to use?
			if (ok)
			{
				// Add to list
				Att_NewNode(list,lister->cur_buffer->buf_Path,(ULONG)lister,ADDNODE_SORT|ADDNODE_EXCLUSIVE);
			}
		}

		// Unlock lister list
		unlock_listlock(&GUI->lister_list);
	}

	// Only put up list if too many listers, or none
	if (!usetype || !(IsListEmpty((struct List *)list)))
	{
		// Any other possible paths?
		if (!(IsListEmpty((struct List *)list)) && current)
		{
			// Add the path we do have to lister
			if (node=Att_NewNode(list,current->pn_lister->cur_buffer->buf_Path,(ULONG)current,ADDNODE_SORT))
				sel=Att_FindNodeNumber(list,node);
		}

		// Clear buffer
		handle->func_work_buf[0]=0;

		// Are there no paths in the list?
		if (IsListEmpty((struct List *)list))
		{
			// Set requester coordinates
			*((struct IBox *)&handle->filereq->fr_LeftEdge)=handle->coords;

			// Put up requester
			if (AslRequestTags(
				handle->filereq,
				ASLFR_Screen,GUI->screen_pointer,
				ASLFR_TitleText,GetString(&locale,(type==LISTERF_SOURCE)?MSG_SELECT_SOURCE:MSG_SELECT_DESTINATION),
				ASLFR_Flags1,FRF_PRIVATEIDCMP,
				ASLFR_Flags2,FRF_DRAWERSONLY,
				TAG_END))
			{
				// Build path name
				strcpy(handle->func_work_buf,handle->filereq->fr_Drawer);
				AddPart(handle->func_work_buf,handle->filereq->fr_File,256);
				sel=-1;
			}

			// Cancelled
			else sel=-2;

			// Save coordinates
			handle->coords=*((struct IBox *)&handle->filereq->fr_LeftEdge);
		}

		// Otherwise, show selection list
		else
		{
			// Show selection list
			sel=SelectionList(
				list,
				0,GUI->screen_pointer,
				GetString(&locale,(type==LISTERF_SOURCE)?MSG_SELECT_SOURCE:MSG_SELECT_DESTINATION),
				sel,
				SLF_DIR_FIELD,handle->func_work_buf,
				GetString(&locale,MSG_OKAY),
				GetString(&locale,MSG_ABORT),0,0);
		}

		// Cancelled?
		if (sel==-2 || (sel==-1 && !handle->func_work_buf[0]))
		{
			// Cancelled
			ret=0;

			// Already got a path?
			if (current)
			{
				// Unlock current path
				function_unlock_paths(handle,path_list,locker);

				// Remove current path
				Remove((struct Node *)current);
				FreeMemH(current);
				path_list->current=0;
			}
		}

		// New path?
		else	
		if (handle->func_work_buf[0] ||
			(node=Att_FindNode(list,sel)) && (!current || node->att_data!=(ULONG)current))
		{
			// Already got a path?
			if (current)
			{
				// Unlock current path
				function_unlock_paths(handle,path_list,locker);

				// Free current path
				FreeMemH(current);
			}

			// Get new current path
			if (current=AllocMemH(handle->memory,sizeof(PathNode)))
			{
				// Typed-in path name?
				if (handle->func_work_buf[0])
				{
					char *tmp_path;

					if(current->pn_lock = LockFromPath(handle->func_work_buf, NULL, NULL))
					{
					    current->pn_flags = LISTNF_MADE_LOCK;

					    if (type==LISTERF_SOURCE)
						handle->func_source_lock = current->pn_lock;
					    else
						handle->func_dest_lock = current->pn_lock;

					}

					if (tmp_path = CopyString(handle->memory, handle->func_work_buf))
					{
					    if (current->pn_path_buf)
						FreeMemH(current->pn_path_buf);

					    current->pn_path_buf = tmp_path;
					}

					current->pn_path=current->pn_path_buf;
				}

				// Lister
				else
				{
/********* THIS SHOULD BE PROTECTED ************/
					// Got a node?
					if (node)
					{
						// Lock lister list
						lock_listlock(&GUI->lister_list,FALSE);

						// Go through lister list
						for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
							ipc->node.mln_Succ;
							ipc=(IPCData *)ipc->node.mln_Succ)
						{
							// Is this the lister we asked for?
							if (IPCDATA(ipc)==(APTR)node->att_data)
							{
								// Grab it
								current->pn_lister=(Lister *)node->att_data;

								if (current->pn_lister->cur_buffer && (current->pn_lock = current->pn_lister->cur_buffer->buf_Lock))
								{
								    if (type==LISTERF_SOURCE)
									handle->func_source_lock = current->pn_lock;
								    else
									handle->func_dest_lock = current->pn_lock;
								}
								break;
							}
						}

						// Unlock lister list
						unlock_listlock(&GUI->lister_list);
					}

					// Haven't got a lister?
					if (!current->pn_lister)
					{
						// Get path
						if (node)
						{
						    char *tmp_path;

						    if (tmp_path = CopyString(handle->memory, node->node.ln_Name))
						    {
						        if (current->pn_path_buf)
							    FreeMemH(current->pn_path_buf);

						        current->pn_path_buf = tmp_path;
						    }
						}
						else
						{
						    if (current->pn_path_buf)
							FreeMemH(current->pn_path_buf);

						    current->pn_path_buf = 0;
						}

						current->pn_path=current->pn_path_buf;
					}
				}

				// Add to list
				NewList((struct List *)path_list);
				AddHead((struct List *)path_list,(struct Node *)current);

				// Fix current pointer
				path_list->current=current;

				// Lock this path (if a lister)
				if (current->pn_lister)
				{
					ret=function_lock_paths(handle,path_list,locker);
				}
			}

			// Failed
			else
			{
				path_list->current=0;
				ret=0;
			}
		}
	}

	// Free list
	Att_RemList(list,0);

	return ret;
}


// Check for a path supplied in instruction
void function_check_ins_path(
	FunctionHandle *handle,
	InstructionParsed *instruction,
	PathList *path_list,
	char path_type,
	short locker)
{
	char *ptr;

	// Path supplied in instruction?
	if (instruction &&
		instruction->ipa_command &&
		instruction->ipa_command->template_key &&
		(ptr=strchr(instruction->ipa_command->template_key,path_type)))
	{
		// Parsed arguments?
		if (instruction->ipa_funcargs)
		{
			char *path;

			// Get number of path argument
			short num=atoi(ptr+1);

			// Valid path?
			if ((path = (char *)instruction->ipa_funcargs->FA_Arguments[num]) && path[0])
			{
				char *tmp_path, **pathp;

				// Replace existing paths with new one
				function_replace_paths(handle,path_list,path,locker);

				if (path_type=='d')
				{
				    pathp = &handle->func_dest_path;
				    handle->func_dest_lock = handle->func_dest_paths.current->pn_lock;
				}
				else
				{
				    pathp = &handle->func_source_path;
				    handle->func_source_lock = handle->func_source_paths.current->pn_lock;
				}

			        if (tmp_path = CopyString(handle->memory, path))
			        {
				    if (*pathp)
				        FreeMemH(*pathp);

				    *pathp = tmp_path;
			        }
			}
		}
	}
}


// Replace paths
void function_replace_paths(
	FunctionHandle *handle,
	PathList *path_list,
	char *new_path,
	short locker)
{
	PathNode *current;
	BPTR org_dir = 0;

	// Get current path
	if (current=function_path_current(path_list))
	{
		if (current->pn_lock)
		    org_dir = CurrentDir(current->pn_lock);

		// Unlock current path
		function_unlock_paths(handle,path_list,locker);
	}

	// Initialise path list
	NewList((struct List *)path_list);
	path_list->current=0;

	// Get new current path
	if (current=AllocMemH(handle->memory,sizeof(PathNode)))
	{
		char *tmp_path;

		if(current->pn_lock = LockFromPath(new_path, NULL, NULL))
		    current->pn_flags = LISTNF_MADE_LOCK;

		if (tmp_path = CopyString(handle->memory,new_path))
		{
		    if (current->pn_path_buf)
			FreeMemH(current->pn_path_buf);

		    current->pn_path_buf = tmp_path;
		}

		current->pn_path=current->pn_path_buf;

		// Add to list
		AddHead((struct List *)path_list,(struct Node *)current);

		// Fix current pointer
		path_list->current=current;
	}

	if (org_dir)
	    CurrentDir(org_dir);
}
