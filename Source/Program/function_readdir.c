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
#include <gfm/paths_protos.h>
#include <gfm/filesystem_id.h>
#include "lister_protos.h"
#include "function_launch_protos.h"
#include "reselect_protos.h"
#include "function_protos.h"
#include "buffers_protos.h"
#include "status_text_protos.h"
#include "position_protos.h"
#include "filetypes_protos.h"
#include "envoy.h"
#include "lsprintf_protos.h"
#include <libraries/multiuser.h>
#include <proto/multiuser.h>

static short read_dir(DirBuffer *,Lister *,FunctionHandle *,BPTR,ReselectionData *);
static void readdir_check_format(Lister *lister, char *path, BPTR lock, ListFormat *, BOOL);


// Sets up and reads a new directory
// Called from FUNCTION PROCESS
void function_read_directory(
	FunctionHandle *handle,
	Lister *lister,
	char *source_path,
	BPTR source_lock)
{
	ReselectionData reselect;
	BPTR lock;
	short ret, volnamelen = 0;
	BOOL same=0,noread=0;
	struct DateStamp stamp,*stamp_ptr=0;
	struct InfoData __aligned info;
	ListFormat oldformat;
	char *ptr;
	DirBuffer *buffer;
	ULONG disktype=0;
	STRPTR path_full_name, path_full_device;

	// If we're currently displaying a special buffer, return to a normal one
	IPC_Command(
		lister->ipc,
		LISTER_CHECK_SPECIAL_BUFFER,
		0,0,0,
		REPLY_NO_PORT);

	if (!source_lock)
	{
	    if (!source_path || !source_path[0])
		return;

	    // Get last character of path, strip if it's a /
	    ptr=source_path+strlen(source_path)-1;
	    if (*ptr=='/') *ptr=0;

	    lock = LockFromPath(source_path, NULL, NULL);
	}
	else
	    lock = source_lock;

	// Lock path to read
	if (lock)
	{
		struct DosList *doslist;

		path_full_name = PathFromLock(NULL, lock, PFLF_END_SLASH, NULL);
		path_full_device = PathFromLock(NULL, lock, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL);

		// Get disk information
		Info(lock,&info);

		// Get dos list pointer and hence disk datestamp
		if (doslist=(struct DosList *)BADDR(info.id_VolumeNode))
		{
			// Get date stamp
			stamp=doslist->dol_misc.dol_volume.dol_VolumeDate;
			stamp_ptr=&stamp;

			volnamelen = *(UBYTE *)BADDR(doslist->dol_Name);

			// Get disk type
			disktype=doslist->dol_misc.dol_volume.dol_DiskType;
		}

		// No disktype?
		if (!disktype) disktype=info.id_DiskType;
	}

	// Couldn't lock
	else
	{
	    if (!(strchr(source_path,':')))
	    {
			path_full_name = JoinString(0, lister->cur_buffer->buf_Path, source_path, NULL, JSF_E_SLASH);
			path_full_device = CopyString(0, path_full_name);
	    }
	    else
	    {
			path_full_name = CopyString(0, source_path);
			path_full_device = CopyString(0, path_full_name);
	    }
	}

	// Store current list format
	oldformat=lister->cur_buffer->buf_ListFormat;

	// Search for this path in another buffer?
	if (handle->flags&GETDIRF_CANCHECKBUFS)
	{
		DirBuffer *buffer;
/*
		char *ptr;

		// Get volume name
		if (ptr=strchr(PATH_FULL_NAME,':'))
		{
			char *volume;

			// Clear colon temporarily
			*ptr=0;

			// Allocate memory for volume name
			if (volume=AllocVec(strlen(PATH_FULL_NAME)+1,MEMF_CLEAR))
			{
				// Copy the volume name
				strcpy(volume,PATH_FULL_NAME);
			}

			// Restore the colon
			*ptr=':';

			// Save pointer to name
			ptr=volume;
		}
*/

		if (source_lock)
		{
		    buffer=(DirBuffer *)IPC_Command(
			lister->ipc,
			LISTER_BUFFER_FIND_LOCK,
			(ULONG)stamp_ptr,
			(APTR)lock,
			0,
			REPLY_NO_PORT);
		}
		// Look for path
		else
		{
		    buffer=(DirBuffer *)IPC_Command(
			lister->ipc,
			LISTER_BUFFER_FIND,
			(ULONG)stamp_ptr,
			(APTR)path_full_device,
			0,
			REPLY_NO_PORT);
		}

		if (buffer)
		{
			// Found it
			if (lock != buffer->buf_Lock)
			    UnLock(lock);

			// Check for special sort format
			readdir_check_format(lister, path_full_name, buffer->buf_Lock, &oldformat, same);

			// Store format
			lister->format=oldformat;

			// Reset some flags in the buffer
			buffer->more_flags&=~DWF_LOCK_STATE;

			// Show buffer in lister
			IPC_Command(lister->ipc,LISTER_SHOW_BUFFER,TRUE,buffer,0,REPLY_NO_PORT);
			noread=1;
		}
	}

	// Still reading?
	if (!noread)
	{
		ULONG flags = FREEDIRF_FREE_LOCK;

		// Empty buffer?
		if (handle->flags&GETDIRF_CANMOVEEMPTY)
		{
		    if (source_lock)
		    {
			IPC_Command(
				lister->ipc,
				LISTER_BUFFER_FIND_EMPTY_LOCK,
				(ULONG)stamp_ptr,
				(APTR)lock,
				0,
				REPLY_NO_PORT);
		    }
		    else
		    {
			IPC_Command(
				lister->ipc,
				LISTER_BUFFER_FIND_EMPTY,
				(ULONG)stamp_ptr,
				(APTR)path_full_device,
				0,
				REPLY_NO_PORT);
		    }
		}

		// Get buffer
		buffer=lister->cur_buffer;


		if (source_lock)
		{
		    // Same lock?
		    if (source_lock == buffer->buf_Lock)
		    {
				flags &= ~(FREEDIRF_FREE_LOCK);
				same=1;
		    }
		    else
		    if (SameLock(source_lock,buffer->buf_Lock) == LOCK_SAME)
		    {
		        UnLock(source_lock);
		        lock = buffer->buf_Lock;
			flags &= ~(FREEDIRF_FREE_LOCK);
			same=1;
		    }
		}
		// Same path?
		else
		if (buffer->buf_Path && (source_path == buffer->buf_Path || stricmp(buffer->buf_Path,path_full_device)==0))
		{
			//flags &= ~(FREEDIRF_FREE_PATH|FREEDIRF_FREE_EXPANDEDPATH);
			same=1;
		}

		if (!same)
		{
		    Forbid();
		    // Store paths
		    if (path_full_device)
		    {
			STRPTR oldpath = buffer->buf_Path;
			buffer->buf_Path = path_full_device;
			if (oldpath)
			    FreeMemH(oldpath);
		    }

		    if (path_full_name)
		    {
			STRPTR oldpath = buffer->buf_ExpandedPath;
			buffer->buf_ExpandedPath = path_full_name;
			if (oldpath)
			    FreeMemH(oldpath);
		    }
		    Permit();
		}

		// Refresh path field
		IPC_Command(lister->ipc,LISTER_REFRESH_PATH,0,0,0,0);

		// Display text
		status_text(lister,GetString(&locale,MSG_READING_DIRECTORY));

		// Lock buffer
		buffer_lock(buffer,TRUE);

		// Need to do reslection?
		if (handle->flags&GETDIRF_RESELECT)
		{
			InitReselect(&reselect);
			IPC_Command(
				lister->ipc,
				LISTER_MAKE_RESELECT,
				RESELF_SAVE_FILETYPES,
				(APTR)&reselect,
				0,
				REPLY_NO_PORT);
		}

		// Free buffer
		buffer_freedir(buffer,flags); /******* was 1 ******/

		// Default to previous format
		lister->cur_buffer->buf_ListFormat=oldformat;

		// Store lock
		if (lock && flags&FREEDIRF_FREE_LOCK)
		    buffer->buf_Lock = lock;

		// Unlock buffer
		buffer_unlock(buffer);

		// Refresh window display
		IPC_Command(
			lister->ipc,
			LISTER_REFRESH_WINDOW,
			REFRESHF_UPDATE_NAME|REFRESHF_STATUS|REFRESHF_SLIDERS|REFRESHF_CLEAR_ICONS|REFRESHF_NO_DIRLIST,
			0,
			0,
			REPLY_NO_PORT);
			// Clear file area
			EraseRect(&lister->list_area.rast,
				lister->list_area.rect.MinX,
				lister->list_area.rect.MinY,
				lister->list_area.rect.MaxX,
				lister->list_area.rect.MaxY);

		// Invalid directory?
		if (!lock)
		{
			// Display error message
			function_error_text(handle,-1);
			ret=0;
		}

		// Otherwise
		else
		{
			// Lock buffer
			buffer_lock(buffer,TRUE);

			// Check format
			readdir_check_format(lister, path_full_name, lock, &oldformat, same);

			// Store format
			buffer->buf_ListFormat=oldformat;

			/**************/
			lister->format=oldformat;
			/**************/

			// Save disk type
			buffer->buf_DiskType=disktype;

			buffer->buf_VolNameLength = volnamelen;

			// Get volume name
			stccpy(buffer->buf_VolumeLabel,path_full_name,32);
			if (ptr=strchr(buffer->buf_VolumeLabel,':')) *ptr=0;

			// Unlock buffer
			buffer_unlock(buffer);

			// Read directory
			ret=read_dir(
				buffer,
				lister,
				handle,
				lock,
				/*&info,*/
				(handle->flags&GETDIRF_RESELECT)?&reselect:0);
		}

		// Unlock directory
		//UnLock(lock);

		// Store disk stamp
		if (stamp_ptr) buffer->buf_VolumeDate=*stamp_ptr;

		// Do reselection
		if (handle->flags&GETDIRF_RESELECT)
		{
			if (ret)
			{
				IPC_Command(
					lister->ipc,
					LISTER_DO_RESELECT,
					0,
					(APTR)&reselect,
					0,
					REPLY_NO_PORT);
			}
			FreeReselect(&reselect);
		}

		// Add to history list
		lister_add_history(lister);
	}

	// Refresh window display if read was successful
	if (lock)
	{
		short mode;

		// Refresh
		IPC_Command(
			lister->ipc,
			LISTER_REFRESH_WINDOW,
			REFRESHF_UPDATE_NAME|REFRESHF_STATUS|REFRESHF_SLIDERS|REFRESHF_CD,
			0,
			0,0);

		// Check space gauge
		IPC_Command(lister->ipc,LISTER_SET_GAUGE,0,0,0,0);

		// Get icons if necessary
		if (lister->flags&(LISTERF_VIEW_ICONS|LISTERF_ICON_ACTION))
		{
			lister_get_icons(handle,lister,0,GETICONSF_SHOW);
		}
		// Initial mode
		mode=(lister->flags&LISTERF_VIEW_ICONS)?0:LISTERMODE_ICON;

		// Get position entry
		if (!(GetListerPosition(path_full_name, stamp_ptr, 0, 0, &lister->other_dims, &mode, 0, 0, 0, GLPF_USE_MODE)))
		{
			// No entry; use same dimensions as current
			if (lister_valid_window(lister)) lister->other_dims=*((struct IBox *)&lister->window->LeftEdge);
		}
	}

	if (noread)
	{
	    if (path_full_device)
		FreeMemH(path_full_device);
	    if (path_full_name)
		FreeMemH(path_full_name);
	}

	if (same)
	{
	    if (path_full_device)
		FreeMemH(path_full_device);
	    if (path_full_name)
		FreeMemH(path_full_name);
	}
}


// Read a directory into a buffer
// Called from FUNCTION PROCESS
static short read_dir(
	DirBuffer *buffer,
	Lister *lister,
	FunctionHandle *handle,
	BPTR lock,
	ReselectionData *reselect)
{
	long file_count=0,dir_count=0;
	struct FileInfoBlock *fileinfo;
	BPTR parent;
	DirEntry *entry;
	struct MinList file_list;
	NetworkInfo network,*network_ptr=0;
	short sniff=SNIFFF_NO_FILETYPES;
	short item;

	// Go through display items
	for (item=0;item<DISPLAY_LAST;item++)
	{
		// Do we want to show network stuff?
		if (buffer->buf_ListFormat.display_pos[item]==DISPLAY_OWNER ||
			buffer->buf_ListFormat.display_pos[item]==DISPLAY_GROUP ||
			buffer->buf_ListFormat.display_pos[item]==DISPLAY_NETPROT)
		{
			// Yes
			network_ptr=&network;
		}

		// Or show filetypes?
		else
		if (buffer->buf_ListFormat.display_pos[item]==DISPLAY_FILETYPE)
			sniff&=~SNIFFF_NO_FILETYPES;

		// Or show versions?
		else
		if (buffer->buf_ListFormat.display_pos[item]==DISPLAY_VERSION)
			sniff|=SNIFFF_VERSION;
	}

	// Allocate fib
	if (!(fileinfo=AllocDosObject(DOS_FIB,0)))
		return 0;

	// Examine this object
	Examine(lock,fileinfo);

	// If object is a file, fail
	if (fileinfo->fib_DirEntryType<0)
	{
		function_error_text(handle,ERROR_OBJECT_WRONG_TYPE);
		FreeDosObject(DOS_FIB,fileinfo);
		return 0;
	}

	// Store directory datestamp
	buffer->buf_DirectoryDate=fileinfo->fib_Date;

	// Set reading flag
	buffer->flags|=DWF_READING;

	// Is this a root?
	if (!(parent=ParentDir(lock))) buffer->flags|=DWF_ROOT;
	else UnLock(parent);

	// Save object name
	if (fileinfo->fib_FileName[0])
		strcpy(buffer->buf_ObjectName,fileinfo->fib_FileName);
	else
	{
		if (buffer->flags&DWF_ROOT)
		{
			char *ptr;

			stccpy(buffer->buf_ObjectName,buffer->buf_ExpandedPath,GUI->def_filename_length-1);
			if (ptr=strchr(buffer->buf_ObjectName,':')) *ptr=0;
		}
		else stccpy(buffer->buf_ObjectName,FilePart(buffer->buf_ExpandedPath),GUI->def_filename_length-1);
	}

	// Initialise list
	NewList((struct List *)&file_list);

	// Loop until directory is empty
	while (ExNext(lock,fileinfo))
	{
		BOOL ok=1;

		// Check abort
		if (function_check_abort(handle))
		{
			buffer->flags|=DWF_ABORTED;
			break;
		}

		// Ok to add?
		if (ok)
		{
			// Get network info if needed
			if (network_ptr)
			{
				network_get_info(
					network_ptr,
					buffer,
					buffer->buf_Lock,
					fileinfo->fib_OwnerUID,
					fileinfo->fib_OwnerGID,
					fileinfo->fib_Protection);
			}

			// Directories are meant to have no size, apparently
			if (fileinfo->fib_DirEntryType>0)
				fileinfo->fib_Size=0;

			// Create entry
			if (entry=
				create_file_entry(
					buffer,
					lock,
					fileinfo->fib_FileName,
					fileinfo->fib_Size,
					fileinfo->fib_DirEntryType,
					&fileinfo->fib_Date,
					fileinfo->fib_Comment,
					fileinfo->fib_Protection,
					0,0,0,
					network_ptr))
			{
				// Add to list
				AddTail((struct List *)&file_list,(struct Node *)entry);

				// Increment counts
				if (ENTRYTYPE(entry->de_Node.dn_Type)==ENTRY_DIRECTORY)
					++dir_count;
				else
					++file_count;
			}

			// Failed
			else
			{
				function_error_text(handle,-1);
				break;
			}
		}
	}

	// Reselection list?
	if (reselect && reselect->flags&RESELF_SAVE_FILETYPES)
		GetReselectFiletypes(reselect,&file_list);

	// Lock buffer
	buffer_lock(buffer,TRUE);

	// Sort directory
	buffer_sort_list(buffer,&file_list,file_count,dir_count);

	// Unlock buffer
	buffer_unlock(buffer);

	// Get things?
	if (sniff!=SNIFFF_NO_FILETYPES) filetype_find_typelist(lister,sniff);

	// Clear reading flag, set valid flag
	buffer->flags&=~DWF_READING;
	buffer->flags|=DWF_VALID;

	// Clear fake-dir flag
	lister->more_flags&=~LISTERF_FAKEDIR;

	// Free up
	FreeDosObject(DOS_FIB,fileinfo);
	return 1;
}


// Get network information for a file
void network_get_info(
	NetworkInfo *network,
	DirBuffer *buffer,
	BPTR lock,
	UWORD owner,
	UWORD group,
	ULONG protection)
{
	char *ptr;
	struct MsgPort *port;
	short a,b;

	// Get device process message port
	port = ((struct FileLock *)BADDR(buffer->buf_Lock))->fl_Task;

	// Initialise network info
	network->owner_id=owner;
	network->group_id=group;
	network->owner[0]=0;
	network->group[0]=0;

	// Valid owner?
	if (owner)
	{
		// If owner ID matches the last one, copy that name
		if (buffer->last_owner==owner)
			stccpy(network->owner,buffer->owner_name,sizeof(network->owner));

		// Try sending packet
		else
		if (buffer->user_info && (DoPkt(port,ACTION_UID_TO_USERINFO,owner,(ULONG)buffer->user_info,0,0,0)))
		{
			// Store owner for next time
			buffer->last_owner=owner;
			strcpy(buffer->owner_name,buffer->user_info->ui_UserName);

			// Copy name
			stccpy(network->owner,buffer->user_info->ui_UserName,sizeof(network->owner));
		}

		// MUFS maybe?
		else
		if (buffer->mu_user_info && buffer->buf_DiskType!=ID_ENVOY_DISK)
		{
			// Owner by someone?
			if (owner!=muNOBODY_UID)
			{
				// Try for info
				buffer->mu_user_info->uid=owner;
				if (muGetUserInfo(buffer->mu_user_info,muKeyType_uid))
				{
					// Store owner for next time
					buffer->last_owner=owner;
					strcpy(buffer->owner_name,buffer->mu_user_info->UserID);

					// Copy name
					stccpy(network->owner,buffer->mu_user_info->UserID,sizeof(network->owner));
				}
			}
			else owner=0;
		}

		// No owner yet?
		if (!*network->owner)
		{
			// Build number string
			lsprintf(network->owner,"%ld",owner);
		}
	}

	// Valid group?
	if (group)
	{
		// If group ID matches the last one, copy that name
		if (buffer->last_group==group) stccpy(network->group,buffer->group_name,sizeof(network->group));

		// Try sending packet
		else
		if (buffer->group_info && (DoPkt(port,ACTION_GID_TO_GROUPINFO,group,(ULONG)buffer->group_info,0,0,0)))
		{
			// Store owner for next time
			buffer->last_group=group;
			strcpy(buffer->group_name,buffer->group_info->gi_GroupName);

			// Copy name
			stccpy(network->group,buffer->group_info->gi_GroupName,sizeof(network->group));
		}

		// MUFS maybe?
		else
		if (buffer->mu_group_info && buffer->buf_DiskType!=ID_ENVOY_DISK)
		{
			// Try for info
			buffer->mu_group_info->gid=group;
			if (muGetGroupInfo(buffer->mu_group_info,muKeyType_gid))
			{
				// Store group for next time
				buffer->last_group=group;
				strcpy(buffer->group_name,buffer->mu_group_info->GroupID);

				// Copy name
				stccpy(network->group,buffer->mu_group_info->GroupID,sizeof(network->group));
			}
		}

		// No group yet?
		if (!*network->group)
		{
			// Build number string
			lsprintf(network->group,"%ld",group);
		}
	}

	// Get pointer to protection bits buffer
	ptr=network->net_protbuf;

	// Build group protection bits
	for (a=FIBB_GRP_READ,b=0;a>=FIBB_GRP_DELETE;a--,b++)
	{
		if (!(protection&(1<<a))) *ptr++='-';
		else *ptr++=((char *)"rwed")[b];
	}

	// Build other protection bits
	for (a=FIBB_OTR_READ,b=0;a>=FIBB_OTR_DELETE;a--,b++)
	{
		if (!(protection&(1<<a))) *ptr++='-';
		else *ptr++=((char *)"rwed")[b];
	}

	// Null-terminate
	*ptr=0;
}


// Check sort format
static void readdir_check_format(Lister *lister, char *path, BPTR lock, ListFormat *format, BOOL same)
{
	position_rec *pos;
	struct DateStamp date = {0};

	VolIdFromLock(lock, &date, NULL);

	// Is format locked, or this is a rescan?
	if (lister->more_flags&LISTERF_LOCK_FORMAT || same)
	{
		// Use current format	
		*format=lister->format;
	}

	// See if position is supplied
	else
	if (!(pos=GetListerPosition(path, &date, 0, 0, 0, 0, format, 0, 0, 0)) ||
		!(pos->flags&POSITIONF_FORMAT))
	{
		// Use current format from lister
		*format=lister->format;

		// Don't inherit format?
		if (!(lister->format.flags&LFORMATF_INHERIT))
		{
			// Get space gauge setting from default format
			if (environment->env->list_format.flags&LFORMATF_GAUGE)
				format->flags|=LFORMATF_GAUGE;
			else
				format->flags&=~LFORMATF_GAUGE;
		}
	}

	// Is the lister in icon mode?
	if (lister->flags&LISTERF_VIEW_ICONS)
	{
		// Filter icons on?
		if (format->flags&LFORMATF_REJECT_ICONS)
		{
			// Turn it off temporarily
			format->flags&=~LFORMATF_REJECT_ICONS;
			format->flags|=LFORMATF_TEMP_ICONS;
		}
	}
}
