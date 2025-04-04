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

// GETSIZES, CHECKFIT, CLEARSIZES
GALILEOFM_FUNC(function_getsizes)
{
	FunctionEntry *entry;
	PathNode *path;
	short ret=1,count=1;
	long total_blocks,dest_blocks;
	char *req_text=0;
	short clear=0;
	Lister *lister;
	DirBuffer *buffer=0;

	// Get current lister
	if (lister=function_lister_current(&handle->func_source_paths))
		buffer=lister->cur_buffer;

	// Clear sizes?
	if (command->function==FUNC_UNBYTE)
	{
		clear=2;
		handle->instruction_flags=INSTF_WANT_DIRS;
	}
	else
	{
		// Allow recursive directories
		handle->instruction_flags=INSTF_RECURSE_DIRS|INSTF_WANT_DIRS|INSTF_WANT_DIRS_END;

		// GetSizes with clear option?
		if (instruction->ipa_funcargs &&
			instruction->ipa_funcargs->FA_Arguments[0]) clear=1;
	}

	// Display progress requester
	if (clear<2)
		function_progress_on(
			handle,
			GetString(&locale,MSG_SCANNING_DIRECTORIES),
			handle->entry_count,
			PWF_FILENAME|PWF_GRAPH);

	// If check fit, allocate requester text
	if (command->function==FUNC_CHECKFIT &&
		(req_text=AllocVec(2048,0)))
	{
		strcpy(req_text,GetString(&locale,MSG_CHECK_FIT_RESULTS));
	}

	// Go through destination paths
	path=function_path_next(&handle->func_dest_paths);
	FOREVER
	{
		// Zero destination blocks
		total_blocks=0;
		dest_blocks=-1;

		// Got a destination path (for checkfit)?
		if (path && command->function==FUNC_CHECKFIT)
		{
			BPTR lock;
			struct InfoData __aligned info;

			// RAM?
			if (strncmp(path->pn_path,"RAM:",4)==0)
			{
				// Kludge for RAM
				dest_blocks=(AvailMem(MEMF_ANY)>>10)-2;
				info.id_BytesPerBlock=1024;
				info.id_DiskType=ID_FFS_DISK;
			}

			// Get disk information
			else
			if (lock=Lock(path->pn_path,ACCESS_READ))
			{
				// Get info
				Info(lock,&info);
				UnLock(lock);

				// Number of blocks free
				dest_blocks=info.id_NumBlocks-info.id_NumBlocksUsed;
			}

			// Couldn't lock; assume FFS
			else
			{
				info.id_BytesPerBlock=512;
				info.id_DiskType=ID_FFS_DISK;
			}

			// Get block size
			handle->dest_block_size=info.id_BytesPerBlock;
			handle->dest_data_block_size=info.id_BytesPerBlock;

			// Old file system?
			if (info.id_DiskType==ID_DOS_DISK ||
				info.id_DiskType==ID_INTER_DOS_DISK ||
				info.id_DiskType==ID_FASTDIR_DOS_DISK ||
                info.id_DiskType==ID_LONG_DOS_DISK) handle->dest_data_block_size-=24;
		}

		// Otherwise, assume FFS for fun
		else
		{
			handle->dest_block_size=512;
			handle->dest_data_block_size=512;
		}

		// Go through files
		while (entry=function_get_entry(handle))
		{
			short file_ok=0;
			DirEntry *dirent=0;

			// Check abort
			if (function_check_abort(handle))
			{
				ret=0;
				break;
			}

			// Get directory entry
			if (entry->fe_entry) dirent=entry->fe_entry;
			else
			if (buffer)
			{
				dirent=find_entry(&buffer->entry_list,entry->fe_name,0,buffer->more_flags&DWF_CASE);
				entry->fe_entry=dirent;
			}

			// Got entry?
			if (dirent)
			{
				// Top-level directory?
				if (entry->fe_type>=ENTRY_DIRECTORY &&
					(clear==2 || entry->fe_flags&FUNCENTF_ENTERED) &&
					!(entry->fe_flags&FUNCENTF_ICON_ACTION))
				{
					// Clear size?
					if (clear)
					{
						// Got a buffer?
						if (buffer)
						{
							// Remove from buffer counts
							buffer->buf_TotalBytes[0]-=dirent->de_Size;
	
							// Selected?
							if (dirent->de_Flags&ENTF_SELECTED)
								buffer->buf_SelectedBytes[0]-=dirent->de_Size;
						}

						// Clear size
						dirent->de_Size=0;
						dirent->de_Flags&=~ENTF_NO_SIZE;
						file_ok=0;
					}

					// Not clear sizes command?
					if (clear<2)
					{
						// Update progress indicator
						function_progress_update(handle,entry,count);

						// Ok to enter
						file_ok=1;

						// Already have a size?
						if (dirent->de_Size>0)
						{
							// If check fit, see if block size is ok
							if (command->function!=FUNC_CHECKFIT ||
								dirent->de_dest_blocksize==handle->dest_data_block_size)
							{
								// Don't need to do directory
								total_blocks+=dirent->de_block_total;
								file_ok=0;
							}
						}
					}
				}

				// Check-fit?
				if (command->function==FUNC_CHECKFIT)
				{
					// Exited directory?
					if (entry->fe_flags&FUNCENTF_EXITED && !(entry->fe_flags&FUNCENTF_RECURSE))
					{
						// Add count to total
						total_blocks+=dirent->de_block_total;
					}

					// Or a top-level file
					else
					if (entry->fe_type<0 && !(entry->fe_flags&FUNCENTF_ICON_ACTION))
					{
						long fileListEntries;
						long dataBlocks;
						long fileLists;
						long totalBlocks;

						// Calculate block size of file
						fileListEntries=(handle->dest_block_size>>2)-56;
						dataBlocks=
							(dirent->de_Size+handle->dest_data_block_size-1)/
							handle->dest_data_block_size;
						fileLists=
							(dataBlocks+fileListEntries-1)/fileListEntries;
						totalBlocks=dataBlocks+fileLists;

						// Increment count
						total_blocks+=totalBlocks;
					}
				}
			}

			// Get next entry
			count+=function_end_entry(handle,entry,file_ok);
		}

		// Aborted?
		if (!ret) break;

		// Doing check fit?
		if (req_text)
		{
			// Got available blocks?
			if (dest_blocks>-1)
			{
				short percent;
				//double pct;

				// Fit completely?
				if (dest_blocks>=total_blocks) percent=100;
				else if (dest_blocks==0) percent=0;
				else
				{
					// Get percent that will fit
					//pct=dest_blocks*100;
					//percent=(short)(pct/(double)total_blocks);
                    percent=(short)((dest_blocks*100)/total_blocks);
				}

				// Build string
				lsprintf(handle->func_work_buf,
					"\n%s %s %ld%%",
					path->pn_path,
					GetString(&locale,MSG_FIT),
					percent);
			}

			// No
			else lsprintf(handle->func_work_buf,
				"%ld %s",
				total_blocks,
				GetString(&locale,MSG_BLOCKS_NEEDED));

			// Add to requester text
			strcat(req_text,handle->func_work_buf);
		}

		// Done with this path
		function_path_end(handle,&handle->func_dest_paths,0);

		// Get next path
		if (command->function!=FUNC_CHECKFIT ||
			!(path=function_path_next(&handle->func_dest_paths)))
			break;

		// Start with first entry again
		handle->current_entry=(FunctionEntry *)handle->entry_list.lh_Head;
	}

	// Got a requester?
	if (req_text)
	{
		// If not aborted, show requester
		if (ret)
		{
			function_request(
				handle,
				req_text,
				0,
				GetString(&locale,MSG_OKAY),0);
		}
		FreeVec(req_text);
	}
	return ret;
}
