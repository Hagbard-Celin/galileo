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
#include "misc_protos.h"
#include "function_launch_protos.h"
#include "function_protos.h"
#include "filetypes_protos.h"

typedef struct
{
	BPTR entry_dir;
	char *name;
	unsigned short qual;
	Cfg_Filetype *last_type;
	Cfg_Function *type_function;
	struct List filechange_list;
	struct WBArg *wb_arg;
	struct GLAData *argd;
	BYTE isroot;
} filetype_data;

static void filetype_get(FunctionHandle *handle, filetype_data *data);

// Do a filetype on a function (? :-)
void function_filetype(FunctionHandle *handle)
{
	Cfg_Function *old_function;
	struct Node *node,*next;
	ULONG wbargs = 0;
	struct WBArg *arglist = 0;
	struct GLAData *argdata = 0;
	filetype_data *data;

	// Set data pointer
	data = (filetype_data *)handle->temp_buffer;
	data->last_type = 0;
	data->type_function = 0;
	data->argd = 0;

	// Extract qualifier
	data->qual=(handle->data>>16)&0xffff;
	handle->data&=0xffff;

	// Backup function pointer
	old_function=handle->function;

	// Lock filetype list
	lock_listlock(&GUI->filetypes,FALSE);

	// Steal the filechange list
	NewList(&data->filechange_list);
	for (node=handle->filechange.lh_Head;node->ln_Succ;node=next)
	{
		next=node->ln_Succ;
		Remove(node);
		AddTail(&data->filechange_list, node);
	}

	if (handle->lister_app_msg)
	{
	    wbargs = handle->lister_app_msg->glam_NumArgs;
	    arglist = handle->lister_app_msg->glam_ArgList;
	    argdata = handle->lister_app_msg->glam_ArgData;

	}
	else
	if (handle->app_msg)
	{
	    wbargs = handle->app_msg->am_NumArgs;
	    arglist = handle->app_msg->am_ArgList;
	}

	// Got arguments?
	if (wbargs)
	{
	    //struct GLAData *argd;
	    ULONG currarg = 0;

	    data->entry_dir = 0;
	    data->name = 0;

	    while (currarg < wbargs)
	    {
		struct FileInfoBlock __aligned tmp_fib;

		data->name = 0;
		data->entry_dir = 0;
		data->isroot = FALSE;
	        data->wb_arg = &arglist[currarg];

		if (argdata)
		    data->argd = &argdata[currarg];

		if (handle->app_msg)
		{
		    BPTR parent_lock;


		    if (WBArgDir(data->wb_arg))
		    {
			// Get directory name
			Examine(data->wb_arg->wa_Lock,&tmp_fib);
			data->name = tmp_fib.fib_FileName;

		        // Try to get parent dir
		        if (parent_lock = ParentDir(data->wb_arg->wa_Lock))
		        {
			    UnLock(data->wb_arg->wa_Lock);
			    data->wb_arg->wa_Lock = parent_lock;
		        }
		        else
		        {
			    UBYTE len;

			    len = strlen(data->name);
			    data->name[len] = ':';
			    data->name[len+1] = 0;
			    data->isroot = TRUE;
		        }
		    }
		}

		if (!data->name)
		    data->name = data->wb_arg->wa_Name;

	        if (data->wb_arg->wa_Lock)
		    data->entry_dir = data->wb_arg->wa_Lock;
	        else
		if (handle->lister_app_msg)
		    data->entry_dir = handle->lister_app_msg->glam_Lock;

		filetype_get(handle, data);
	        currarg++;
	    }
	}

	// Got arguments?
	else
	if (handle->args)
	{
		struct ArgArrayEntry *arg;

		// Go through list
		for (arg=(struct ArgArrayEntry *)handle->args->aa_List.mlh_Head;
			arg->aae_Node.mln_Succ;
			arg=(struct ArgArrayEntry *)arg->aae_Node.mln_Succ)
		{
			data->entry_dir = arg->aae_Lock;
			data->name = arg->aae_String;
			filetype_get(handle, data);
		}
	}
	else
	if (handle->func_source_lock && handle->func_source_path && handle->func_source_path[0])
	{
	    data->entry_dir = handle->func_source_lock;
	    data->name = handle->func_source_path;
	    filetype_get(handle, data);
	}

	// Anything left in the external list?
	if (!(IsListEmpty((struct List *)&handle->external_list)) &&
		data->type_function)
	{
		Cfg_Function *copy;

		// Initialise handle function
		function_handle_init(handle,1);

		// Copy the function
		if (copy=CopyFunction(data->type_function,handle->memory,0))
		{
			// Set function pointer
			handle->function=copy;

			// Unlock the filetype list
			unlock_listlock(&GUI->filetypes);

			// Run function
			function_run_function(handle);

			// Free the function
			FreeFunction(copy);

			// Lock filetype list again
			lock_listlock(&GUI->filetypes,FALSE);

			// Steal new filechanges
			for (node=handle->filechange.lh_Head;node->ln_Succ;node=next)
			{
				next=node->ln_Succ;
				Remove(node);
				AddTail(&data->filechange_list, node);
			}
		}
	}

	// Unlock the filetype list
	unlock_listlock(&GUI->filetypes);

	// Restore function pointer
	handle->function=old_function;

	// Copy filechange list back
	for (node = data->filechange_list.lh_Head; node->ln_Succ; node = next)
	{
		next=node->ln_Succ;
		Remove(node);
		AddTail(&handle->filechange,node);
	}
}

static void filetype_get(FunctionHandle *handle, filetype_data *data)
{
    Cfg_Filetype *type;

    // Match filetype for this file
    // FIXME: Lock?
    if (!(type = filetype_identify(data->name, data->entry_dir, handle->data, 0, data->qual)))
    {
	// Couldn't match. Was this a drag & drop operation?
	if (handle->flags&FUNCF_DRAG_DROP)
	{
	    // Use "default" filetype (copy)
	    type=default_filetype;
	}
    }

    // Got filetype?
    if (type)
    {
	Cfg_Function *func;

	// Get appropriate function
	if (func=FindFunctionType(&type->function_list,handle->data))
	{
	    ExternalEntry *entry;

	    // Different to the last type?
	    if (data->last_type && type != data->last_type)
	    {
	        // Anything in the external list?
	        if (!(IsListEmpty((struct List *)&handle->external_list)) &&
		        data->type_function)
	        {
		    struct Node *node,*next;
		    Cfg_Function *copy;

	            // Initialise handle function
	            function_handle_init(handle,1);

	            // Copy the function
	            if (copy=CopyFunction(data->type_function,handle->memory,0))
	            {
		        // Set function pointer
		        handle->function=copy;

		        // Unlock the filetype list
		        unlock_listlock(&GUI->filetypes);

		        // Run function
		        function_run_function(handle);

		        // Free the function
		        FreeFunction(copy);

		        // Lock filetype list again
		        lock_listlock(&GUI->filetypes,FALSE);

		        // Steal new filechanges
		        for (node=handle->filechange.lh_Head;node->ln_Succ;node=next)
		        {
		            next=node->ln_Succ;
		            Remove(node);
			    AddTail(&data->filechange_list, node);
		        }
	            }

	            // Clear external list
	            NewList((struct List *)&handle->external_list);
	        }
	    }

	    // Save type pointer
	    data->last_type=type;

	    // Save function pointer
	    data->type_function=func;

	    // Create a new external entry
	    if (entry=new_external_entry(handle,data->name,data->entry_dir))
	    {
		if (data->wb_arg)
		{
		    if (data->argd)
		    {
		        // Directory?
			if (data->argd->glad_Flags&AAEF_DIR)
			    entry->een_type=1;

		        // Link?
			if (data->argd->glad_Flags&AAEF_LINK)
			    entry->een_flags|=FUNCENTF_LINK;

		        // Device?
		        if (data->argd->glad_Flags&AAEF_DEV)
		        {
			    entry->een_type=ENTRY_DEVICE;
			    handle->flags|=FUNCF_COPY_NO_MOVE;
		        }

		        // Link?
		        if (data->argd->glad_Flags&AAEF_LINK)
			    entry->een_flags|=FUNCENTF_LINK;

		        // Assign?
		        if (data->argd->glad_Flags&AAEF_ASSIGN)
		        {
			    entry->een_flags|=FUNCENTF_ASSIGN;
			    handle->flags|=FUNCF_COPY_NO_MOVE;
		        }

		        // Multi directory assign?
		        if (data->argd->glad_Flags&AAEF_MULTI_ASSIGN)
		        {
			    entry->een_flags|=FUNCENTF_MULTI_ASSIGN;
			    handle->flags|=FUNCF_COPY_NO_MOVE;
		        }

		        // Fake icon?
			if (data->argd->glad_Flags&AAEF_FAKE_ICON)
			    entry->een_flags|=FUNCENTF_FAKE_ICON;
		    }
		    else
		    {
		        // Directory?
		        if (WBArgDir(data->wb_arg))
			    entry->een_type=1;

		        // Device?
			if (data->isroot)
		        {
			    entry->een_type=ENTRY_DEVICE;
			    handle->flags|=FUNCF_COPY_NO_MOVE;
		        }
		    }
		}
	        // Add to external entry list
	        AddTail((struct List *)&handle->external_list,(struct Node *)entry);
	    }
	}
    }

}
