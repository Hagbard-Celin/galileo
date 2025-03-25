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
#include "/Modules/modules.h"

// PRINTDIR internal function
GALILEOFM_FUNC(function_printdir)
{
	PathNode *path;
	Lister *lister;
	BPTR lock;
	APTR outfile;
	struct Library *ModuleBase;

	// Get source path
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Open temporary output file
	lsprintf(handle->func_work_buf+800,"T:Galileo-tmp.%lx",handle);
	if (!(outfile=OpenBuf(handle->func_work_buf+800,MODE_NEWFILE,1024)))
		return 0;

	// Output path
	WriteBuf(outfile,path->pn_path,-1);
	WriteBuf(outfile,"\n\n",2);

	// Valid lister?
	if ((lister=path->pn_lister))
	{
		DirBuffer *buffer;
		DirEntry *entry;

		// Lock lister buffer
		buffer_lock((buffer=lister->cur_buffer),FALSE);

		// Go through buffer
		for (entry=(DirEntry *)buffer->entry_list.mlh_Head;
			entry->de_Node.dn_Succ;
			entry=(DirEntry *)entry->de_Node.dn_Succ)
		{
			short len;

			// Build display string for this entry
			builddisplaystring(entry,handle->func_work_buf,lister);

			// Find last character in string
			for (len=strlen(handle->func_work_buf)-1;len>=0;len--)
			{
				if (handle->func_work_buf[len]!=' ')
				{
					handle->func_work_buf[len+1]=0;
					break;
				}
			}

			// Output to temporary file
			WriteBuf(outfile,handle->func_work_buf,-1);
			WriteBuf(outfile,"\n",1);
		}

		// Unlock buffer
		buffer_unlock(buffer);
	}

	// Otherwise, lock directory
	else if (lock=Lock(path->pn_path,ACCESS_READ))
	{
		// Examine directory
		Examine(lock,handle->s_info);

		// Go through directory
		while (ExNext(lock,handle->s_info))
		{
			// Get size
			if (handle->s_info->fib_DirEntryType<0)
			{
				if (handle->s_info->fib_Size>0)
					lsprintf(handle->temp_buffer,"%ld",handle->s_info->fib_Size);
				else strcpy(handle->temp_buffer,GetString(&locale,MSG_EMPTY));
			}
			else strcpy(handle->temp_buffer,"<dir>");

			// Get protection and date strings
			protect_get_string(handle->s_info->fib_Protection,handle->temp_buffer+20);
			date_build_string(&handle->s_info->fib_Date,handle->temp_buffer+40,1);

			// Build output string for this file
			lsprintf(handle->func_work_buf,
				"%-24s%8s %8s %s\n",
				handle->s_info->fib_FileName,
				handle->temp_buffer,
				handle->temp_buffer+20,
				handle->temp_buffer+40);

			// Write output
			WriteBuf(outfile,handle->func_work_buf,-1);

			// Comment?
			if (handle->s_info->fib_Comment[0])
			{
				WriteBuf(outfile,": ",2);
				WriteBuf(outfile,handle->s_info->fib_Comment,-1);
				WriteBuf(outfile,"\n",1);
			}
		}

		// Unlock directory
		UnLock(lock);
	}

	// Close output file
	CloseBuf(outfile);

	// Open print module
	if (ModuleBase=OpenModule("print.gfmmodule"))
	{
		struct List list;
		struct Node node;

		// Initialise fake list
		NewList(&list);
		node.ln_Name=handle->func_work_buf+800;
		AddTail(&list,&node);

		// Print file
		Module_Entry(&list,GUI->screen_pointer,handle->ipc,&main_ipc,0,0,0);

		// Close print module
		CloseLibrary(ModuleBase);
	}

	// Delete temporary file
	DeleteFile(handle->func_work_buf+800);
	return 1;
}
