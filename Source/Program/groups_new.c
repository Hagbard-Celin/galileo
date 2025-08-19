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
#include "lsprintf_protos.h"
#include "misc_protos.h"
#include "requesters.h"
#include "backdrop_protos.h"
#include "groups.h"
#include "icons.h"

void groups_new(BackdropInfo *info,IPCData *ipc)
{
	BackdropObject *object;
	char name[32],path[80];
	BPTR dir;
	short a;

	// Get group name
	name[0]=0;
	if (!(super_request_args(
		info->window->WScreen,
		GetString(&locale,MSG_GROUP_ENTER_NAME),
		SRF_SCREEN_PARENT|SRF_BUFFER|SRF_IPC,
		name,30,
		ipc,
		GetString(&locale,MSG_OKAY),
		GetString(&locale,MSG_CANCEL),0))) return;

	// Strip and / or :
	for (a=0;name[a];a++)
	{
		if (name[a]=='/' || name[a]==':')
		{
			name[a]=0;
			break;
		}
	}

	// Invalid name?
	if (!name[0]) return;

	// Build full path
	strcpy(path,"PROGDIR:groups");
	AddPart(path,name,80);

	// Create directory
	if (!(dir=CreateDir(path)))
	{
		DisplayBeep(info->window->WScreen);
		return;
	}

	// Unlock directory
	UnLock(dir);

	// Write icon
	if (!(icon_write(ICONTYPE_GROUP,path,0,0,0,0)))
	{
		// Failed
		DeleteFile(path);
		return;
	}

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// Create new entry
	if (object=backdrop_leftout_new(info,path,0,0))
	{
		// Change type to group
		object->bdo_type=BDO_GROUP;

		// Add object
		backdrop_new_group_object(info,object,BDNF_CD|BDNF_RECALC);
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Open group
	if (object) backdrop_open_group(info,object,1);
}


// Add a new object
BOOL backdrop_new_group_object(
	BackdropInfo *info,
	BackdropObject *object,
	unsigned short flags)
{
	BOOL show=1;

	// Get icon
	backdrop_get_icon(info,object,flags&(GETICON_CD|GETICON_KEEP));

	// If it's a disk, fail
	if (!object->bdo_icon || (flags&BDNF_NO_DISKS && object->bdo_icon->do_Type==WBDISK))
		return 0;

	// Arrange new icon if necessary
	if (object->bdo_flags&BDOF_NO_POSITION)
	{
		// Allowed to arrange?
		if (!(flags&BDNF_NO_ARRANGE))
		{
			// Position new icon
			backdrop_place_object(info,object);
		}

		// Don't show, will be arranged later
		else show=0;
	}

	// Show icon
	if (show) backdrop_render_object(info,object,BRENDERF_CLIP);

	// Recalc?
	if (flags&BDNF_RECALC) backdrop_calc_virtual(info);
	return 1;
}


// Dereference a group object
group_record *group_dereference(
	BPTR lock,
	char *name)
{
	BPTR old=0;
	BPTR file;
	ULONG data[2] = {0};
	group_record *record = 0;
	BOOL ok=0;

	// Change directory?
	if (lock) old = CurrentDir(lock);

	// Open file
	if (file = Open(name,MODE_OLDFILE))
	{
		// Read header
		Read(file,&data,sizeof(data));

		// Valid group object?
		if (data[0] == MAKE_ID('G','R','P','\1'))
		{
			ULONG len;

			data[1]	+= sizeof(group_record);

			if (record = AllocVec(data[1] + 1,MEMF_ANY))
			{
			    // Read data
			    len = Read(file, (char *)record, data[1]);

			    if (len == data[1])
			    {
				char *tmp = (char *)record;

				tmp += len;
				// Null-terminate
				*tmp = 0;
				//*(((char *)record) + len) = 0;

			        // Set ok flag
			        ok=1;
			    }
			}
		}
		// Upgrade old group object?
		else
		if (data[0] == MAKE_ID('G','R','P','\0'))
		{
		    char *buffer;
		    short len,pos;
		    APTR bfile;

		    Close(file);
		    file = 0;

		    // Allocate buffer
		    if (buffer = AllocVec(640 , MEMF_CLEAR))
		    {
			// Open file
			if (bfile = OpenBuf(name, MODE_OLDFILE, 320))
			{
			    // Skip header
			    SeekBuf(bfile, sizeof(ULONG), OFFSET_BEGINNING);

			    // Read data
			    len = ReadBuf(bfile, buffer, 300);

			    // Look for newline
			    for (pos=0;pos<len;pos++)
				    if (buffer[pos]=='\n') break;

			    // Got newline?
			    if (buffer[pos]=='\n')
			    {
				BPTR lock;
				char *fullname;
				ULONG length;

				// Null-terminate
				if (pos>255)
				{
				    buffer[255]=0;
				    length = 255;
				}
				else
				{
				    buffer[pos]=0;
				    length = pos;
				}

				if (lock = LockFromPath(buffer, length, LFPF_TRY_ICON))
				{
				    char *infotst;
				    fullname = buffer + 320;

				    NameFromLock(lock, fullname, 300);

				    infotst = fullname + strlen(fullname) - 5;

				    if (!(stricmp(infotst, ".info")))
					infotst[0] = 0;


				    if (record = AllocVec(sizeof(group_record) + strlen(fullname) + 1,MEMF_ANY))
				    {
				        struct FileLock *fl;
				        struct DosList *volume;

					// Get filelock pointer
					fl = (struct FileLock *)BADDR(lock);

				        // Get volume entry
					if (volume = (struct DosList *)BADDR(fl->fl_Volume))
				        {
					    // Get volume id
					    record->date = volume->dol_misc.dol_volume.dol_VolumeDate;
					    record->len = *(UBYTE *)BADDR(volume->dol_Name);

					    // Copy path
					    strcpy(record->name, fullname);

					    // Get position
					    if (len>pos+4)
						    CopyMem((char *)buffer+pos+1, (char *)&record->pos, sizeof(Point));

					    // Get flags
					    if (len>pos+8)
						    CopyMem((char *)buffer+pos+5, (char *)&record->flags, sizeof(ULONG));

					    // Set ok flag
					    ok=1;

					    // Close file
					    CloseBuf(bfile);

					    // Write upgraded file
					    group_write_data(name, record->name, NULL, record);
				        }

				    }

				    UnLock(lock);
				}
			    }
			}

			FreeVec(buffer);
		    }
		}

		// Close file
		if (file)
		    Close(file);
	}

	if (!ok)
	{
	    if (record)
		FreeVec(record);
	    record = 0;
	}

	// Restore directory
	if (lock) CurrentDir(old);
	return record;
}


// Write group data file
BOOL group_write_data(char *name,char *path,char *filename, group_record *record)
{
	APTR file;
	ULONG len, pathlen, namelen = 0;

	// Create file
	if (!(file=OpenBuf(name,MODE_NEWFILE,384)))
	{
		return 0;
	}

	pathlen = strlen(path);

	if (filename)
	    namelen = strlen(filename);

	len = pathlen + namelen;

	// Write id
	WriteBuf(file, "GRP\1", 4);

	// Write total string length
	WriteBuf(file, (char *)&len, sizeof(ULONG));

	// Write data
	WriteBuf(file, (char *)record, sizeof(group_record));

	// Write full path
	WriteBuf(file, path, pathlen);
	if (namelen)
	    WriteBuf(file, filename, namelen);

	// Close file
	CloseBuf(file);
	return 1;
}

// Snapshot position in group
void group_snapshot_icon(BackdropInfo *info,BackdropObject *icon,short x,short y)
{
	GroupData *group;
	char buffer[123];
	BPTR lock,old;
	group_record *record;

	// Get group pointer
	group=(GroupData *)IPCDATA(info->ipc);

	// Build group path
	lsprintf(buffer,"PROGDIR:groups/%s",group->name);

	// Change into group directory
	if (!(lock=Lock(buffer,ACCESS_READ))) return;
	old=CurrentDir(lock);

	// Dereference object
	if (record = group_dereference(0,icon->bdo_device_name))
	{
		record->pos.x = x;
		record->pos.y = y;
		// Write new data file
		group_write_data(icon->bdo_device_name, record->name, NULL, record);

		FreeVec(record);
	}

	// Restore directory
	UnLock(CurrentDir(old));
}
