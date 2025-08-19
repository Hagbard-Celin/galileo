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
#include "app_msg_protos.h"
#include "dirlist_protos.h"
#include "file_select.h"

// Allocate an AppMessage
GalileoListerAppMessage *alloc_lister_appmsg(APTR memory,
					     struct MsgPort *reply,
					     WORD num)
{
    GalileoListerAppMessage *msg;

    // Allocate message
    if (!(msg = AllocMemH(memory,
			    sizeof(GalileoListerAppMessage)+
			    ((sizeof(struct WBArg) + sizeof(struct GLAData)) * num))))
    {
	return 0;
    }

    // Set check pointer
    msg->glam_Class = GLAMCLASS_LISTER;

    // Set ArgList pointer
    msg->glam_NumArgs=num;

    // Set ArgList pointer
    if (num>0) msg->glam_ArgData=(struct GLAData *)(msg->glam_ArgList + num);

    // Fill out miscellaneous fields
    //msg->glam_Message.mn_ReplyPort=reply;
    msg->glam_Version=GLAM_VERSION;

    return msg;
}


// Free an ListerAppMessage
void free_lister_appmsg(GalileoListerAppMessage *msg)
{
    // Valid message?
    if (msg)
    {
	WORD arg;

	if (!(msg->glam_Flags&DROPF_BORROWED_LOCK))
	{
	    UnLock(msg->glam_Lock);
	}
	// Free any arguments
	if (msg->glam_ArgList)
	{
	    for (arg=0;arg<msg->glam_NumArgs;arg++)
	    {
		FreeMemH(msg->glam_ArgList[arg].wa_Name);

		if (msg->glam_ArgList[arg].wa_Lock && !(msg->glam_ArgData[arg].glad_Flags&WBAF_NOT_A_LOCK))
		{
		    UnLock(msg->glam_ArgList[arg].wa_Lock);
		}
	    }
	}

	// Free message
	FreeMemH(msg);
    }
}

// Reply to an AppMessage
void reply_lister_appmsg(GalileoListerAppMessage *msg)
{
	// Valid message?
	if (msg)
	{
		if (msg->glam_Message.mn_ReplyPort)
		{
			ReplyMsg((struct Message *)msg);
		}
		else
		{
			free_lister_appmsg(msg);
		}
	}
}

// Set a GLArg entry
BOOL set_glarg(GalileoListerAppMessage *msg,
	       WORD num,
	       char *name,
	       APTR memory)
{
	// Valid message and number?
	if (!msg || num>msg->glam_NumArgs) return 0;

	// Null-name?
	if (!name) name="";

	// Copy name
	if (msg->glam_ArgList[num].wa_Name=AllocMemH(memory,strlen(name)+1))
		strcpy(msg->glam_ArgList[num].wa_Name,name);

	return 1;
}


// Allocate and fill out an AppMessage
void set_wbargs_files(ULONG numargs,
		      struct WBArg *args,
		      struct GLAData *argsdata,
		      BPTR dir_lock,
		      DirEntry *entry,
		      DirBuffer *buffer)
{
	WORD arg;


	// Any arguments?
	if (numargs>0)
	{
		// If multi-drag, get first selected file
		if (numargs>1) entry=get_entry(&buffer->entry_list,1,ENTRY_ANYTHING);

		// Fill out arguments
		for (arg=0;arg<numargs && entry;arg++)
		{
			// Is entry a directory?
			if (entry->de_Node.dn_Type>=ENTRY_DIRECTORY)
			{
			    if (argsdata)
			    {
				// Set name
				args[arg].wa_Name = CopyString(global_memory_pool, entry->de_Node.dn_Name);

				// Flag as directory
				argsdata[arg].glad_Flags = AAEF_DIR;
			    }
			    else
			    {
				BPTR old;

				// Go to directory
				old=CurrentDir(dir_lock);

				// Null-name
				args[arg].wa_Name = (STRPTR)(args + numargs);

				// Get lock on directory
				if (!(args[arg].wa_Lock=Lock(entry->de_Node.dn_Name,ACCESS_READ)))
				{
					// If a custom handler, just pass name
					if (buffer->buf_CustomHandler[0])
					    args[arg].wa_Name = CopyString(global_memory_pool, entry->de_Node.dn_Name);
				}

				// Return to previous directory
				if (old) CurrentDir(old);
			    }
			}

			// A device?
			// FIXME: Lock????????
			else
			if (entry->de_Node.dn_Type==ENTRY_DEVICE)
			{
				char *ptr;
				BPTR lock_entry;

				// Multi assign entry?
				if (entry->de_SubType == SUBENTRY_MULTI_ASSIGN)
				{
					// Use assign name
					ptr=(char *)GetTagData(DE_AssignName,0,entry->de_Tags);
				}
				// Cache?
				else
				if (entry->de_SubType==SUBENTRY_BUFFER)
				{
					// Use comment if available
					if (!(ptr=(char *)GetTagData(DE_Comment,0,entry->de_Tags)))
					{
						// Otherwise use display string
						ptr=(char *)GetTagData(DE_DisplayString,0,entry->de_Tags);
					}
				}

				// Otherwise, use comment
				else
				{
				    ptr = (char *)GetTagData(DE_Comment,0,entry->de_Tags);
				    ptr = stpcpy(buffer->buf_CurrentLister->work_buffer, ptr);
				    *ptr = ':';
				    ptr++;
				    *ptr = 0;
				    ptr = buffer->buf_CurrentLister->work_buffer;
				}

				// Duplicate lock if this is an assign
				if (entry->de_Size == DLT_DIRECTORY)
				{
				    lock_entry = DupLock((BPTR)entry->de_UserData);
				}
				else
				if (!entry->de_SubType)
				{
				    lock_entry = Lock(entry->de_Node.dn_Name,ACCESS_READ);
				}
				else
				    lock_entry = Lock(ptr, ACCESS_READ);

				// Get lock
				if (lock_entry)
				{
				    if (argsdata)
				    {
				        BPTR lock_parent;

				        // If assign, try locking parent
				        if (entry->de_Size == DLT_DIRECTORY && (lock_parent = ParentDir(lock_entry)))
				        {
					    argsdata[arg].glad_Flags = AAEF_DIR;

					    argsdata[arg].glad_Flags |= AAEF_ASSIGN;

					    if (entry->de_SubType==SUBENTRY_ASSIGN)
					    {

					        UnLock(lock_entry);
					        lock_entry = lock_parent;
					    }
					    else
					    {
					        UnLock(lock_parent);
					    }
				        }
				        else
				        {
					    argsdata[arg].glad_Flags = AAEF_DEV;
				        }

					args[arg].wa_Name = CopyString(global_memory_pool, ptr);

				        if (entry->de_SubType==SUBENTRY_MULTI_ASSIGN)
				        {
					    argsdata[arg].glad_Flags |= AAEF_MULTI_ASSIGN;
				        }
				    }
				    else
				    {
					// Null-name
					args[arg].wa_Name = (STRPTR)(args + numargs);
				    }

				    args[arg].wa_Lock = lock_entry;
				}

				// Set name if not already set
				if ((argsdata || !args[arg].wa_Lock) && !args[arg].wa_Name)
				{
				    args[arg].wa_Name = CopyString(global_memory_pool, entry->de_Node.dn_Name);
				}
			}

			// Otherwise, it's a file
			else
			{
			    args[arg].wa_Name = CopyString(global_memory_pool, entry->de_Node.dn_Name);
			    if (!argsdata)
				args[arg].wa_Lock = DupLock(dir_lock);
			}

			// No lock and non-arexx custom handler? use entry
			if (!args[arg].wa_Lock && buffer->buf_CustomHandler[0] && *(ULONG *)buffer->buf_CustomHandler == CUSTH_TYPE_GFMMODULE)
			{
			    args[arg].wa_Lock = (LONG)entry;
			    argsdata[arg].glad_Flags |= WBAF_NOT_A_LOCK;
			}

			// Deselect entry
			deselect_entry(buffer,entry);

			// Get next entry (for multidrag)
			if (numargs>1) entry=get_entry((struct MinList *)entry,1,ENTRY_ANYTHING);
			else break;
		}
	}
}


// Allocate and fill out an AppMessage
GalileoListerAppMessage *alloc_lister_appmsg_files(
	DirEntry *entry,
	DirBuffer *buffer,
	BOOL multifiles)
{
	GalileoListerAppMessage *msg;

	// Get lock on directory
	if (!buffer->buf_Lock)
	{
	        // If lock failed and we don't have a custom handler, and aren't a special path, fail
	        if (!buffer->buf_CustomHandler[0] &&
		        !(buffer->more_flags&(DWF_DEVICE_LIST|DWF_CACHE_LIST))) return 0;
	}

	// Allocate AppMessage
	if (!(msg=
		alloc_lister_appmsg(
			global_memory_pool,
			GUI->appmsg_port,
			(multifiles)?(buffer->buf_SelectedFiles[0]+buffer->buf_SelectedDirs[0]):1)))
	{
		return 0;
	}

	if (buffer->buf_Lock)
	{
	    msg->glam_Lock = buffer->buf_Lock;
	    msg->glam_Flags|=DROPF_BORROWED_LOCK;
	}

	// Any arguments?
	if (msg->glam_NumArgs>0)
	    set_wbargs_files(msg->glam_NumArgs, msg->glam_ArgList, msg->glam_ArgData, buffer->buf_Lock, entry, buffer);

	return msg;
}

// Allocate and fill out an AppMessage
struct AppMessage *alloc_appmsg_files(
	DirEntry *entry,
	DirBuffer *buffer,
	BOOL multifiles)
{
	struct AppMessage *msg;

	// Get lock on directory
	if (!buffer->buf_Lock)
	{
		// If lock failed and we don't have a custom handler, and aren't a special path, fail
		if (!buffer->buf_CustomHandler[0] &&
			!(buffer->more_flags&(DWF_DEVICE_LIST|DWF_CACHE_LIST))) return 0;
	}

	// Allocate AppMessage
	if (!(msg=
		AllocAppMessage(
			global_memory_pool,
			GUI->appmsg_port,
			(multifiles)?(buffer->buf_SelectedFiles[0]+buffer->buf_SelectedDirs[0]):1)))
	{
		return 0;
	}

	// Any arguments?
	if (msg->am_NumArgs>0)
	    set_wbargs_files(msg->am_NumArgs, msg->am_ArgList, NULL, buffer->buf_Lock, entry, buffer);

	return msg;
}


// Free an array of arguments
void FreeArgArray(struct ArgArray *array)
{
	if (array)
	{
		struct ArgArrayEntry *arg;

		// Go through arguments
		for (arg=(struct ArgArrayEntry *)array->aa_List.mlh_Head;
			arg->aae_Node.mln_Succ;
			arg=(struct ArgArrayEntry *)arg->aae_Node.mln_Succ)
		{
		    if (arg->aae_Lock)
		    {
			UnLock(arg->aae_Lock);
		    }
		}

		// Free arguments and array
		FreeMemHandle(array->aa_Memory);
		FreeVec(array);
	}
}


// Find a WBArg by name and lock
WORD FindGLArg(struct WBArg *args,WORD count,char *name)
{
	WORD num;

	// Go through arguments
	for (num=0;num<count;num++)
	{
		// Compare by name?
		if (args[num].wa_Name && *args[num].wa_Name)
		{
			// Compare by name
			if (strcmp(args[num].wa_Name,name)==0) return num;
		}
	}

	// Not found
	return -1;
}


// Build an arg array
struct ArgArray *BuildArgArray(char *arg,...)
{
	return BuildArgArrayA(&arg);
}

struct ArgArray *BuildArgArrayA(char **args)
{
	struct ArgArray *array;
	WORD num;

	// Allocate arg array
	if (array=NewArgArray())
	{
		// Go through arguments
		for (num=0;args[num];num++)
		{
			struct ArgArrayEntry *entry;

			// Allocate this argument
			if (entry=AllocMemH(array->aa_Memory,sizeof(struct ArgArrayEntry)+strlen(args[num])))
			{
				// Copy path
				strcpy(entry->aae_String,args[num]);

				// Increment array index
				++array->aa_Count;

				// Add to list
				AddTail((struct List *)&array->aa_List,(struct Node *)entry);
			}
		}
	}

	return array;
}


// Allocate a new ArgArray
struct ArgArray *NewArgArray(void)
{
	struct ArgArray *array;

	// Allocate arg array
	if (array=AllocVec(sizeof(struct ArgArray),MEMF_CLEAR))
	{
		// Initialise arg array
		NewList((struct List *)&array->aa_List);

		// Allocate memory handle
		if (!(array->aa_Memory=NewMemHandle(2048,512,MEMF_CLEAR)))
		{
			// Failed	
			FreeVec(array);
			return 0;
		}
	}

	return array;
}


struct ArgArrayEntry *NewArgArrayEntry(struct ArgArray *array,char *string)
{
	struct ArgArrayEntry *entry;

	// Allocate entry
	if (entry=AllocMemH(array->aa_Memory,sizeof(struct ArgArrayEntry)+strlen(string)))
	{
		// Copy path
		strcpy(entry->aae_String,string);

		// Add to list
		AddTail((struct List *)&array->aa_List,(struct Node *)entry);
	}

	return entry;
}
