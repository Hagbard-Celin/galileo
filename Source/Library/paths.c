/*

Galileo Amiga File-Manager and Workbench Replacement
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

#include "galileofmlib.h"
#include "paths.h"

// How to do a dirty hack in the least amount of code
// The global optimizer refused to let my putreg() be, I refused to abandon the
// idea of pushing the string length in d1, so this happened....
void __asm set_d1(register __d1 ULONG value);


UBYTE __saveds __asm L_VolIdFromLock(register __a0 BPTR lock,
				     register __a1 struct DateStamp *date,
				     register __a2 char *name)
{
    struct FileLock *fl;
    struct DosList *volume;
    UBYTE len;

    // Clear buffer
    if (!lock) return 0;

    // Get filelock pointer
    if (!(fl=(struct FileLock *)BADDR(lock))) return 0;

    if (!(volume = (struct DosList *)BADDR(fl->fl_Volume))) return 0;

    if (date)
	*date = volume->dol_misc.dol_volume.dol_VolumeDate;

    len = *(UBYTE *)BADDR(volume->dol_Name);

    if (name)
	strcpy(name, ((char *)BADDR(volume->dol_Name)) + 1);

    return len;
}


BPTR __saveds __asm L_LockFromVolIdPath(register __a0 STRPTR volumename,
					register __a1 CONST_STRPTR path,
					register __a2 struct DateStamp *volumedate,
				        register __d0 UBYTE namelen,
					register __d1 ULONG flags)
{
    static const char __aligned nullbstr[] = "\0";
    BPTR lock = 0;
    struct DosList *doslist;
    BOOL found = FALSE;
    TEXT name[32];
    STRPTR usename;

    if (!namelen && path && path[0])
    {
	do
	{
	    namelen++;
	} while (namelen < 32 && path[namelen] != ':' &&  path[namelen]);

	if (path[namelen] != ':')
	    return 0;
    }

    if (!namelen)
	    return 0;

    if (!volumename)
    {
	if (!path || !path[0])
	    return 0;
	stccpy(name, path, namelen);
	usename = name;
    }
    else usename = volumename;

    doslist = LockDosList(LDF_VOLUMES|LDF_READ);

    // Go through DOS list
    while (doslist=NextDosEntry(doslist,LDF_VOLUMES))
    {
	char *ptr;

	ptr = BADDR(doslist->dol_Name);
	if (*ptr++ == namelen &&
	    !memcmp(&doslist->dol_misc.dol_volume.dol_VolumeDate, volumedate, sizeof(struct DateStamp)) &&
	    !memcmp(ptr, usename, namelen))
	{
	    found = TRUE;
	    break;
	}
    }

    if (found && doslist->dol_Task)
    {
	lock = DoPkt(doslist->dol_Task,ACTION_LOCATE_OBJECT,0,MKBADDR(&nullbstr),SHARED_LOCK,0,0);
#ifdef RESOURCE_TRACKING
	if (lock)
	    RT_InjectLock(lock, __FILE__, __LINE__);
#endif
    }

    UnLockDosList(LDF_VOLUMES|LDF_READ);

    if (lock && path && path[0])
    {
	BPTR org_dir, dir_lock;

	org_dir = CurrentDir(lock);
	dir_lock = L_LockFromPath(path + namelen + 1, NULL, flags);
	UnLock(CurrentDir(org_dir));
	lock = dir_lock;
    }
    return lock;
}

BPTR __saveds __asm L_LockFromPathQuick(register __a0 CONST_STRPTR path,
					register __d0 ULONG length)
{
    // Get length if not supplied
    if (!path || (!length && (!(length = strlen(path)))))
    {
	return 0;
    }

    if (length < 256)
	return Lock(path,ACCESS_READ);
    else
    {
	STRPTR tmp;
	BPTR orgdir = 0;
	UBYTE ok = 1;

	if (tmp = AllocVec(256,MEMF_ANY))
	{
	    UBYTE pos = 254;
	    BPTR dir;

	    // Copy first 255 chars
	    stccpy(tmp,path,256);

	    // Find longest valid path
	    while (tmp[pos] != '/')
		pos--;

	    // Terminate the string
	    tmp[pos] = 0;

	    // Try lock
	    if (dir = Lock(tmp,ACCESS_READ))
	    {
		ULONG offset = 0;

		// CD and save orgiginal
		orgdir = CurrentDir(dir);

		// Repeat until path exhausted
		while (++pos + offset < length)
		{
		    offset = pos + offset;
		    pos = 254;

		    // Copy next 255 char segment
		    stccpy(tmp,path+offset,256);

		    // Find longest valid path
		    while (tmp[pos] != '/')
			pos--;

		    // Terminate the string
		    tmp[pos] = 0;

		    // Bail out on error
		    if (!(dir = Lock(tmp,ACCESS_READ)))
		    {
			ok = 0;
			UnLock(CurrentDir(orgdir));
			break;
		    }

		    // Change dir and unlock previous
		    UnLock(CurrentDir(dir));
		}
	    }
	    else
		ok = 0;

	    FreeVec(tmp);


	}

	// CurrentDir should now be what we where looking for
	if (ok)
	    return CurrentDir(orgdir);
	else
	{
	    return 0;
	}
    }
}


BPTR __saveds __asm L_LockFromPath(register __a0 CONST_STRPTR path,
				   register __d0 ULONG length,
				   register __d1 ULONG flags)
{
    BPTR orgdir = 0, dir;
    ULONG pos = 0;
    UBYTE part_pos = 0;
    TEXT part[108];

    if (!path)
    {
	return 0;
    }

    // Get length if not supplied
    if (!length && (!(length = strlen(path))))
	return Lock(path, ACCESS_READ);

    do
    {
	part[part_pos] = path[pos];

	// Check for ':', '/' or end of string
	if (part[part_pos] == ':' || part[part_pos] == '/' || part[part_pos] == 0)
	{
	    LONG err;
	    char *part_end, part_len, last_part = 0;

	    if (!part_pos && !part[part_pos])
		break;

	    part_end = part + part_pos;
	    part_len = part_pos;

	    // A ':' or a slingle '/', terminate string
	    if (!part_pos || part[part_pos] == ':')
		part_end++;

	    // Terminate string stripping any traling '/'
	    *part_end = 0;

	    if (pos == length && part_len <= 102)
		last_part = 1;

	    if (last_part && flags&LFPF_TRY_ICON_FIRST)
	    {
		strcpy(part_end, ".info");
		if (!(dir = Lock(part,ACCESS_READ)))
		    *part_end = 0;
	    }

	    // Try lock
	    else
	    if (!(dir = Lock(part,ACCESS_READ)))
	    {
		err = IoErr();

		if (last_part && flags&LFPF_TRY_ICON)
		{
		    strcpy(part_end, ".info");
		    dir = Lock(part,ACCESS_READ);
		}

		if (!dir)
		{
		    if (orgdir)
			UnLock(CurrentDir(orgdir));

		    SetIoErr(err);
		    return 0;
		}
	    }

	    if (last_part && !dir && flags&LFPF_TRY_ICON)
	    {
		if (!(dir = Lock(part,ACCESS_READ)))
		{
		    err = IoErr();
		    if (orgdir)
			UnLock(CurrentDir(orgdir));

		    SetIoErr(err);
		    return 0;
		}

	    }


	    // Change to this dir
	    if (!orgdir)
	        orgdir = CurrentDir(dir);
	    else
	        UnLock(CurrentDir(dir));
	    //ok = 1;
	    part_pos = 0;
	}
	else
	{
	    part_pos++;

	    if (part_pos >= 108)
	    {
		if (orgdir)
		    UnLock(CurrentDir(orgdir));

		SetIoErr(ERROR_INVALID_COMPONENT_NAME);
		return 0;
	    }
	}
	pos++;
    } while (pos <= length);

    if (!orgdir)
    {
	SetIoErr(ERROR_INVALID_COMPONENT_NAME);
	return 0;
    }
    // CurrentDir should now be what we where looking for
    else
	return CurrentDir(orgdir);
}


STRPTR __saveds __asm L_PathFromLock(register __a0 APTR memory, register __d0 BPTR lock, register __d1 ULONG flags, register __a1 CONST_STRPTR append)
{
    struct PathParts *parts;

    // Allocate first part of temporary buffer
    if (parts = AllocMem(sizeof(struct PathParts),MEMF_ANY))
    {
	STRPTR partname;
	struct PathParts *tmp_parts;
	ULONG path_len;
	UBYTE failed = FALSE, trunc = 0, this_part = 0;

	{
	    D_S(struct FileInfoBlock,tmp_fib);
	    BPTR parent_lock;
	    UWORD  parts_used;

	    // Init
	    parts->truncated = 0;
	    parts->next_parts = 0;
	    partname = parts->names;

	    // Get name of locked object
	    Examine(lock,tmp_fib);
	    path_len = stccpy(partname, tmp_fib->fib_FileName, 108);

	    // Is the lock a root directory?
	    if (!(parent_lock = ParentDir(lock)))
	    {
		STRPTR path;

		if (flags&PFLF_USE_DEVICENAME)
		{
		    L_DeviceFromLock(lock,tmp_fib->fib_FileName);

		    path_len = stccpy(partname, tmp_fib->fib_FileName, 108);
		}
		// Add one for null-trmination
		else
		    path_len++;

		// Allocate space for result string
		if (path = L_AllocMemH(memory,path_len))
		{
		    STRPTR tmp;

		    // Copy object name
		    tmp = stpcpy(path,partname);

		    if (!(flags&PFLF_USE_DEVICENAME))
		    {
		        // Add ':' and terminate string
		        *tmp = ':';
		        tmp++;
		        *tmp = 0;
		    }

		    // Free temporary buffer
		    FreeMem(parts,sizeof(struct PathParts));

		    path_len--;

		    set_d1(path_len);
		    return path;
		}
	    }

	    // Save temporary buffer-space used
	    parts_used = path_len;
	    // tmp_parts = parts;

	    do
	    {
		UBYTE len;

		// Is this temporary buffer part full
		if (parts_used >= 250)
		{
		    // Allocate new temporary buffer part
		    if (!(tmp_parts = AllocMem(sizeof(struct PathParts),MEMF_ANY)))
		    {
			failed = TRUE;
			break;
		    }

		    // Link in front of previous buffer part
		    tmp_parts->next_parts = parts;
		    parts = tmp_parts;
		    parts->truncated = 0;
		    partname = parts->names;

		    // Reset part usage counter
		    parts_used = 0;
		}

		if (trunc)
		{
		    // Copy rest of name into new buffer
		    len = stccpy(partname, tmp_fib->fib_FileName + trunc, 250);
		    path_len--;
		    parts->truncated = trunc;
		    trunc = 0;
		}
		else
		{
		    BPTR tmp_lock;

		    // Do we want device name name for root?
		    if (!(tmp_lock = ParentDir(parent_lock)) && (flags&PFLF_USE_DEVICENAME))
		    {
			char a;

			L_DeviceFromLock(parent_lock,tmp_fib->fib_FileName);

			// Remove trailing ':'
			a = strlen(tmp_fib->fib_FileName);
			tmp_fib->fib_FileName[a - 1] = 0;
		    }
		    else
		        // Get name
		        Examine(parent_lock,tmp_fib);

		    // Copy into temporary buffer
		    len = stccpy(partname + parts_used, tmp_fib->fib_FileName, 250 - parts_used);

		    //Did we fill this temporary buffer part before whole name was copied?
		    if (len == 250 - parts_used && tmp_fib->fib_FileName[len])
			trunc = len - 1;

		    // Unlock current parent and get next
		    UnLock(parent_lock);
		    parent_lock = tmp_lock;
		}
		this_part = parts_used;
		path_len += len;

		// Update part usage
		parts_used += len;

		if (this_part)
		{
		    if (parts_used > 247)
			parts_used = 250;

		    // Save array-position of start of this name
		    partname[parts_used] = this_part;
		    parts_used++;
		}
	    } while (parent_lock);
	}

	if (!failed)
	{
	    STRPTR path;
	    ULONG append_len = 0, alloc_size = path_len;

	    if (flags&PFLF_END_SLASH)
		alloc_size++;

	    if (append)
	    {
		append_len = strlen(append);
		alloc_size += append_len;
		if (flags&PFLF_SLASH_APPEND)
		    alloc_size++;
	    }

	    // Allocate space for result string
	    if (path = L_AllocMemH(memory,alloc_size))
	    {
		STRPTR pos = path;
		BOOL first = TRUE;

		partname = parts->names;

		FOREVER
		{
		    // Is this name truncated?
		    if (!this_part && parts->truncated)
		    {
		        STRPTR trunc_part;

			// Get first part of name from next buffer part
		        trunc_part = parts->next_parts->names + parts->next_parts->names[250];

			// Copy to result string
			pos = stpcpy(pos, trunc_part);

			// Update offset to point to next object name
			parts->next_parts->names[250] = trunc_part[-1];

		    }

		    // Copy to result string
		    pos = stpcpy(pos, partname + this_part);

		    if (first)
		    {
			*pos = ':';
			pos++;
			first = FALSE;
		    }
		    else
		    if (pos + 1 < path + path_len)
		    {
			*pos = '/';
			pos++;
		    }
		    else
			break;

		    // Done with this buffer part?
		    if (!this_part)
		    {
		        tmp_parts = parts;
		        parts = tmp_parts->next_parts;
		        FreeMem(tmp_parts,sizeof(struct PathParts));
			partname = parts->names;
		        this_part = partname[250];
		    }
		    // Get array-position of beginning of next name
		    else
			this_part = partname[this_part - 1];
		}

		FreeMem(parts,sizeof(struct PathParts));

		if (append_len)
		{
		    if (flags&PFLF_SLASH_APPEND)
		    {
			path[path_len - 1] = '/';
			path_len++;
		    }
		    strcpy(path + path_len - 1 , append);
		    path_len += append_len;
		}

		if (flags&PFLF_END_SLASH)
		{
		    path[path_len - 1] = '/';
		    path[path_len] = 0;
		    path_len++;
		}

		path_len--;

		set_d1(path_len);
		return path;
	    }
	}

	// If we arrive here something failed after allocating
	// the first temporary buffer
	do
	{
	    tmp_parts = parts->next_parts;
	    FreeMem(parts,sizeof(struct PathParts));
	    parts = tmp_parts;
	} while (parts);
    }

    return 0;
}
