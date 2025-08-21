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

#include "galileofmlib.h"


/********************************** CreateDir **********************************/

// Patched CreateDir()    
BPTR __asm __saveds L_PatchedCreateDir(register __d1 char *name)
{
	struct FileInfoBlock *fib;
	BPTR lock;

	// Call original function
	lock=((BPTR (* __asm)
			(register __d1 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_CREATEDIR])
			(name,DOSBase);

	// Failed?
	if (!lock) return 0;

	// Get directory information
	if (fib=dospatch_fib(lock,1))
	{
		// Send notification
		L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_CREATEDIR,0,(char *)(fib+1),fib);

		// Free info block
		FreeVec(fib);
	}
	return lock;
}


// Calls original CreateDir directly
BPTR __asm __saveds L_OriginalCreateDir(
	register __d1 char *name)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return CreateDir(name);

	// Create directory
	return
		((BPTR (* __asm)
			(register __d1 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_CREATEDIR])
			(name,DOSBase);
}

/********************************** DeleteFile **********************************/

// Patched DeleteFile()
long __asm __saveds L_PatchedDeleteFile(register __d1 char *name)
{
	char *buf=0;
	BPTR lock;
	long res;
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Try to lock file to delete
	if (lock=Lock(name,ACCESS_READ))
	{
		// Get full path
		buf = L_PathFromLock(NULL, lock, PFLF_USE_DEVICENAME, NULL);

		// Unlock file
		UnLock(lock);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;

	// Call original function
	res=((long (* __asm)
			(register __d1 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_DELETEFILE])
			(name,DOSBase);

	// Failed?
	if (!res) return 0;

	// Got name?
	if (buf && *buf)
	{
		// Send notification
		L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_DELETEFILE,0,buf,0);
	}

	// Free buffer
	if (buf)
	    L_FreeMemH(buf);

	return res;
}


// Original DeleteFile
long __asm __saveds L_OriginalDeleteFile(
	register __d1 char *name)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return DeleteFile(name);

	// Delete file
	return
		((BPTR (* __asm)
			(register __d1 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_DELETEFILE])
			(name,DOSBase);
}


/********************************** SetFileDate **********************************/

// Patched SetFileDate()
BOOL __asm __saveds L_PatchedSetFileDate(
	register __d1 char *name,
	register __d2 struct DateStamp *date)
{
	BOOL res;
	BPTR lock;
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Call original function
	res=((BOOL (* __asm)
			(register __d1 char *,register __d2 struct DateStamp *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETFILEDATE])
			(name,date,DOSBase);

	// Failed?
	if (!res) return 0;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Try to lock file
	if (lock=Lock(name,ACCESS_READ))
	{
		struct FileInfoBlock *fib;

		// Get directory information
		if (fib=dospatch_fib(lock,0))
		{
			// Send notification
			L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_SETFILEDATE,0,(char *)(fib+1),fib);

			// Free info block
			FreeVec(fib);
		}

		// Unlock file
		UnLock(lock);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;

	return res;
}


// Calls original SetFileDate directly
BOOL __asm __saveds L_OriginalSetFileDate(
	register __d1 char *name,
	register __d2 struct DateStamp *date)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return (BOOL)SetFileDate(name,date);

	// Set date
	return
		((BOOL (* __asm)
			(register __d1 char *,register __d2 struct DateStamp *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETFILEDATE])
			(name,date,DOSBase);
}


/********************************** SetComment **********************************/

// Patched SetComment()
BOOL __asm __saveds L_PatchedSetComment(
	register __d1 char *name,
	register __d2 char *comment)
{
	BOOL res;
	BPTR lock;
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Call original function
	res=((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETCOMMENT])
			(name,comment,DOSBase);

	// Failed?
	if (!res) return 0;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Lock file
	if (lock=Lock(name,ACCESS_READ))
	{
		struct FileInfoBlock *fib;

		// Get directory information
		if (fib=dospatch_fib(lock,0))
		{
			// Send notification
			L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_SETCOMMENT,0,(char *)(fib+1),fib);

			// Free info block
			FreeVec(fib);
		}

		// Unlock file
		UnLock(lock);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;

	return res;
}


// Calls original SetComment directly
BOOL __asm __saveds L_OriginalSetComment(
	register __d1 char *name,
	register __d2 char *comment)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return (BOOL)SetComment(name,comment);

	// Set comment
	return
		((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETCOMMENT])
			(name,comment,DOSBase);
}


/********************************** SetProtection **********************************/

// Patched SetProtection()
BOOL __asm __saveds L_PatchedSetProtection(
	register __d1 char *name,
	register __d2 ULONG mask)
{
	BOOL res;
	BPTR lock;
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Call original function
	res=((BOOL (* __asm)
			(register __d1 char *,register __d2 ULONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETPROTECTION])
			(name,mask,DOSBase);

	// Failed?
	if (!res) return 0;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Lock file
	if (lock=Lock(name,ACCESS_READ))
	{
		struct FileInfoBlock *fib;

		// Get directory information
		if (fib=dospatch_fib(lock,0))
		{
			// Send notification
			L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_SETPROTECTION,0,(char *)(fib+1),fib);

			// Free info block
			FreeVec(fib);
		}

		// Unlock file
		UnLock(lock);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;
	return res;
}


// Calls original SetProtection directly
BOOL __asm __saveds L_OriginalSetProtection(
	register __d1 char *name,
	register __d2 ULONG mask)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return (BOOL)SetProtection(name,mask);

	// Set protection
	return
		((BOOL (* __asm)
			(register __d1 char *,register __d2 ULONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_SETPROTECTION])
			(name,mask,DOSBase);
}


/********************************** Rename **********************************/

// Patched Rename()
BOOL __asm __saveds L_PatchedRename(
	register __d1 char *oldname,
	register __d2 char *newname)
{
	struct FileInfoBlock *fib=0;
	BOOL res;
	BPTR lock;
	char old_name[108];
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Get lock on old file?
	if (lock=Lock(oldname,ACCESS_READ))
	{
		// Get file information
		if (!(fib=dospatch_fib(lock,0)))
		{
			// Failed to get info
			UnLock(lock);
			lock=0;
		}
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;

	// Call original function
	res=((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_RENAME])
			(oldname,newname,DOSBase);

	// Failure?
	if (!res)
	{
		// Unlock and free fib
		if (fib) FreeVec(fib);
		if (lock) UnLock(lock);
		return res;
	}

	// Got no Fib?
	if (!fib) return res;

	// Store old name
	strcpy(old_name,fib->fib_FileName);

	// Turn requesters off
	if (task) task->pr_WindowPtr=(APTR)-1;

	// Get new information
	if (Examine(lock,fib))
	{
		// Copy new name to comment
		stccpy(fib->fib_Comment,fib->fib_FileName,79);

		// Restore old name
		strcpy(fib->fib_FileName,old_name);

		// Send notification
		L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_RENAME,0,(char *)(fib+1),fib);
	}

	// Free info block and lock
	FreeVec(fib);
	UnLock(lock);

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;
	return res;
}


// Calls original Rename directly
BOOL __asm __saveds L_OriginalRename(
	register __d1 char *oldname,
	register __d2 char *newname)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return (BOOL)Rename(oldname,newname);

	// Rename file
	return
		((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_RENAME])
			(oldname,newname,DOSBase);
}


/********************************** Relabel **********************************/

// Patched Relabel()
BOOL __asm __saveds L_PatchedRelabel(
	register __d1 char *volumename,
	register __d2 char *name)
{
	struct FileInfoBlock fib;
	BOOL res;

	// Call original function
	res=((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_RELABEL])
			(volumename,name,DOSBase);

	// Failure?
	if (!res) return res;

	// Fake fib with new name
	strcpy(fib.fib_FileName,name);

	// Send notification
	L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_RELABEL,0,volumename,&fib);
	return res;
}


// Calls original Relabel directly
BOOL __asm __saveds L_OriginalRelabel(
	register __d1 char *volumename,
	register __d2 char *name)
{
	// Relabel disk
	return
		((BOOL (* __asm)
			(register __d1 char *,register __d2 char *,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_RELABEL])
			(volumename,name,DOSBase);
}


/********************************** Open **********************************/

// Patched Open()
BPTR __asm __saveds L_PatchedOpen(
	register __d1 char *name,
	register __d2 LONG accessMode)
{
	BPTR file,lock;
	BOOL create=0;
	APTR wsave=(APTR)-1;
	struct Process *task;

	// Find process
	if (task=(struct Process *)FindTask(0))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Check mode
	switch (accessMode)
	{
		// Create?
		case MODE_NEWFILE:
			create=1;
			break;

		// Read/write
		case MODE_READWRITE:

			// If file is going to be created, we use it
			if (!(lock=Lock(name,ACCESS_READ))) create=1;
			else UnLock(lock);
			break;
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;

	// Call original function
	file=((BPTR (* __asm)
			(register __d1 char *,register __d2 LONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_OPEN])
			(name,accessMode,DOSBase);

	// Failure?
	if (!file) return file;

	// If file is Interactive, assume it's not a filesystem
	if (IsInteractive(file)) return file;

	// Disable requesters
	if (task) task->pr_WindowPtr=(APTR)-1;

	// Get lock on parent directory
#ifdef RESOURCE_TRACKING
	if (lock=NRT_ParentOfFH(file))
#else
	if (lock=ParentOfFH(file))
#endif
	{
		struct FileHandleWrapper *handle;

		// Allocate wrapper handle
		if (handle=AllocVec(sizeof(struct FileHandleWrapper),0))
		{
			// Fill out handle
			handle->fhw_FileHandle=file;
			handle->fhw_Flags=0;
			handle->fhw_Parent=lock;

			// Lock file list
			ObtainSemaphore(&gfmlib_data.file_list.lock);

			// Add to head of list
			AddHead(&gfmlib_data.file_list.list,(struct Node *)handle);

			// Unlock file list
			ReleaseSemaphore(&gfmlib_data.file_list.lock);
		}

		// If this was a create operation, we need to send a message now
		if (create)
		{
			STRPTR name;
			ULONG len;

			// Get full path of parent
			if (name = L_PathFromLock(NULL, lock, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL))
			{
			    struct FileInfoBlock *fib;

			    len = getreg(REG_D1);

			    // Get FileInfoBlock and a buffer
			    if (fib=AllocVec(sizeof(struct FileInfoBlock) + len + 1,MEMF_CLEAR))
			    {
				// Get buffer pointer
				char *path=(char *)(fib+1);

			        // Copy path
			        strcpy(path, name);

				// Examine the new file
				if (ExamineFH(file,fib))
				{
					// Send notification
					L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_CREATE,0,path,fib);
				}

				// Free fib
				FreeVec(fib);
			    }
			    L_FreeMemH(name);
			}
		}

		// Unlock parent, unless handle has grabbed it
		if (!handle) UnLock(lock);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;
	return file;
}


// Calls original Open directly
BPTR __asm __saveds L_OriginalOpen(
	register __d1 char *name,
	register __d2 LONG accessMode)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return Open(name,accessMode);

	// Open file
	return
		((BPTR (* __asm)
			(register __d1 char *,register __d2 LONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_OPEN])
			(name,accessMode,DOSBase);
}


/********************************** Close **********************************/

// Patched Close()
BOOL __asm __saveds L_PatchedClose(register __d1 BPTR file)
{
	// Get library
	if (!file) return 0;

	// If this is the 'last' file, clear pointer
	if (gfmlib_data.last_file==file)
		gfmlib_data.last_file=0;

	// Is file not interactive (ie a filesystem)?
	if (!IsInteractive(file))
	{
		struct FileHandleWrapper *handle;

		// See if we have a wrapper
		if (handle=find_filehandle(file))
		{
			// Remove it from the list
			Remove((struct Node *)handle);

			// Unlock the list
			ReleaseSemaphore(&gfmlib_data.file_list.lock);

			// Did the file get written to?
			if (handle->fhw_Flags&FHWF_WRITTEN)
			{
				struct FileInfoBlock *fib;
				APTR wsave=(APTR)-1;
				struct Process *task;
				STRPTR name;
				ULONG len;

				// Find process
				if (task=(struct Process *)FindTask(0))
				{
					// Is it a process?
					if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
					{
						// Save pointer and turn requesters off
						wsave=task->pr_WindowPtr;
						task->pr_WindowPtr=(APTR)-1;
					}
					else task=0;
				}



				// Get full path of parent, and terminate it
				if (name = L_PathFromLock(NULL, handle->fhw_Parent, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL))
				{
				    len = getreg(REG_D1);
				    // Get FileInfoBlock and a buffer
				    if (fib=AllocVec(sizeof(struct FileInfoBlock) + len + 1,MEMF_CLEAR))
				    {
					char *path;

				        // Get buffer pointer
					path=(char *)(fib+1);

				        // Copy path
					strcpy(path, name);

					// Examine the file
					if (ExamineFH(file,fib))
					{
						// Send notification
						L_SendNotifyMsg(GN_DOS_ACTION,0,GNF_DOS_CLOSE,0,path,fib);
					}

					// Free fib
					FreeVec(fib);
				    }
				    L_FreeMemH(name);
				}

				// Fix requesters
				if (task) task->pr_WindowPtr=wsave;
			}

			// Unlock parent lock
#ifdef RESOURCE_TRACKING
			NRT_UnLock(handle->fhw_Parent);
#else
			UnLock(handle->fhw_Parent);
#endif
			// Free wrapper handle
			FreeVec(handle);
		}
	}

	// Close file
	return
		((BOOL (* __asm)
			(register __d1 BPTR,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_CLOSE])
			(file,DOSBase);
}


// Calls original Close directly
BOOL __asm __saveds L_OriginalClose(
	register __d1 BPTR file)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return (BOOL)Close(file);

	// Close file
	return
		((BOOL (* __asm)
			(register __d1 BPTR,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_CLOSE])
			(file,DOSBase);
}


/********************************** Write **********************************/

// Patched Write()
LONG __asm __saveds L_PatchedWrite(
	register __d1 BPTR file,
	register __d2 void *wdata,
	register __d3 LONG length)
{
	struct FileHandleWrapper *handle;

	// Get library
	if (!file) return 0;

	// Different to the last file we saw?
	if (gfmlib_data.last_file!=file)
	{
		// See if we have a wrapper
		if (handle=find_filehandle(file))
		{
			// Set 'written' flag
			handle->fhw_Flags|=FHWF_WRITTEN;

			// Unlock the list
			ReleaseSemaphore(&gfmlib_data.file_list.lock);
		}

		// Remember this file
		gfmlib_data.last_file=file;
	}

	// Write data
	return
		((LONG (* __asm)
			(register __d1 BPTR,register __d2 void *,register __d3 LONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_WRITE])
			(file,wdata,length,DOSBase);
}


// Calls original Write directly
LONG __asm __saveds L_OriginalWrite(
	register __d1 BPTR file,
	register __d2 void *wdata,
	register __d3 LONG length)
{
	// If patch wasn't installed, call DOS directly
	if (!(gfmlib_data.flags&LIBDF_DOS_PATCH))
		return Write(file,wdata,length);

	// Write data
	return
		((LONG (* __asm)
			(register __d1 BPTR,register __d2 void *,register __d3 LONG,register __a6 struct DosLibrary *))
				gfmlib_data.wb_data.old_function[WB_PATCH_WRITE])
			(file,wdata,length,DOSBase);
}


/******************************************************************************/

// Get information on a filelock
struct FileInfoBlock *dospatch_fib(BPTR lock,BOOL req)
{
	struct FileInfoBlock *fib=0;
	BPTR parent;
	BOOL ok=0;
	APTR wsave=(APTR)-1;
	struct Process *task=0;

	// Find process
	if (req && (task=(struct Process *)FindTask(0)))
	{
		// Is it a process?
		if (task->pr_Task.tc_Node.ln_Type==NT_PROCESS)
		{
			// Save pointer and turn requesters off
			wsave=task->pr_WindowPtr;
			task->pr_WindowPtr=(APTR)-1;
		}
		else task=0;
	}

	// Get parent directory
	if (parent=ParentDir(lock))
	{
		STRPTR name;
		ULONG len;

		// Get full path of parent, and terminate it
		if (name = L_PathFromLock(NULL, parent, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL))
		{
		    len = getreg(REG_D1);
		    // Get FileInfoBlock and a buffer
		    if (fib=AllocVec(sizeof(struct FileInfoBlock) + len + 1,MEMF_CLEAR))
		    {
			    char *buf;

			    // Get buffer pointer
			    buf=(char *)(fib+1);

			    // Copy path
			    strcpy(buf, name);

			    // Examine the lock
			    if (Examine(lock,fib)) ok=1;
		    }
		    L_FreeMemH(name);
		}

		// Not ok?
		if (!ok)
		{
			FreeVec(fib);
			fib=0;
		}

		// Unlock parent
		UnLock(parent);
	}

	// Fix requesters
	if (task) task->pr_WindowPtr=wsave;
	return fib;
}

// Find FileHandle in tracking list
struct FileHandleWrapper *find_filehandle(BPTR file)
{
	struct FileHandleWrapper *handle;

	// Lock list
	ObtainSemaphore(&gfmlib_data.file_list.lock);

	// Go through list
	for (handle=(struct FileHandleWrapper *)gfmlib_data.file_list.list.lh_Head;
		handle->fhw_Node.mln_Succ;
		handle=(struct FileHandleWrapper *)handle->fhw_Node.mln_Succ)
	{
		// Match handle
		if (handle->fhw_FileHandle==file)
		{
			// Not the top one?
			if (handle!=(struct FileHandleWrapper *)gfmlib_data.file_list.list.lh_Head)
			{
				// Shift it to the top as the most recently accessed
				Remove((struct Node *)handle);
				AddHead(&gfmlib_data.file_list.list,(struct Node *)handle);
			}

			// Return the handle (leave the list locked)
			return handle;
		}
	}

	// Not found
	ReleaseSemaphore(&gfmlib_data.file_list.lock);
	return 0;
}
