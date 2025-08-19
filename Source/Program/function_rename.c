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

enum
{
	RENAME_NAME,
	RENAME_NEWNAME
};

typedef struct
{
	short	wild_flag;
	short	confirm_each;
	char	new_name_edit[108];
	char	old_name_edit[108];
} RenameData;

// RENAME internal function
GALILEOFM_FUNC(function_rename)
{
	FunctionEntry *entry;
	int ret=1,count=1;
	PathNode *path;
	RenameData *data;
	BOOL progress=0;
	char dest_file[108], new_name[108];

	// Get current source path
	if (!(path=function_path_current(&handle->func_source_paths)))
		return 0;

	// Get pointer to data
	data=(RenameData *)handle->inst_data;

	// Tell this lister to update it's datestamp at the end
	path->pn_flags|=LISTNF_UPDATE_STAMP;

	// Go through entries
	while (entry=function_get_entry(handle))
	{
		BOOL file_ok=1,asked=0;
		BPTR tmplock = 0, org_dir, source_lock = 0;

		// Update progress indicator
		if (function_progress_update(handle,entry,count))
		{
			function_abort(handle);
			ret=0;
			break;
		}

	        // CD to source-file directory
		if (entry->fe_lock)
		    source_lock = entry->fe_lock;
	        else
	        if (path->pn_lock)
		    source_lock = path->pn_lock;

		// CD to source dir
		org_dir = CurrentDir(source_lock);

		//strcpy(source_file,entry->fe_name);

		// Get destination filename
		strcpy(dest_file,entry->fe_name);

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
			if (!data->wild_flag)
			{
				// Name supplied in arguments?
				if (instruction->ipa_funcargs &&
					instruction->ipa_funcargs->FA_Arguments[RENAME_NAME] &&
					instruction->ipa_funcargs->FA_Arguments[RENAME_NEWNAME])
				{
					// Store names
					strcpy(data->old_name_edit,FilePart((char *)instruction->ipa_funcargs->FA_Arguments[RENAME_NAME]));
					strcpy(data->new_name_edit,FilePart((char *)instruction->ipa_funcargs->FA_Arguments[RENAME_NEWNAME]));
					ret=1;
				}

				// Need to ask
				else
				{
					// Ask for a new name
					strcpy(data->new_name_edit,entry->fe_name);
ask_point:
					strcpy(data->old_name_edit,entry->fe_name);
					asked=1;
					if (!(ret=function_request(
						handle,
						GetString(&locale,MSG_ENTER_NEW_NAME),
						SRF_BUFFER|SRF_BUFFER2|SRF_PATH_FILTER,
						data->new_name_edit,GUI->def_filename_length,
						data->old_name_edit,GUI->def_filename_length,
						GetString(&locale,MSG_RENAME),
						GetString(&locale,MSG_SKIP),
						GetString(&locale,MSG_ABORT),0)))
					{
						function_abort(handle);

					        if (org_dir)
						    CurrentDir(org_dir);

					        if (tmplock)
						    UnLock(tmplock);

						break;
					}

					// Skip?
					if (ret==2)
					{
						file_ok=0;
						ret=0;
					}

					// Otherwise
					else
					{
						// Turn -1 into 1
						if (ret==-1) ret=1;
					}
				}

				// Got file?
				if (file_ok)
				{
					// Does the new name contain wildcards?
					if (strchr(data->new_name_edit,'*'))
					{
						// If the old name doesn't, make it a *
						if (!(strchr(data->old_name_edit,'*')))
							strcpy(data->old_name_edit,"*");

						// Set wildcard flag
						data->wild_flag=1;
					}

					// No wildcards
					else
					{
						// Store new name
						strcpy(dest_file,data->new_name_edit);
					}
				}
			}

			// Wildcard rename?
			if (data->wild_flag)
			{
				// Progress indicator not shown yet?
				if (!progress)
				{
					// Turn progress indicator on
					function_progress_on(
						handle,
						GetString(&locale,MSG_PROGRESS_RENAMING),
						handle->entry_count,
						PWF_FILENAME|PWF_GRAPH);
					progress=1;
				}

				// Get new name
				file_ok=rename_get_wild(
					data->old_name_edit,
					data->new_name_edit,
					entry->fe_name,
					dest_file);
			}

			// Still ok?
			if (file_ok)
			{
				// Get new name
				strcpy(new_name,dest_file);
			}
		}

		// Ok to do this file?
		if (file_ok)
		{
			// Check names are different
			if (strcmp(dest_file,entry->fe_name)!=0)
			{
				// Check destination is ok
				if ((stricmp(dest_file,entry->fe_name))==0 ||
					(ret=check_file_destination(handle,entry,dest_file,source_lock,&data->confirm_each))==1)
				{
					short suc=0;

					// Loop while unsuccessful
					while (!suc)
					{
						// Got a lister entry?
						if (entry->fe_entry)
						{
							// Use original function
							suc=OriginalRename(entry->fe_name,dest_file);
						}

						// Otherwise, allow patched function to be used
						else
						{
							suc=Rename(entry->fe_name,dest_file);
						}

						// Successful?
						if (suc)
						{
							ret=1;
							break;
						}

						// Display error
						if (!(ret=function_error(handle,entry->fe_name,MSG_RENAMING,IoErr())) ||
							ret==-1) break;

						if (asked) goto ask_point;
					}
				}

				// Successful?
				if (ret==1)
				{
					BPTR lock;

					// Mark this entry for removal
					entry->fe_flags|=FUNCENTF_REMOVE;

					// Lock the new file
					if (lock=Lock(dest_file,ACCESS_READ))
					{
						// Examine it
						Examine(lock,handle->s_info);

						// Was it a link?
						if (entry->fe_entry && entry->fe_entry->de_Flags&ENTF_LINK)
						{
							// Get new name from user string
							strcpy(handle->s_info->fib_FileName,FilePart(dest_file));

							// Set type appropriately
							handle->s_info->fib_DirEntryType=
								(handle->s_info->fib_DirEntryType<0)?ST_LINKFILE:ST_LINKDIR;
						}

						// Add new file to listers
						function_filechange_addfile(handle,path->pn_path,path->pn_lock,handle->s_info,0,0);

						// Is it a directory?
						if (handle->s_info->fib_DirEntryType>0)
						{
							STRPTR sourcepath, destpath;
							ULONG srclen, dstlen, size;

							srclen = strlen(path->pn_path) + strlen(entry->fe_name) + 2;
							dstlen = strlen(path->pn_path) + strlen(dest_file) + 2;

							size = srclen + dstlen;

							if (sourcepath = AllocMem(size,MEMF_ANY))
							{
							    destpath = sourcepath + srclen;
							    strcpy(sourcepath, path->pn_path);
							    strcat(sourcepath, entry->fe_name);
							    sourcepath[srclen - 2] = '/';
							    sourcepath[srclen - 1] = 0;
							    strcpy(destpath, path->pn_path);
							    strcat(destpath, dest_file);
							    destpath[dstlen - 2] = '/';
							    destpath[dstlen - 1] = 0;
							    function_filechange_rename(handle, sourcepath, NULL, destpath);
							    FreeMem(sourcepath, size);
							}

						}
						UnLock(lock);
					}
				}

				// File has failed
				else file_ok=0;
			}
		}

	        // Restore current directory
	        if (org_dir)
		    CurrentDir(org_dir);

	        if (tmplock)
		    UnLock(tmplock);

		// Aborted?
		if (ret==-1)
		{
			function_abort(handle);
			ret=0;
			break;
		}

		// Get next entry, increment count
		count+=function_end_entry(handle,entry,file_ok);

		// Reset result code
		ret=1;
	}

	return ret;
}


// Given a source and destination "pattern" and an old name, build a new name
BOOL rename_get_wild(char *src_pattern,char *dst_pattern,char *old_name,char *new_name)
{
	char *src_prefix,*src_suffix;
	char *dst_prefix,*dst_suffix;
	char *keep_bit;
	int len,prefix_len=0,suffix_len=0;
	int a;

	if (!(src_prefix=AllocVec(540,0)))
		return 0;
	src_suffix=src_prefix+108;
	dst_prefix=src_suffix+108;
	dst_suffix=dst_prefix+108;
	keep_bit=dst_suffix+108;

	// Initialise source prefix and suffix
	src_prefix[0]=0;
	src_suffix[0]=0;

	// Get length of source pattern
	len=strlen(src_pattern);

	// Look for * in source pattern
	for (a=0;a<len;a++)
	{
		// Found a *
		if (src_pattern[a]=='*')
		{
			// Get prefix (bit before *)
			strcpy(src_prefix,src_pattern);
			prefix_len=a;
			src_prefix[prefix_len]=0;

			// Get suffix (bit after *)
			strcpy(src_suffix,src_pattern+a+1);
			suffix_len=len-a-1;
			break;
		}
	}

	// Look for * in destination pattern
	for (a=0;dst_pattern[a];a++)
	{
		// Found a *
		if (dst_pattern[a]=='*')
		{
			// Get prefix
			strcpy(dst_prefix,dst_pattern);
			dst_prefix[a]=0;

			// Get suffix
			strcpy(dst_suffix,dst_pattern+a+1);
			break;
		}
	}

	// Get old name length
	len=strlen(old_name);

	// If no prefix, or the old name matches the prefix
	if ((prefix_len==0 || (strnicmp(old_name,src_prefix,prefix_len))==0) &&

		// If no suffix, or old name matches the suffix
		(suffix_len==0 || (len>=suffix_len && (stricmp(&old_name[len-suffix_len],src_suffix))==0)))
	{
		UWORD size;

		// Get length of bit to keep
		len-=prefix_len+suffix_len;

		// Copy bit we are keeping
		copy_mem(old_name+prefix_len,keep_bit,len);
		keep_bit[len]=0;

		// Build new name
		size = stccpy(new_name, dst_prefix, 108);
		size--;
		size += stccpy(new_name + size, keep_bit, 108 - size);
		size--;
		strncat(new_name,dst_suffix, 108 - size);

		// New name is valid?
		if (new_name[0]!=0)
		{
			return 1;
		}
	}

	// Invalid (didn't match)
	return 0;
}
