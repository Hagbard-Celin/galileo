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

// Get the parent of a path
STRPTR path_parent_alloch(APTR memory, STRPTR path)
{
	ULONG len;
	STRPTR parent;

	// Get length of path
	if ((len=strlen(path)-1)<0) return 0;

	// If last character is a :, can't go any further
	if (path[len]==':') return 0;

	// If last character is a /, start at one before
	if (path[len]=='/') --len;

	// Go back to previous / or :
	while (len>0 && path[len]!='/' && path[len]!=':') --len;

	// Got a separator?
	if (path[len]=='/' || path[len]==':')
	{
	    char tmp;

	    tmp = path[len+1];
	    // Clear after this
	    path[len+1]=0;
	    parent = CopyString(memory, path);
	    path[len+1] = tmp;
	    return parent;
	}

	// Nothing
	return 0;
}


// Get the parent of a path
STRPTR path_root_alloch(APTR memory, STRPTR path)
{
	ULONG len;
	STRPTR root, pos;
	char tmp;

	// Get length of path
	if ((len=strlen(path)-1)<0) return 0;

	// If last character is a :, can't go any further
	if (path[len]==':') return 0;

	if (!(pos = strchr(path,':')))
	    return 0;

	pos++;

	tmp = *pos;
	root = CopyString(memory, path);

	*pos = tmp;

	return root;
}


// Get the parent of a path
path_parent(char *path)
{
	short len;

	// Get length of path
	if ((len=strlen(path)-1)<0) return 0;

	// If last character is a :, can't go any further
	if (path[len]==':') return 0;

	// If last character is a /, start at one before
	if (path[len]=='/') --len;

	// Go back to previous / or :
	while (len>0 && path[len]!='/' && path[len]!=':') --len;

	// Got a separator?
	if (path[len]=='/' || path[len]==':')
	{
		// Clear after this
		path[len+1]=0;
		return 1;
	}

	// Nothing
	return 0;
}


// Get the root of a path
path_root(char *path)
{
	short count;

	// Keep doing parent until we reach the root
	for (count=0;path_parent(path);count++);

	return count;
}



// Get the root of a pathname
get_path_root(char *path,char *root_name,struct DateStamp *date)
{
	struct DevProc *proc;
	struct InfoData __aligned info;
	struct DosList *dos;

	// Get device process
	if (!(proc=GetDeviceProc(path,0)))
		return 0;

	// Send info packet
	DoPkt(proc->dvp_Port,ACTION_DISK_INFO,MKBADDR(&info),0,0,0,0);

	// Get DOS list pointer
	if (!(dos=(struct DosList *)BADDR(info.id_VolumeNode)))
	{
		FreeDeviceProc(proc);
		return 0;
	}

	// Get root device name
	if (root_name)
	{
		if (dos->dol_Name) BtoCStr(dos->dol_Name,root_name,32);
		else *root_name=0;
	}

	// Copy datestamp if buffer supplied
	if (date) *date=dos->dol_misc.dol_volume.dol_VolumeDate;

	// Free device process
	FreeDeviceProc(proc);

	// Return disk state
	return info.id_DiskState;
}
