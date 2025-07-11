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

enum
{
	USER_ARG_ID,
};

enum
{
	USER_ARG_ALT,
	USER_ARG_CTRL,
	USER_ARG_SHIFT,
};

// USER filetype function
GALILEOFM_FUNC(function_user)
{
	Cfg_Filetype *type,*last_type=0;
	Cfg_Function *func,*type_function=0;
	PathNode *source;
	FunctionEntry *entry;
	short action;
	char *cmd_ptr=0,cmd_buf[10],*dest_path=0;
	Lister *dest_lister=0,*source_lister=0;
	UWORD qual=0;

	// Get user filetype type
	if (command->function==FUNC_USER1)
	{
		action=FTTYPE_USER_1;
		cmd_ptr="user 1";
	}
	else
	if (command->function==FUNC_USER2)
	{
		action=FTTYPE_USER_2;
		cmd_ptr="user 2";
	}
	else
	if (command->function==FUNC_USER3)
	{
		action=FTTYPE_USER_3;
		cmd_ptr="user 3";
	}
	else
	if (command->function==FUNC_USER4)
	{
		action=FTTYPE_USER_4;
		cmd_ptr="user 4";
	}
	else
	if (command->function==FUNC_DOUBLECLICK)
	{
		// Get command
		cmd_ptr="doubleclick";

		// Arguments?
		if (instruction->ipa_funcargs)
		{
			// Control?
			if (instruction->ipa_funcargs->FA_Arguments[USER_ARG_CTRL])
			{
				action=FTTYPE_CTRL_DOUBLECLICK;
				qual|=IEQUALIFIER_CONTROL;
			}
			else

			// Alt?
			if (instruction->ipa_funcargs->FA_Arguments[USER_ARG_ALT])
			{
				action=FTTYPE_ALT_DOUBLECLICK;
				qual|=IEQUALIFIER_LALT;
			}
			else
			action=FTTYPE_DOUBLE_CLICK;
		}
		else
		action=FTTYPE_DOUBLE_CLICK;
	}
	else
	if (command->function==FUNC_DRAGNDROP)
	{
		// Get command
		cmd_ptr="dropfrom";

		// Arguments?
		if (instruction->ipa_funcargs)
		{
			// Control?
			if (instruction->ipa_funcargs->FA_Arguments[USER_ARG_CTRL])
			{
				action=FTTYPE_CTRL_DRAGDROP;
				qual|=IEQUALIFIER_CONTROL;
			}
			else

			// Alt?
			if (instruction->ipa_funcargs->FA_Arguments[USER_ARG_ALT])
			{
				action=FTTYPE_ALT_DRAGDROP;
				qual|=IEQUALIFIER_LALT;
			}
			else
			action=FTTYPE_DRAG_DROP;
		}
		else
		action=FTTYPE_DRAG_DROP;
	}
	else
	if (command->function==FUNC_USER && instruction->ipa_funcargs && instruction->ipa_funcargs->FA_Arguments[USER_ARG_ID])
	{
		short id;

		// Get ID
		id=*((ULONG *)instruction->ipa_funcargs->FA_Arguments[USER_ARG_ID]);

		// Build command
		lsprintf(cmd_buf,"user %ld",id);
		cmd_ptr=cmd_buf;

		// Get action from ID
		if (id==1) action=FTTYPE_USER_1;
		else
		if (id==2) action=FTTYPE_USER_2;
		else
		if (id==3) action=FTTYPE_USER_3;
		else
		if (id==4) action=FTTYPE_USER_4;
		else
		if (id>=5 && id<=10) action=FTTYPE_USER_5+(id-5);
		else return 0;
	}
	else return 0;

	// Get destination lister
	if (source=function_path_current(&handle->func_dest_paths))
	{
		// Get destination path
		if (dest_lister=source->pn_lister)
			dest_path=dest_lister->cur_buffer->buf_Path;
	}

	// Get source lister
	if (source=function_path_current(&handle->func_source_paths))
		source_lister=source->pn_lister;

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		// Custom handler assigned?
		if (source_lister && source_lister->cur_buffer->buf_CustomHandler[0])
		{
			// Send double-click message
			rexx_handler_msg(
				0,
				source_lister->cur_buffer,
				RXMF_WARN,
				HA_String,0,cmd_ptr,
				HA_Value,1,source_lister,
				HA_String,2,entry->fe_name,
				HA_Value,3,dest_lister,
				HA_String,5,dest_path,
				HA_Qualifier,6,qual,
				TAG_END);

			// Get next entry
			function_end_entry(handle,entry,1);
			continue;
		}

		// Build source name
		function_build_source(handle,entry,handle->func_work_buf);

		// Match filetype for this file
		if (type=filetype_identify(handle->func_work_buf,action,0,0))
		{
			// Get appropriate function
			if (func=FindFunctionType(&type->function_list,action))
			{
				ExternalEntry *exentry;

				// Different to the last type?
				if (last_type && type!=last_type)
				{
					// Anything in the external list?
					if (!(IsListEmpty((struct List *)&handle->external_list)) &&
						type_function)
					{
						// Run this function
						function_user_run(handle,type_function);
					}
				}

				// Save type pointer
				last_type=type;

				// Save function pointer
				type_function=func;

				// Create a new external entry
				if (exentry=new_external_entry(handle,handle->func_work_buf))
				{
					// Save pointer to lister entry
					exentry->een_entry=entry->fe_entry;

					// Copy icon mode flag
					exentry->een_flags |= entry->fe_flags&FUNCENTF_ICON_ACTION;

					// Add to external entry list
					AddTail((struct List *)&handle->external_list,(struct Node *)exentry);
				}
			}
		}

		// Get next entry
		function_end_entry(handle,entry,1);
	}

	// Anything left in the external list?
	if (!(IsListEmpty((struct List *)&handle->external_list)) &&
		type_function)
	{
		// Run this function
		function_user_run(handle,type_function);
	}

	return handle->ret_code;
}


// Run another function on top of this one
void function_user_run(FunctionHandle *handle,Cfg_Function *function)
{
	FunctionHandle *backup;
	ULONG flags;

	// Allocate backup
	if (!(backup=AllocVec(sizeof(FunctionHandle),0)))
		return;

	// Back-up handle
	CopyMem((char *)handle,(char *)backup,sizeof(FunctionHandle));

	// Initialise handle function
	function_handle_init(handle,0);

	// Set function pointer
	handle->function=function;

	// Run function
	function_run_function(handle);

	// Has the destination path list changed?
	if (backup->func_dest_paths.list.mlh_Head!=handle->func_dest_paths.list.mlh_Head ||
		backup->func_dest_paths.list.mlh_TailPred!=handle->func_dest_paths.list.mlh_TailPred)
	{
		// List not empty?
		if (!(IsListEmpty((struct List *)&backup->func_dest_paths)))
		{
			// Copy destination list pointers into backup
			backup->func_dest_paths=handle->func_dest_paths;
			backup->func_dest_paths.list.mlh_TailPred->mln_Succ=(struct MinNode *)&handle->func_dest_paths.list.mlh_Tail;
			backup->func_dest_paths.list.mlh_Head->mln_Pred=(struct MinNode *)&handle->func_dest_paths.list.mlh_Head;
		}
	}

	// Save flags
	flags=handle->flags;

	// Restore handle
	backup->ret_code=handle->ret_code;
	CopyMem((char *)backup,(char *)handle,sizeof(FunctionHandle));

	// Desktop flag?
	if (flags&FUNCF_RESCAN_DESKTOP) handle->flags|=FUNCF_RESCAN_DESKTOP;

	// Free backup
	FreeVec(backup);

	// Clear the external list
	NewList((struct List *)&handle->external_list);
}
