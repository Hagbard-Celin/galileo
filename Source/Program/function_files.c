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

#include "galileofm.h"
#include <gfm/semaphores_protos.h>
#include <gfm/paths_protos.h>
#include <gfm/cfg_flags.h>
#include "lister.h"
#include "dirlist_protos.h"
#include "misc_protos.h"
#include "reselect_protos.h"
#include "function_launch_protos.h"
#include "function_protos.h"
#include "rexx_protos.h"
#include "buffers_protos.h"
#include "icons.h"

// Build entry list for a function
int function_build_list(FunctionHandle *handle,
		        PathNode **path,
		        InstructionParsed *instruction)
{
    FunctionEntry *funcentry;
    DirEntry *entry;
    ExternalEntry *external;
    char do_icons=0;
    DirBuffer *buffer;
    BOOL done=0;

    // Initialise the entry list
    NewList(&handle->entry_list);

    handle->entry_count=0;
    handle->current_entry=0;
    handle->use_entry=0;
    handle->file_count=0;
    handle->dir_count=0;
    handle->link_file_count=0;
    handle->link_dir_count=0;

    // Do we need entries?
    if (instruction && !(instruction->ipa_flags&(FUNCF_NEED_ENTRIES|FUNCF_WANT_ENTRIES)))
	return 1;

    // Can we do icons?
    if (instruction && instruction->ipa_flags&FUNCF_CAN_DO_ICONS)
    {
	// Config flag set?
	if (environment->env->settings.icon_flags&ICONFLAG_DOUNTOICONS)
	    do_icons=1;

	// Icon function?
	else
	if (handle->flags&FUNCF_ICONS)
	    do_icons=1;
    }

    // Is there an external list?
    if (!(IsListEmpty((struct List *)&handle->external_list)))
    {
	// Go through external list
	for (external=(ExternalEntry *)handle->external_list.mlh_Head;
	     external->een_node.mln_Succ;)
	{
	    ExternalEntry *next=(ExternalEntry *)external->een_node.mln_Succ;

	    // Correct type?
	    if (!instruction ||
	        (instruction->ipa_flags&FUNCF_NEED_FILES && external->een_type<=ENTRY_FILE) ||
	        (instruction->ipa_flags&FUNCF_NEED_DIRS && external->een_type>=ENTRY_DEVICE) ||
	        (instruction->ipa_flags&FUNCF_WANT_ENTRIES &&
	        !(instruction->ipa_flags&(FUNCF_NEED_FILES|FUNCF_NEED_DIRS))))
	    {
	        // Allocate new function entry
	        if (funcentry=function_new_entry(handle,external->een_name,0))
	        {
	            // Fill out entry
	            funcentry->fe_entry=external->een_entry;
	            funcentry->fe_type=external->een_type;
	            funcentry->fe_lock=external->een_lock;
	            funcentry->fe_flags=FUNCENTF_TOP_LEVEL;

	            // Lister in icon mode?
	            if (external->een_flags&FUNCENTF_ICON_ACTION)
		        funcentry->fe_flags|=FUNCENTF_ICON_ACTION;

	            // Link?
	            if (external->een_flags&FUNCENTF_LINK)
		        funcentry->fe_flags|=FUNCENTF_LINK;

	            // Assign?
	            if (external->een_flags&FUNCENTF_ASSIGN)
		        funcentry->fe_flags|=FUNCENTF_ASSIGN;

	            // Multi assign?
	            if (external->een_flags&FUNCENTF_MULTI_ASSIGN)
		        funcentry->fe_flags|=FUNCENTF_MULTI_ASSIGN;

	            // Increment counts
	            ++handle->entry_count;
	            if (funcentry->fe_type>0)
	            {
		        ++handle->dir_count;
		        if (funcentry->fe_flags&FUNCENTF_LINK)
		            ++handle->link_dir_count;
	            }
	            else
	            {
		        ++handle->file_count;
		        if (funcentry->fe_flags&FUNCENTF_LINK)
		            ++handle->link_file_count;
	            }

	            // If this is a directory, check filters
	            if (funcentry->fe_type>=ENTRY_DEVICE &&
		        (GUI->flags&GUIF_FILE_FILTER) &&
		        instruction &&
		        (instruction->ipa_flags&FUNCF_ASK_FILTER) &&
		        !handle->got_filter)
	            {
		        if (!(function_check_filter(handle)))
		            return -1;
	            }

	            // If doing icons, add an icon entry (unless this is an icon)
	            if ((do_icons || funcentry->fe_flags&FUNCENTF_ASSIGN) && !(isicon(external->een_name)) && !(external->een_flags&FUNCENTF_FAKE_ICON))
	            {
		        // Skip if this is a device
		        if (external->een_type != ENTRY_DEVICE)
		        {
		            char *tmp;

		            // Get destination filename
		            tmp = stpcpy(handle->func_work_buf,external->een_name);
		            if (external->een_type == ENTRY_DIRECTORY && external->een_flags&FUNCENTF_ASSIGN)
		            {
			        tmp--;
			        *tmp = 0;
		            }

		            if (funcentry=function_new_entry(handle,handle->func_work_buf,1))
		            {
			        ExternalEntry *test;
			        BOOL removeent=FALSE;

			        if (*path && (*path)->pn_lister && ((*path)->pn_lister->cur_buffer))
			        {
			            if (!(find_entry(&(*path)->pn_lister->cur_buffer->entry_list,
					 funcentry->fe_name,
					 0,
					 (*path)->pn_lister->cur_buffer->more_flags&DWF_CASE|FINDENTRY_ICON)))
			            {
				        removeent=TRUE;
			            }

			        }

			        // Check that entry didn't already exist in external list
			        for (test=(ExternalEntry *)handle->external_list.mlh_Head;
			             test->een_node.mln_Succ;
			             test=(ExternalEntry *)test->een_node.mln_Succ)
			        {
			            if (strcmpi(test->een_name,funcentry->fe_name)==0)
			            {
				        // Is in external list, so we don't need to add one ourselves
				        removeent=TRUE;
				        break;
			            }
			        }

			        if (removeent)
			        {
			            // Is in external list, so we don't need to add one ourselves
			            Remove((struct Node *)funcentry);
			            FreeMemH(funcentry);
			            funcentry=0;
			        }

			        // Still got entry?
			        if (funcentry)
			        {
			            // Fill out entry
			            funcentry->fe_entry=0;
			            funcentry->fe_type=-1;
			            funcentry->fe_lock=external->een_lock;
			            funcentry->fe_flags=FUNCENTF_ICON|FUNCENTF_TOP_LEVEL;

			            if (external->een_type == ENTRY_DIRECTORY && external->een_flags&FUNCENTF_ASSIGN)
				        funcentry->fe_flags |= FUNCENTF_ASSIGN;

			            // Increment counts
			            ++handle->entry_count;
			        }
		            }
		        }
	            }
	        }

	        // Remove from external list
	        Remove((struct Node *)external);
	    }

	    // Get next
	    external=next;
	}

	// Set flag
	done=1;
    }

    // Otherwise, is instruction a command?
    else
    if (instruction && instruction->ipa_command)
    {
	BOOL trapped=0;

	// See if there's a current source lister, with a custom handler
	if (*path && (*path)->pn_lister && (*path)->pn_lister->cur_buffer->buf_CustomHandler[0])
	{
	    // See if there's a trap for this command
	    if (!instruction->ipa_new_arg &&
	        FindFunctionTrap(instruction->ipa_command->name,(*path)->pn_lister->cur_buffer->buf_CustomHandler,0))
	    {
	        trapped=1;
	    }
	}

	// See if it has a valid template
	if (instruction->ipa_command->template_key)
	{
	    char *ptr;

	    // Can it supply files?
	    if ((ptr=strchr(instruction->ipa_command->template_key,FUNCKEY_FILE)) ||
	        (ptr=strchr(instruction->ipa_command->template_key,FUNCKEY_FILENO)))
	    {
	        short num;

	        // Get number of name argument
	        num=atoi(ptr+1);

	        // Parsed arguments?
	        if (instruction->ipa_funcargs &&
	            instruction->ipa_funcargs->FA_Arguments[num])
	        {
	            // Skip wildcards if needed
	            if (*ptr==FUNCKEY_FILE ||
		    !(strchr((char *)instruction->ipa_funcargs->FA_Arguments[num],'*')))
	            {
		        // Allocate new function entry
		        if (funcentry=function_new_entry(handle,(char *)instruction->ipa_funcargs->FA_Arguments[num],0))
		        {
		            // Got a source buffer and a filename only?
		            if (*path && (*path)->pn_lister && !strchr(funcentry->fe_name,'/') && !strchr(funcentry->fe_name,':'))
		            {
			        DirEntry *entry;

			        // Find entry in path
			        if (entry=find_entry(
			            &(*path)->pn_lister->cur_buffer->entry_list,
			            funcentry->fe_name,
			            0,
			            (*path)->pn_lister->cur_buffer->more_flags&DWF_CASE))
			        {
			            // Fix as a lister entry
			            funcentry->fe_entry=entry;
			            funcentry->fe_type=entry->de_Node.dn_Type;
			            funcentry->fe_flags=FUNCENTF_TOP_LEVEL;
			            funcentry->fe_lock = (*path)->pn_lister->cur_buffer->buf_Lock;

			            // Link?
			            if (entry->de_Flags&ENTF_LINK)
				        funcentry->fe_flags|=FUNCENTF_LINK;
			        }
		            }

		            // If we haven't got a lister entry, try to lock file
		            if (!funcentry->fe_entry && !trapped)
		            {
			        BPTR lock,parent;

			        // Lock file
			        if (lock = LockFromPath(funcentry->fe_name, NULL, NULL))
			        {
			            // Examine file
			            Examine(lock,handle->s_info);

			            // Get lock on parent
			            if (parent=ParentDir(lock))
			            {
				        char *tmp_path;

				        // Get name of parent
				        if (tmp_path = PathFromLock(handle->memory, parent, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL))
				        {
				            // Make this the source path
				            function_replace_paths(handle,&handle->func_source_paths,tmp_path,1);

				            if (handle->func_source_path)
					        FreeMemH(handle->func_source_path);

				            handle->func_source_path = tmp_path;

				            // Get lock
				            handle->func_source_lock = handle->func_source_paths.current->pn_lock;

				            // Save old source lister
				            handle->saved_source_lister=handle->source_lister;
				            handle->source_lister=0;

				            // Get new path pointer
				            *path=function_path_next(&handle->func_source_paths);
				        }

				        UnLock(parent);
			            }

			            // Unlock file lock
			            UnLock(lock);

			            // Get type and filename
			            funcentry->fe_type=(handle->s_info->fib_DirEntryType<0)?-1:1;
			            strcpy(funcentry->fe_name,handle->s_info->fib_FileName);
			        }
		            }

		            // Increment counts
		            ++handle->entry_count;
		            if (funcentry->fe_type>0)
		            {
			        ++handle->dir_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_dir_count;
		            }
		            else
		            {
			        ++handle->file_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_file_count;
		            }

		            // Set success flag
		            done=1;
		        }
	            }
	        }
	    }
	}
    }

    // If we don't have anything yet, check for a valid source buffer?
    if (!done && *path && (*path)->pn_lister && (buffer=(*path)->pn_lister->cur_buffer))
    {
	/* ACTION_CHANGE */
	// Icon mode?
	if ((*path)->pn_lister->flags&(LISTERF_VIEW_ICONS|LISTERF_ICON_ACTION))
	{
	    BackdropObject *object;
	    BackdropInfo *info;

	    // Get backdrop info pointer
	    info=(*path)->pn_lister->backdrop_info;

	    // Can we do icons?
	    if (instruction && instruction->ipa_flags&FUNCF_CAN_DO_ICONS)
	        do_icons=1;

	    // Lock backdrop list
	    lock_listlock(&info->objects,0);

	    // Lock buffer
	    buffer_lock(buffer,FALSE);

	    // Go through backdrop list
	    for (object=(BackdropObject *)info->objects.list.lh_Head;
	         object->bdo_node.ln_Succ;
	         object=(BackdropObject *)object->bdo_node.ln_Succ)
	    {
	        // Is icon selected?
	        if (object->bdo_state)
	        {
	            short type;
	            BOOL icon_only=0;
	            DirEntry *entry;

	            // See if we can find this entry
	            if ((entry=find_entry(&buffer->entry_list,object->bdo_name,0,buffer->more_flags&DWF_CASE)) ||
		        (entry=find_entry(&buffer->reject_list,object->bdo_name,0,buffer->more_flags&DWF_CASE)))
	            {
		        // Get type from entry
		        type=entry->de_Node.dn_Type;
	            }

	            // Get type from icon
	            else
	            {
		        type=(object->bdo_icon->do_Type==WBDRAWER || object->bdo_icon->do_Type==WBGARBAGE)?ENTRY_DIRECTORY:ENTRY_FILE;
		        icon_only=1;
	            }

	            // Correct type?
	            if (!instruction ||
		        (instruction->ipa_flags&FUNCF_NEED_FILES && type<ENTRY_DEVICE) ||
		        (instruction->ipa_flags&FUNCF_NEED_DIRS && type>=ENTRY_DEVICE) ||
		        (instruction->ipa_flags&FUNCF_WANT_ENTRIES &&
		        !(instruction->ipa_flags&(FUNCF_NEED_FILES|FUNCF_NEED_DIRS))))
	            {
		        // Not only an icon?
		        if (!icon_only)
		        {
		            // Allocate new function entry
		            if (funcentry=function_new_entry(handle,object->bdo_name,0))
		            {
			        // Fill out entry
			        funcentry->fe_entry=(DirEntry *)object;
			        funcentry->fe_type=type;
			        funcentry->fe_flags=FUNCENTF_TOP_LEVEL|FUNCENTF_ICON_ACTION;

			        // Link?
			        if (object->bdo_flags&BDOF_LINK_ICON)
				    funcentry->fe_flags|=FUNCENTF_LINK;

			        // Increment count
			        ++handle->entry_count;
			        if (funcentry->fe_type>=ENTRY_DEVICE)
			        {
			            ++handle->dir_count;
			            if (funcentry->fe_flags&FUNCENTF_LINK)
				        ++handle->link_dir_count;
			        }
			        else
			        {
			            ++handle->file_count;
			            if (funcentry->fe_flags&FUNCENTF_LINK)
				        ++handle->link_file_count;
			        }
		            }
		        }

		        // Not a fake icon, and we want icons?
		        if (!(object->bdo_flags&BDOF_FAKE_ICON) &&
			    (do_icons || icon_only))
		        {
		            // Create entry for icon
		            if (funcentry=function_new_entry(handle,object->bdo_name,1))
		            {
			        // Fill out entry
			        funcentry->fe_entry=0;
			        funcentry->fe_type=ENTRY_FILE;
			        funcentry->fe_flags=FUNCENTF_TOP_LEVEL|FUNCENTF_ICON_ACTION;

			        // Not only an icon?
			        if (!icon_only) funcentry->fe_flags|=FUNCENTF_ICON;

			        // Link?
			        if (object->bdo_flags&BDOF_LINK_ICON)
			            funcentry->fe_flags|=FUNCENTF_LINK;

			        // Increment counts
			        ++handle->entry_count;
		            }
		        }
	            }
	        }
	    }

	    // Unlock buffer
	    buffer_unlock(buffer);

	    // Unlock backdrop list
	    unlock_listlock(&info->objects);
	}

	// Normal file mode
	else
	{
	    BOOL first=1;

	    // Lock buffer
	    buffer_lock(buffer,FALSE);

	    // Go through source buffer
	    for (entry=(DirEntry *)buffer->entry_list.mlh_Head;
	         entry->de_Node.dn_Succ;
	         entry=(DirEntry *)entry->de_Node.dn_Succ)
	    {
	        // Is entry selected?
	        if (entry->de_Flags&ENTF_SELECTED)
	        {
	            // Correct type?
	            if (!instruction ||
		        (instruction->ipa_flags&FUNCF_NEED_FILES && entry->de_Node.dn_Type<=ENTRY_FILE) ||
		        (instruction->ipa_flags&FUNCF_NEED_DIRS && entry->de_Node.dn_Type>=ENTRY_DEVICE) ||
		        (instruction->ipa_flags&FUNCF_WANT_ENTRIES &&
		        !(instruction->ipa_flags&(FUNCF_NEED_FILES|FUNCF_NEED_DIRS))))
	            {
		        // If this is a directory, check filters
		        if (entry->de_Node.dn_Type>=ENTRY_DEVICE &&
		            (GUI->flags&GUIF_FILE_FILTER) &&
		            instruction &&
		            (instruction->ipa_flags&FUNCF_ASK_FILTER) &&
		            !handle->got_filter)
		        {
		            if (!(function_check_filter(handle)))
		            {
			        buffer_unlock(buffer);
			        return -1;
		            }
		        }

		        // If we're doing icons, and this is an icon, skip it temporarily
		        if (do_icons && isicon(entry->de_Node.dn_Name))
		        {
		            entry->de_Flags|=ENTF_SKIPPED;
		            continue;
		        }

		        // Make reselection list if this is the first
		        if (first)
		        {
		            MakeReselect(&(*path)->pn_lister->reselect,buffer,0);
		            first=0;
		        }

		        // Allocate new function entry
		        if (funcentry=function_new_entry(handle,entry->de_Node.dn_Name,0))
		        {
		            // Fill out entry
		            funcentry->fe_entry=entry;
		            funcentry->fe_type=entry->de_Node.dn_Type;
		            funcentry->fe_flags=FUNCENTF_TOP_LEVEL;

		            // Link?
		            if (entry->de_Flags&ENTF_LINK)
			    funcentry->fe_flags|=FUNCENTF_LINK;

		            // Increment count
		            ++handle->entry_count;
		            if (funcentry->fe_type>0)
		            {
			        ++handle->dir_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_dir_count;
		            }
		            else
		            {
			        ++handle->file_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_file_count;
		            }

		            // Can we handle icons?
		            if (do_icons)
/*
			    instruction &&
			    instruction->flags&FUNCF_CAN_DO_ICONS &&
			    environment->env->settings.icon_flags&ICONFLAG_DOUNTOICONS)
*/
		            {
			        DirEntry *icon_entry;
			        BPTR lock, org_dir = 0;

			        if (handle->func_source_lock)
			            org_dir = CurrentDir(handle->func_source_lock);
#ifdef _DEBUG
		                else
			            KPrintF("       LOCK ERROR in %s:%s, line: %ld\n", __FILE__, __FUNC__, __LINE__);
#endif


			        // Build icon name
			        strcpy(handle->func_work_buf, entry->de_Node.dn_Name);
			        strcat(handle->func_work_buf,".info");

			        // See if entry has an icon in buffer
			        if (!(icon_entry=find_entry(&buffer->entry_list,handle->func_work_buf,0,buffer->more_flags&DWF_CASE)))
			        {
			            // See if it's in the reject list
			            if (icon_entry=find_entry(&buffer->reject_list,handle->func_work_buf,0,buffer->more_flags&DWF_CASE))
			            {
#ifdef _DEBUG_FUNC_FILES
				        KPrintF("FIXME: function_build_list()\n");
				        KPrintF("locking: %s\n",handle->func_work_buf);
#endif
				        // Check that icon is on the disk
				        if (!(lock=Lock(handle->func_work_buf,ACCESS_READ)))
				        {
				            // It's not there; remove from reject list
				            remove_file_entry(buffer,icon_entry);
				            icon_entry=0;
				        }
				        else UnLock(lock);
			            }
		                    else
			            {
			                icon_entry=0;
			            }
			        }

			        if (org_dir)
			            CurrentDir(org_dir);

			        // Did we get an icon?
			        if (icon_entry)
			        {
			            // Allocate new entry
			            if (funcentry=function_new_entry(handle,icon_entry->de_Node.dn_Name,0))
			            {
				        // Fill out entry
				        funcentry->fe_entry=icon_entry;
				        funcentry->fe_type=icon_entry->de_Node.dn_Type;
				        funcentry->fe_flags=FUNCENTF_ICON|FUNCENTF_TOP_LEVEL;

				        // Link?
				        if (icon_entry->de_Flags&ENTF_LINK)
				            funcentry->fe_flags|=FUNCENTF_LINK;

				        // Increment count
				        ++handle->entry_count;
				        if (funcentry->fe_type>0)
				        {
				            ++handle->dir_count;
				            if (funcentry->fe_flags&FUNCENTF_LINK)
					        ++handle->link_dir_count;
				        }
				        else
				        {
				            ++handle->file_count;
				            if (funcentry->fe_flags&FUNCENTF_LINK)
					        ++handle->link_file_count;
				        }

				        // Mark this icon to skip it later
				        icon_entry->de_Flags|=ENTF_SKIP;
			            }
			        }
		            }
		        }
	            }
	        }
	    }

	    // Go through source buffer again
	    for (entry=(DirEntry *)buffer->entry_list.mlh_Head;
	         entry->de_Node.dn_Succ;
	         entry=(DirEntry *)entry->de_Node.dn_Succ)
	    {
	        // Is this an icon we skipped before?
	        if (entry->de_Flags&ENTF_SKIPPED)
	        {
	            // Should we process it?
	            if (!(entry->de_Flags&ENTF_SKIP))
	            {
		        // Allocate new function entry
		        if (funcentry=function_new_entry(handle,entry->de_Node.dn_Name,0))
		        {
		            // Fill out entry
		            funcentry->fe_entry=entry;
		            funcentry->fe_type=entry->de_Node.dn_Type;
		            funcentry->fe_flags=FUNCENTF_TOP_LEVEL;

		            // Link?
		            if (entry->de_Flags&ENTF_LINK)
			        funcentry->fe_flags|=FUNCENTF_LINK;

		            // Increment count
		            ++handle->entry_count;
		            if (funcentry->fe_type>0)
		            {
			        ++handle->dir_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_dir_count;
		            }
		            else
		            {
			        ++handle->file_count;
			        if (funcentry->fe_flags&FUNCENTF_LINK)
			            ++handle->link_file_count;
		            }
		        }
	            }

	            // Clear skip flags
	            entry->de_Flags&=~(ENTF_SKIPPED|ENTF_SKIP);
	        }
	    }

	    // Unlock buffer
	    buffer_unlock(buffer);
	}
    }

    // Initialise current entry pointer
    handle->current_entry=(FunctionEntry *)handle->entry_list.lh_Head;

    // Return entry count
    return handle->entry_count;
}


// Add a new entry to a function entry list
FunctionEntry *function_new_entry(FunctionHandle *handle,
	                          char *name,
	                          BOOL icon)
{
    FunctionEntry *entry;

    // Allocate space for new entry and name
    if (!name ||
	!(entry=AllocMemH(handle->entry_memory,sizeof(FunctionEntry)+strlen(name)+((icon)?6:1))))
	return 0;

    // Get name pointer and copy name
    entry->fe_name=(char *)(entry+1);
    strcpy(entry->fe_name,name);
    if (icon) strcat(entry->fe_name,".info");

    // Add to entry list
    AddTail(&handle->entry_list,(struct Node *)entry);

    // Return entry pointer
    return entry;
}


// Get current entry
FunctionEntry *function_current_entry(FunctionHandle *handle)
{
    // Valid entry?
    if (!handle->current_entry || !handle->current_entry->fe_node.mln_Succ)
	return 0;

    // Return entry
    return handle->current_entry;
}


// Get next entry to process
FunctionEntry *function_next_entry(FunctionHandle *handle)
{
//  FunctionEntry *entry;

    // Valid entry?
    if (!handle->current_entry || !handle->current_entry->fe_node.mln_Succ)
	return 0;

    // Move on to next entry
    handle->current_entry=(FunctionEntry *)handle->current_entry->fe_node.mln_Succ;

    return handle->current_entry;
}


// Get the next entry for a function
FunctionEntry *function_get_entry(FunctionHandle *handle)
{
    FunctionEntry *entry = 0;
    ULONG flags=0;

    // If there's a recursively-collected entry waiting, return it
    if (handle->recurse_entry) return handle->recurse_entry;

    // Loop until we get a file or null
    while (1)
    {
	// Are we recursing a directory?
	if (handle->recurse_return==0 &&
	    !(handle->instruction_flags&INSTF_ABORT_DIR))
	{
	    // Fill out entry
	    handle->recurse_entry_data->fe_name=handle->recurse_path;
	    handle->recurse_entry_data->fe_entry=0;
	    handle->recurse_entry_data->fe_flags=flags;

	    // Copy entry info
	    handle->recurse_info=handle->anchor->ap_Info;

	    // Link?
	    if (handle->anchor->ap_Info.fib_DirEntryType==ST_LINKFILE ||
		handle->anchor->ap_Info.fib_DirEntryType==ST_LINKDIR ||
		handle->anchor->ap_Info.fib_DirEntryType==ST_SOFTLINK)
	    {
		handle->recurse_entry_data->fe_flags|=FUNCENTF_LINK;

		if (handle->anchor->ap_Info.fib_DirEntryType == ST_SOFTLINK)
		{
		    struct FileInfoBlock __aligned link_fib;
		    //BPTR link, link_parent, org_dir;
		    BPTR link, org_dir;

		    //link_parent = DupLock(handle->anchor->ap_Last->an_Lock);
		    org_dir = CurrentDir(handle->anchor->ap_Last->an_Lock);
		    if (link = Lock(handle->anchor->ap_Info.fib_FileName, ACCESS_READ))
		    {
			Examine(link, &link_fib);
			if (link_fib.fib_DirEntryType < 0)
			    handle->recurse_entry_data->fe_flags |= FUNCENTF_LINK_FILE;
			UnLock(link);
		    }

		    CurrentDir(org_dir);
		}
	    }
	    else
		handle->recurse_entry_data->fe_flags&=~FUNCENTF_LINK;

	    // Is this a file or valid directory?
	    if (handle->anchor->ap_Info.fib_DirEntryType<0 ||
	        !(handle->anchor->ap_Flags&APF_DIDDIR))
	    {
		// Copy filename
		strcpy(handle->recurse_path,handle->anchor->ap_Info.fib_FileName);

		// Do we want this entry?
		if (handle->anchor->ap_Info.fib_DirEntryType<0 ||
		    handle->recurse_entry_data->fe_flags&FUNCENTF_LINK_FILE ||
		    handle->instruction_flags&INSTF_WANT_DIRS)
		{
		    // Point to dummy entry
		    handle->recurse_entry=handle->recurse_entry_data;
		    handle->recurse_entry->fe_flags|=FUNCENTF_RECURSE;

		    // Get parent lock
		    handle->recurse_entry->fe_lock=DupLock(handle->anchor->ap_Last->an_Lock);

		    // File?
		    if (handle->anchor->ap_Info.fib_DirEntryType<0 ||
			handle->recurse_entry_data->fe_flags&FUNCENTF_LINK_FILE)
		    {
			// If we have a file filter, compare it
			if (handle->got_filter==2 &&
			    !(MatchPatternNoCase(handle->file_filter_parsed,
					         handle->anchor->ap_Info.fib_FileName)))
			{
			    // Failed filter
			    handle->recurse_entry=0;
			    handle->recurse_entry_data->fe_flags&=~FUNCENTF_RECURSE;
			}

			// Ok to use
			else
			{
			    // Set entry type
			    handle->recurse_entry->fe_type=ENTRY_FILE;

			    // Set entry size
			    handle->recurse_entry->fe_size=handle->anchor->ap_Info.fib_Size;

			    // Increment counts
			    ++handle->recurse_count;
			    handle->recurse_bytes+=handle->anchor->ap_Info.fib_Size;

			    // Calculate block size?
			    if (handle->dest_block_size)
			    {
				long fileListEntries;
				long dataBlocks;
				long fileLists;
				long totalBlocks;

				// Calculate block size of file
				fileListEntries=(handle->dest_block_size>>2)-56;
				dataBlocks=(handle->anchor->ap_Info.fib_Size+handle->dest_data_block_size-1)/
					    handle->dest_data_block_size;
				fileLists=(dataBlocks+fileListEntries-1)/fileListEntries;
				totalBlocks=dataBlocks+fileLists;

				// Increment count
				handle->dest_recurse_blocks+=totalBlocks;
			    }
			}
		    }

		    // Set entry type for directory
		    else
		    {
			handle->recurse_entry->fe_type=ENTRY_DIRECTORY;
			handle->recurse_entry->fe_size=0;
		    }
		}

		// Entering a new directory?
		if (handle->anchor->ap_Info.fib_DirEntryType>0 &&
		    !(handle->recurse_entry_data->fe_flags&FUNCENTF_LINK_FILE))
		{
		    BPTR lock, current;
		    BPTR tmp_lock;

		    tmp_lock = DupLock(handle->anchor->ap_Last->an_Lock);
		    current = CurrentDir(tmp_lock);

		    // Check path exists
		    if (lock=Lock((char *)handle->anchor->ap_Info.fib_FileName,ACCESS_READ))
		    {
			// It does
			UnLock(lock);

		        // Replace path
		        if (current)
			    strcpy(handle->anchor_path,handle->anchor->ap_Info.fib_FileName);

			// Set bit to enter dir, increment depth count
			handle->anchor->ap_Flags|=APF_DODIR;
			++handle->recurse_depth;
			++handle->dest_recurse_blocks;
		    }

		    // Doesn't exist, something went wrong, skip over it
		    else
		    {
			// Don't enter
			handle->anchor->ap_Flags&=~APF_DODIR;
		    }

		    if (current)
			UnLock(CurrentDir(current));

		}
	    }

	    // Returning from a previous directory
	    else
	    if (handle->anchor->ap_Info.fib_DirEntryType>0)
	    {
		// Clear anchor bits
		handle->anchor->ap_Flags&=~APF_DIDDIR;

		// Decrement depth count
		--handle->recurse_depth;

		{
		    BPTR tmp_lock;
		    D_S(struct FileInfoBlock,tmp_fib);

		    tmp_lock = DupLock(handle->anchor->ap_Last->an_Lock);

		    Examine(tmp_lock,tmp_fib);
		    strcpy(handle->anchor_path,tmp_fib->fib_FileName);

		    UnLock(tmp_lock);
		}

		// Not the top-level directory?
		if (handle->recurse_depth>0)
		{
		    // Do we want this dir now?
		    if (handle->instruction_flags&INSTF_WANT_DIRS_END)
		    {
		        // Copy filename
		        strcpy(handle->recurse_path,handle->anchor->ap_Info.fib_FileName);

		        // Get parent lock
		        handle->recurse_entry_data->fe_lock = DupLock(handle->anchor->ap_Last->an_Lock);

			// Point to dummy entry
			handle->recurse_entry=handle->recurse_entry_data;

			// Set entry type and flags
			handle->recurse_entry->fe_type=ENTRY_DIRECTORY;
			handle->recurse_entry->fe_flags=flags|FUNCENTF_RECURSE|FUNCENTF_EXITED;
			handle->recurse_entry->fe_size=0;
		    }
		}

		// Otherwise, set exited flag for the next entry
		else flags=FUNCENTF_EXITED;
	    }

#ifdef RESOURCE_TRACKING
	    RT_DiscardLock(handle->anchor->ap_Last->an_Lock, __FILE__, __LINE__);
#endif

	    // Get the next match
	    handle->recurse_return=MatchNext(handle->anchor);

#ifdef RESOURCE_TRACKING
	    if (!handle->recurse_return)
		RT_InjectLock(handle->anchor->ap_Last->an_Lock, __FILE__, __LINE__);
#endif

	    // Return entry
	    if (handle->recurse_entry)
	        return handle->recurse_entry;
	    else continue;
	}

	// Were we recursing a directory?
	if (handle->recurse_return!=-1 ||
	    (handle->instruction_flags&INSTF_ABORT_DIR))
	{
	    FunctionEntry *entry;

	    // Get current entry
	    if (entry=function_current_entry(handle))
	    {
		// Is entry an entry in a lister (and not aborted)?
		if (entry->fe_entry &&
		    !(entry->fe_flags&FUNCENTF_ICON_ACTION) &&
		    !(handle->instruction_flags&INSTF_ABORT_DIR))
		{
		    DirBuffer *buffer;

		    // Make sure there's a valid buffer
		    if (buffer=handle->func_source_paths.current->pn_lister->cur_buffer)
		    {
			// Allowed to count sizes?
			if (!(handle->instruction_flags&INSTF_DIR_NO_SIZES))
			{
			    // Subtract current size
			    buffer->buf_TotalBytes[0]-=entry->fe_entry->de_Size;
			    if (entry->fe_entry->de_Flags&ENTF_SELECTED)
				buffer->buf_SelectedBytes[0]-=entry->fe_entry->de_Size;
			    entry->fe_entry->de_Size=0;
			    entry->fe_entry->de_Flags|=ENTF_NO_SIZE;

			    // Not just clearing sizes
			    if (!(handle->instruction_flags&INSTF_DIR_CLEAR_SIZES) &&
				handle->got_filter!=2)
			    {
				// Add new size to totals
				entry->fe_entry->de_Size=handle->recurse_bytes;
				buffer->buf_TotalBytes[0]+=entry->fe_entry->de_Size;
				if (entry->fe_entry->de_Flags&ENTF_SELECTED)
				    buffer->buf_SelectedBytes[0]+=entry->fe_entry->de_Size;
				entry->fe_entry->de_Flags&=~ENTF_NO_SIZE;
			    }

			    // Store block size
			    if (handle->dest_block_size)
			    {
				entry->fe_entry->de_dest_blocksize=handle->dest_data_block_size;
				entry->fe_entry->de_block_total=handle->dest_recurse_blocks;
			    }
			}
		    }
		}
	    }

	    // End recurse
	    MatchEnd(handle->anchor);
	    handle->recurse_return=-1;
	    handle->recurse_count=0;
	    handle->recurse_bytes=0;
	    handle->recurse_depth=0;
	    handle->dest_recurse_blocks=0;

	    // Fix CD
	    if (handle->recurse_cd)
	    {
		BPTR lock=CurrentDir(handle->recurse_cd);
		UnLock(lock);
		handle->recurse_cd=0;
	    }

	    // Want this directory?
	    if (handle->instruction_flags&INSTF_WANT_DIRS_END &&
	        !(handle->instruction_flags&INSTF_ABORT_DIR))
	    {
		// Set flags
		if (entry) entry->fe_flags|=FUNCENTF_EXITED;
	    }

	    else
	    {
		BOOL ok=1;

		// Aborted?
		if (handle->instruction_flags&INSTF_ABORT_DIR)
		{
		    // Clear abort dir flag
		    handle->instruction_flags&=~INSTF_ABORT_DIR;
		    ok=0;
		}

		// Get next entry
		function_end_entry(handle,entry,ok);
	    }
	}

	// Get next entry as normal
	if (!(entry=function_current_entry(handle)))
	    return 0;

	// Is this a directory?
	if (entry->fe_type>=ENTRY_DEVICE && !(entry->fe_flags&FUNCENTF_EXITED))
	{
	    // Recursing directories?
	    if (handle->instruction_flags&INSTF_RECURSE_DIRS)
	    {
		// Set flag to enter this entry
		entry->fe_flags|=FUNCENTF_ENTERED;

		// Go straight in?
		if (!(handle->instruction_flags&INSTF_WANT_DIRS))
		{
		    function_end_entry(handle,entry,1);
		    continue;
		}
	    }
	}

	break;
    }

    // Set flags
    if (entry) entry->fe_flags|=flags;

    // Return entry
    return entry;
}


// Finished with an entry
function_end_entry(FunctionHandle *handle,
	           FunctionEntry *entry,
	           int deselect)
{
    int count=0;

    // Valid entry?
    if (entry)
    {
	// Just exited a previous directory
	if (entry->fe_flags&FUNCENTF_EXITED)
	{
	    entry->fe_flags&=~FUNCENTF_EXITED;
	}

	// Recursive entry?
	if (entry->fe_flags&FUNCENTF_RECURSE)
	{
	    // Clear recursive entry pointer
	    handle->recurse_entry=0;

	    if (entry->fe_lock)
	    {
		UnLock(entry->fe_lock);
		entry->fe_lock=0;
	    }
	}

	// Entry we must enter
	else
	if (entry->fe_flags&FUNCENTF_ENTERED && (deselect>0))
	{
	    BPTR lock;

	    // Clear entered flag
	    entry->fe_flags&=~FUNCENTF_ENTERED;

	    // Initialise anchor path
	    handle->anchor->ap_BreakBits=SIGBREAKF_CTRL_C;
	    handle->anchor->ap_Flags=0;

	    // Lock directory
	    if (entry->fe_flags&FUNCENTF_MULTI_ASSIGN || entry->fe_type == ENTRY_DEVICE)
		lock = DupLock(entry->fe_lock);
	    else
	    {
		handle->recurse_cd = CurrentDir((entry->fe_lock)?entry->fe_lock:handle->func_source_lock);
		lock = Lock(entry->fe_name,ACCESS_READ);
	    }

	    if (lock)
	    {
		BPTR tmplock;

		tmplock = CurrentDir(lock);

		if (!handle->recurse_cd)
		    handle->recurse_cd = tmplock;
	    }

	    handle->recurse_return = MatchFirst("#?", handle->anchor);

#ifdef RESOURCE_TRACKING
	    if (!handle->recurse_return)
		RT_InjectLock(handle->anchor->ap_Last->an_Lock, __FILE__, __LINE__);
#endif

	    handle->recurse_depth=1;
	    strcpy(handle->anchor_path,entry->fe_name);
	}

	// Next entry
	else
	{
	    // Deselect if required
	    if (deselect>0) entry->fe_flags|=FUNCENTF_UNSELECT;

	    // Clear entered flag
	    entry->fe_flags&=~FUNCENTF_ENTERED;

	    // Get next entry
	    entry=function_next_entry(handle);
	    ++count;

	    // Did this file "fail"?
	    if (!deselect && entry)
	    {
		// Is the new entry an icon?
		if (entry->fe_flags&FUNCENTF_ICON)
		{
		    // Skip it
		    function_next_entry(handle);
		    ++count;
		}
	    }
	}
    }
    return count;
}


// Ask for file filter
function_check_filter(FunctionHandle *handle)
{
    char *filter;
    short ret;

    if (!(filter=AllocVec(256,0)))
	return 0;

    // Initialise filter
    GetSemaphore(&GUI->filter_lock,SEMF_SHARED,0);
    strcpy(filter,GUI->filter_string);
    FreeSemaphore(&GUI->filter_lock);

    // Ask for filter
    if (!(ret=function_request(
	handle,
	GetString(&locale,MSG_ENTER_FILTER_STRING),
	SRF_BUFFER|SRF_PATH_FILTER,
	filter,255,
	GetString(&locale,MSG_OKAY),
	GetString(&locale,MSG_ALL),
	GetString(&locale,MSG_ABORT),0)))
    {
	FreeVec(filter);
	return 0;
    }

    // Valid filter entered?
    if (ret!=2)
    {
	// Replace * with #?
	if (filter[0]=='*' && filter[1]==0)
	{
	    filter[0]='#';
	    filter[1]='?';
	    filter[2]=0;
	}

	// Get filter
	strcpy(handle->file_filter,filter);

	// Copy into global filter
	GetSemaphore(&GUI->filter_lock,SEMF_EXCLUSIVE,0);
	strcpy(GUI->filter_string,filter);
	FreeSemaphore(&GUI->filter_lock);
    }

    // If it's #?, set flag
    if (handle->file_filter[0]=='#' &&
	handle->file_filter[1]=='?' &&
	handle->file_filter[2]==0)
    {
	handle->got_filter=1;
    }

    // Set flag to say we've got filter
    else handle->got_filter=2;

    // Parse filter
    ParsePatternNoCase(handle->file_filter,handle->file_filter_parsed,64);
    FreeVec(filter);
    return 1;
}


// Convert glargs into external filenames
void function_files_from_glarg(FunctionHandle *handle)
{
    struct WBArg *arg;
    struct GLAData *argdata;
    ExternalEntry *entry;
    ULONG args,currarg=0;

    // No lister appmessage?
    if (!handle->lister_app_msg || !(args = handle->lister_app_msg->glam_NumArgs)) return;

    // Go through arguments
    while (currarg < args)
    {
	BPTR lock;

	arg = &handle->lister_app_msg->glam_ArgList[currarg];
	argdata = &handle->lister_app_msg->glam_ArgData[currarg];

	// Get lock
	if (!(lock = arg->wa_Lock))
	    lock = handle->lister_app_msg->glam_Lock;

	// Create a new external entry
	if (entry=new_external_entry(handle,arg->wa_Name,lock))
	{
	    // Directory?
	    if (argdata->glad_Flags&AAEF_DIR)
		entry->een_type=ENTRY_DIRECTORY;

	    // Device?
	    if (argdata->glad_Flags&AAEF_DEV)
	    {
		entry->een_type=ENTRY_DEVICE;
		handle->flags|=FUNCF_COPY_NO_MOVE;
	    }

	    // Link?
	    if (argdata->glad_Flags&AAEF_LINK)
		entry->een_flags|=FUNCENTF_LINK;

	    // Assign?
	    if (argdata->glad_Flags&AAEF_ASSIGN)
	    {
		entry->een_flags|=FUNCENTF_ASSIGN;
		handle->flags|=FUNCF_COPY_NO_MOVE;
	    }

	    // Multi directory assign?
	    if (argdata->glad_Flags&AAEF_MULTI_ASSIGN)
	    {
		entry->een_flags|=FUNCENTF_MULTI_ASSIGN;
		handle->flags|=FUNCF_COPY_NO_MOVE;
	    }

	    // Fake icon?
	    if (argdata->glad_Flags&AAEF_FAKE_ICON)
		entry->een_flags|=FUNCENTF_FAKE_ICON;

	    // Add to external entry list
	    AddTail((struct List *)&handle->external_list,(struct Node *)entry);
	}

	currarg++;
    }

    handle->func_flags |= FUNCF_GOT_SOURCE;
}


// Convert wbargs into external filenames
void function_files_from_wbarg(FunctionHandle *handle)
{
    //struct ArgArrayEntry *arg;
    struct WBArg *arg;
    ExternalEntry *entry;
    ULONG args,currarg=0;
    char *name;

    // No arguments?
    if (!handle->app_msg || !(args = handle->app_msg->am_NumArgs)) return;

    // Go through arguments
    while (currarg < args)
    {
	struct FileInfoBlock __aligned tmp_fib;
	BYTE isroot = FALSE;

	arg = &handle->app_msg->am_ArgList[currarg];

	Examine(arg->wa_Lock,&tmp_fib);

	if (WBArgDir(arg))
	{
	    BPTR parent_lock;

	    // Get directory name
	    name = tmp_fib.fib_FileName;

	    // Try to get parent dir
	    if (parent_lock = ParentDir(arg->wa_Lock))
	    {
		UnLock(arg->wa_Lock);
		arg->wa_Lock = parent_lock;
	    }
	    else
	    {
		UBYTE len;

		len = strlen(name);
		name[len] = ':';
		name[len+1] = 0;
		isroot = TRUE;
	    }

	}
	else
	    name = arg->wa_Name;

	// Create a new external entry
	if (entry=new_external_entry(handle,name,arg->wa_Lock))
	{

	    // Directory?
	    if (WBArgDir(arg))
	    entry->een_type=1;

	    // Device?
	    if (isroot)
	    {
		entry->een_type=ENTRY_DEVICE;
		handle->flags|=FUNCF_COPY_NO_MOVE;
	    }
	    // Link?
	    //else
	    //if ((entrytype == ST_SOFTLINK) || (entrytype == ST_LINKDIR) || (entrytype == ST_LINKFILE))
	    //	  entry->een_flags|=FUNCENTF_LINK;

	    // Fake icon?
	    //if (arg->aae_Flags&AAEF_FAKE_ICON)
	    //	  entry->een_flags|=FUNCENTF_FAKE_ICON;

	    // Add to external entry list
	    AddTail((struct List *)&handle->external_list,(struct Node *)entry);
	}
	currarg++;
    }

    handle->func_flags |= FUNCF_GOT_SOURCE;
}


// Convert arguments into external filenames
void function_files_from_args(FunctionHandle *handle)
{
    struct ArgArrayEntry *arg;
    ExternalEntry *entry;

    // No arguments?
    if (!handle->args) return;

    // Go through arguments
    for (arg=(struct ArgArrayEntry *)handle->args->aa_List.mlh_Head;
	 arg->aae_Node.mln_Succ;
	 arg=(struct ArgArrayEntry *)arg->aae_Node.mln_Succ)
    {
	// Create a new external entry
	if (entry=new_external_entry(handle,arg->aae_String,arg->aae_Lock))
	{
	    // Directory?
	    if (arg->aae_Flags&AAEF_DIR)
		entry->een_type=1;

	    // Link?
	    if (arg->aae_Flags&AAEF_LINK)
		entry->een_flags|=FUNCENTF_LINK;

	    // Fake icon?
	    if (arg->aae_Flags&AAEF_FAKE_ICON)
		entry->een_flags|=FUNCENTF_FAKE_ICON;

	    // Add to external entry list
	    AddTail((struct List *)&handle->external_list,(struct Node *)entry);
	}
    }

    if (arg->aae_Lock)
    handle->func_flags |= FUNCF_GOT_SOURCE;

}


// Build a string with filenames in it
char *function_build_file_string(FunctionHandle *handle,short quotes)
{
    FunctionEntry *entry;
    struct List list;
    struct Node *node;
    short len,length=0;
    char *string;

    // Initialise list
    NewList(&list);

    // Go through entries
    while (entry=function_get_entry(handle))
    {
	// Add name length
	length+=(len=strlen(entry->fe_name)+((quotes)?3:1));

	// Add node
	if (node=AllocMemH(handle->memory,sizeof(struct Node)+len))
	{
	    node->ln_Name=(char *)(node+1);
	    node->ln_Name[0]=(quotes)?'\"':0;
	    strcat(node->ln_Name,entry->fe_name);
	    if (quotes) strcat(node->ln_Name,"\"");
	    AddTail(&list,node);
	}

	// Get next entry
	function_end_entry(handle,entry,-1);
    }

    // Allocate string
    if (!(string=AllocVec(length+1,MEMF_CLEAR)))
	return 0;

    // Go through names
    for (node=list.lh_Head,len=0;node->ln_Succ;node=node->ln_Succ)
    {
	// Add name to string
	strcat(string,node->ln_Name);
	len+=strlen(node->ln_Name);
	string[len++]=' ';
	string[len]=0;
    }

    // Strip final space
    if (len>0 && string[len-1]==' ')
	string[len-1]=0;

    return string;
}


// Create a new external entry
ExternalEntry *new_external_entry(FunctionHandle *handle,char *path, BPTR lock)
{
    ExternalEntry *entry;
    char *ptr;
    short len;

    // Allocate new entry
    if (!(entry=AllocMemH(handle->memory,sizeof(ExternalEntry)+(len=strlen(path)))))
	return 0;

    // Get lock
    entry->een_lock = lock;

    // Copy path
    strcpy(entry->een_path,path);
    entry->een_name=entry->een_path;
    entry->een_type=-1;

    // Get pointer to end of path
    ptr=entry->een_path+len-1;

    // Path ends in a slash?
    if (*ptr=='/')
    {
	// Kill slash
	*ptr=0;
	--ptr;

	// Entry is a directory
	entry->een_type=1;
    }

    // Or it ends in a colon
    else
    if (*ptr==':')
    {
	// Entry is a device
	entry->een_type=1;
	return entry;
    }

    // Find start of last name
    while (ptr>=entry->een_path)
    {
	if (*ptr=='/' || *ptr==':')
	{
	    // Get pointer to filename
	    entry->een_name=ptr+1;
	    *ptr=0;
	    break;
	}
	--ptr;
    }

    return entry;
}
