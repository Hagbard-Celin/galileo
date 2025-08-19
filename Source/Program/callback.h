/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023-2025 Hagbard Celine

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

#ifndef GALILEOFM_CALLBACK_H
#define GALILEOFM_CALLBACK_H

#define MAINCMD_GET_CALLBACKS	0x40100


typedef struct
{
	ULONG	gc_Count;

	APTR	gc_CreateFileEntry;
	APTR	gc_FileSet;
	APTR	gc_SortFileList;
	APTR	gc_AddFileEntry;
	APTR	gc_ResortLister;
	APTR	gc_RefreshLister;
	APTR	gc_LockFileList;
	APTR	gc_UnlockFileList;
	APTR	gc_FindFileEntry;
	APTR	gc_SetFileComment;
	APTR	gc_RemoveFileEntry;
	APTR	gc_FileQuery;
	APTR	gc_ShowHelp;

	APTR	gc_ConvertEntry;
	APTR	gc_GetLister;
	APTR	gc_GetSource;
	APTR	gc_NextSource;
	APTR	gc_UnlockSource;
	APTR	gc_GetDest;
	APTR	gc_EndSource;
	APTR	gc_EndDest;
	APTR	gc_GetEntry;
	APTR	gc_ExamineEntry;
	APTR	gc_EndEntry;
	APTR	gc_RemoveEntry;
	APTR	gc_EntryCount;
	APTR	gc_ReloadEntry;
	APTR	gc_AddFile;
	APTR	gc_DelFile;
	APTR	gc_LoadFile;
	APTR	gc_DoChanges;
	APTR	gc_CheckAbort;
	APTR	gc_GetWindow;
	APTR	gc_GetPort;
	APTR	gc_GetScreen;
	APTR	gc_GetScreenData;
	APTR	gc_FreeScreenData;
	APTR	gc_OpenProgress;
	APTR	gc_UpdateProgress;
	APTR	gc_CloseProgress;
	APTR	gc_ReplaceReq;
	APTR	gc_GetPointer;
	APTR	gc_FreePointer;
	APTR	gc_SendCommand;
	APTR	gc_CheckDesktop;
	APTR	gc_GetDesktop;
	APTR	gc_Script;
	APTR	gc_DesktopPopup;
	APTR	gc_FirstEntry;
	APTR	gc_RexxCommand;
	APTR	gc_FileRequest;
	APTR	gc_ListerSet;
	APTR    gc_ListerGet;
	APTR	gc_GetThemes;
	APTR    gc_FreePointerDirect;
	APTR	gc_GetFileEntry;
	APTR	gc_NewLister;
	APTR	gc_ReadDir;
	APTR	gc_ClearDir;
} GalileoCallbackInfo;

#define GALILEOFM_CALLBACK_COUNT    59

// Values for HookFileSet

#define HFFS_NAME		( TAG_USER + 0x1 )	// char * 		- Entry name
#define HFFS_SIZE		( TAG_USER + 0x2 )	// ULONG 		- Entry size
#define HFFS_PROTECTION		( TAG_USER + 0x3 )	// ULONG 		- Protection flags
#define HFFS_DATE		( TAG_USER + 0x4 )	// struct Datestamp *	- Entry date
#define HFFS_COMMENT		( TAG_USER + 0x5 )	// char *		- Comment
#define HFFS_SELECTED		( TAG_USER + 0x6 )	// BOOL			- Selected state
#define HFFS_LINK		( TAG_USER + 0x7 )	// BOOL			- Set if a link
#define HFFS_COLOUR		( TAG_USER + 0x8 )	// ULONG		- 1 = device, 2 = assign
#define HFFS_USERDATA		( TAG_USER + 0x9 )	// ULONG		- Userdata
#define HFFS_FILETYPE		( TAG_USER + 0xa )	// char *		- Filetype description
#define HFFS_DISPLAY		( TAG_USER + 0xb )	// char *		- Custom display string
#define HFFS_VERSION		( TAG_USER + 0xc )	// ULONG *		- version, revision, days
#define HFFS_MENU		( TAG_USER + 0xd )	// void *		- not implemented

#define HLT_FAKEDIR		( TAG_USER + 0x1 )	// BOOL			- Not a real dir
#define HLT_SOURCEDEST_LOCK	( TAG_USER + 0x2 )	// BOOL			- Locked as source or destination
#define HLT_CURBUFFER_LOCK	( TAG_USER + 0x3 )	// BPTR			- Lock on current dir


#define HOOKREFRESH_DATE	(1<<0)
#define HOOKREFRESH_FULL	(1<<1)

#define CBCDF_CLEAR_EXPANDEDPATH    (1<<0)
#define CBCDF_CLEAR_PATH	    (1<<1)
#define CBCDF_CLEAR_LOCK            (1<<2)
#define CBCDF_CLEAR_HANDLER         (1<<3)

enum
{
	EE_NAME,		// Get name pointer (READ ONLY!)
	EE_TYPE,		// Get type (<0 = file, >0 = dir)
};

#endif
