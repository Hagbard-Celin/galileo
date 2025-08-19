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
#include "function_protos.h"
#include "buffers.h"
#include "lsprintf_protos.h"

typedef struct
{
	char pattern[40];
	short comment;
} HuntData;

// HUNT
GALILEOFM_FUNC(function_hunt)
{
	FunctionEntry *entry;
	long count=1,ret=1;
	HuntData *data;

	// Get data pointer
	data=(HuntData *)handle->inst_data;

	// Need to ask for pattern?
	if (!data->pattern[0])
	{
		// Get existing pattern
		GetSemaphore(&GUI->findfile_lock,SEMF_SHARED,0);
		strcpy(data->pattern,GUI->findfile_string);
		FreeSemaphore(&GUI->findfile_lock);

		// Ask for hunt pattern
		if (!(function_request(
			handle,
			GetString(&locale,MSG_HUNT_ENTER_FILENAME),
			SRF_BUFFER|SRF_PATH_FILTER|SRF_CHECKMARK,
			data->pattern,40,
			GetString(&locale,MSG_SEARCH_COMMENTS),&data->comment,
			GetString(&locale,MSG_OKAY),
			GetString(&locale,MSG_CANCEL),0)))
		{
			function_abort(handle);
			return 0;
		}

		// Store pattern
		GetSemaphore(&GUI->findfile_lock,SEMF_EXCLUSIVE,0);
		strcpy(GUI->findfile_string,data->pattern);
		FreeSemaphore(&GUI->findfile_lock);

		// Parse pattern
		ParsePatternNoCase(data->pattern,handle->inst_data+64,128);
	}

	// Allow recursive directories
	handle->instruction_flags=INSTF_RECURSE_DIRS|INSTF_WANT_DIRS;

	// Display progress requester
	function_progress_on(
		handle,
		GetString(&locale,MSG_SCANNING_DIRECTORIES),
		handle->entry_count,
		PWF_FILENAME|PWF_GRAPH);

	// Go through files
	while (entry=function_get_entry(handle))
	{
		// Top-level directory?
		if (entry->fe_flags&FUNCENTF_TOP_LEVEL &&
			entry->fe_type>0)
		{
			// Update progress indicator
			if (function_progress_update(handle,entry,count++))
			{
				function_abort(handle);
				ret=0;
				break;
			}
		}

		// Otherwise
		else
		{
			// Check abort
			if (function_check_abort(handle))
			{
				ret=0;
				break;
			}

			// File?
			if (entry->fe_type<0)
			{
				BOOL match=0;
				BPTR org_dir = 0;

			        // CD to source-file directory
				if (entry->fe_lock)
				    org_dir = CurrentDir(entry->fe_lock);

				// Match comments?
				if (data->comment)
				{
					struct FileInfoBlock __aligned fib;

					// Get file information
					if (GetFileInfo(entry->fe_name,&fib))
					{
						// Match comment
						match=MatchPatternNoCase(handle->inst_data+64,fib.fib_Comment);
					}
				}

				// Match filename
				else
				{
					// Match name
					match=MatchPatternNoCase(handle->inst_data+64,entry->fe_name);
				}

				// Does file match?
				if (match)
				{
					char name[35], pathbuf[32];
					BPTR foundin;
					char *fullpath;

					if (foundin = DupLock(entry->fe_lock))
					{
					    if (!(fullpath = PathFromLock(NULL , foundin, NULL, NULL)))
					    {
					        UnLock(foundin);

				                // Restore current directory
				                if (org_dir)
					            CurrentDir(org_dir);

					        function_abort(handle);
					        ret=0;
					        break;
					    }
					    //UnLock(foundin);
					    get_trunc_filename(entry->fe_name,name);
					    get_trunc_path(fullpath,pathbuf);

					    // Build requester text
					    lsprintf(handle->func_work_buf,
						    GetString(&locale,MSG_HUNT_FOUND_FILE),
						    name,
						    pathbuf);

					    FreeMemH(fullpath);

					    // Display requester
					    if (!(ret=function_request(handle,
						                       handle->func_work_buf,
						                       0,
						                       GetString(&locale,MSG_YES),
						                       GetString(&locale,MSG_NEW_LISTER),
						                       GetString(&locale,MSG_NO),
						                       GetString(&locale,MSG_ABORT),0)))
					    {

					        // Restore current directory
					        if (org_dir)
						    CurrentDir(org_dir);

						UnLock(foundin);

						function_abort(handle);
						ret=0;
						break;
					    }

					    // Go to that directory?
					    if (ret==1)
					    {
						PathNode *path;

						// Get current lister
						if ((path=function_path_current(&handle->func_source_paths)) &&
							path->pn_lister)
						{
							// Read directory
							handle->flags=GETDIRF_CANCHECKBUFS|GETDIRF_CANMOVEEMPTY;
							function_read_directory(handle, path->pn_lister, NULL, foundin);

							// Do wildcard selection
							function_select_file(handle,path->pn_lister,(data->comment)?(char *)FilePart(entry->fe_name):data->pattern);

							// Break out
							ret=0;

						        // Restore current directory
						        if (org_dir)
							    CurrentDir(org_dir);

							break;
						}
					    }

					    // New lister?
					    else
					    if (ret==2)
					    {
						Lister *lister;
					        struct DateStamp date = {0};

					        VolIdFromLock(foundin, &date, NULL);

						// Open new lister
						if (lister=
							lister_open_new(
								0,
								&date,
								foundin,
								0,
								0,
								function_lister_current(&handle->func_source_paths)))
						{
							// Initialise lister
							IPC_Command(
								lister->ipc,
								LISTER_INIT,
								0,
								GUI->screen_pointer,
								0,
								0);

							// Do wildcard selection
							function_select_file(handle,lister,(data->comment)?(char *)entry->fe_name:data->pattern);
						}
					    }
					}
				}

			        // Restore current directory
			        if (org_dir)
				    CurrentDir(org_dir);
			}
		}

		// Get next entry
		function_end_entry(handle,entry,1);
	}

	return ret;
}
