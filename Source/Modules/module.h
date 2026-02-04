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

#ifndef _GALILEOFM_MODULE
#define _GALILEOFM_MODULE

#include <gfm/galileo_ipc.h>

typedef struct _PathNode
{
	struct MinNode		pn_node;
	char			*pn_path_buf;
	char			*pn_path;
	APTR			pn_lister;
	ULONG			pn_flags;
	BPTR			pn_lock;
	BPTR			pn_recurse_lock;
} PathNode;

typedef struct _FunctionEntry
{
	struct MinNode		fe_node;
	char			*fe_name;
	struct DirEntry		*fe_entry;
	short			fe_type;
	short			fe_flags;
	BPTR			fe_lock;
} FunctionEntry;

#define LISTNF_UPDATE_STAMP	(1<<5)	// Update datestamp
#define LISTNF_RESCAN		(1<<6)	// Rescan this lister



// Flags for gc_RefreshLister()
#define HOOKREFRESH_DATE	(1<<0)
#define HOOKREFRESH_FULL	(1<<1)


// Type value for gc_ExamineEntry()
enum
{
	EE_NAME,		// Get pointer to name (READ ONLY!)
	EE_TYPE,		// Get type (<0 = file, >0 = dir)
};

// Values for gc_FileSet/gc_FileQuery

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
#define HFFS_VERSION		( TAG_USER + 0xc )	// VersionInfo *	- Version information
#define HFFS_MENU		( TAG_USER + 0xd )	// struct List *	- Custom menus for entry
#define HFFS_ICON		( TAG_USER + 0xe )	// struct DiskObject *	- not implemented

#define HLT_FAKEDIR		( TAG_USER + 0x1 )	// BOOL			- Not a real dir
#define HLT_SOURCEDEST_LOCK	( TAG_USER + 0x2 )	// BOOL			- Locked as source or destination
#define HLT_CURBUFFER_LOCK	( TAG_USER + 0x3 )	// BPTR			- Lock on current dir

#define CBCDF_CLEAR_PATH	    (1<<0)
#define CBCDF_CLEAR_EXPANDEDPATH    (1<<1)
#define CBCDF_CLEAR_LOCK            (1<<2)
#define CBCDF_CLEAR_HANDLER         (1<<3)

typedef struct
{
	// Set to the number of hooks you want
	ULONG	    gc_Count;

	// Create a file entry
	APTR		(* __asm gc_CreateFileEntry)
				(register __a0 APTR lister,
				 register __a1 struct FileInfoBlock *fib,
				 register __d0 BPTR lock);

	// Change parameters of a file entry
	VOID		(* __asm gc_FileSet)
				(register __a0 APTR lister,
				 register __a1 APTR entry,
				 register __a2 struct TagItem *tags);

	// Sort list of entries into a lister
	VOID		(* __asm gc_SortFileList)
				(register __a0 APTR lister,
				 register __a1 struct List *list,
				 register __d0 long file_count,
				 register __d1 long dir_count);

	// Add single entry to a lister
	APTR		(* __asm gc_AddFileEntry)
				(register __a0 APTR lister,
				 register __a1 APTR entry,
				 register __d0 BOOL sort);

	// Resort a lister
	VOID		(* __asm gc_ResortLister)
				(register __a0 APTR lister,
				 register __a1 struct ListFormat *format);

	// Refresh a lister
	VOID		(* __asm gc_RefreshLister)
				(register __a0 APTR lister,
				 register __d0 ULONG flags,
				 register __d1 BOOL wait);

	// Lock lister file list
	VOID		(* __asm gc_LockFileList)
				(register __a0 APTR lister,
				 register __d0 BOOL exclusive);

	// Unlock file list
	VOID		(* __asm gc_UnlockFileList)
				(register __a0 APTR lister);

	// Find entry in a lister by name
	APTR		(* __asm gc_FindFileEntry)
				(register __a0 APTR lister,
				 register __a1 char *name);

	// Change comment of an entry
	BOOL		(* __asm gc_SetFileComment)
				(register __a0 APTR lister,
				 register __a1 char *name,
				 register __a2 char *comment);

	// Remove file entry from a lister
	VOID		(* __asm gc_RemoveFileEntry)
				(register __a0 APTR lister,
				 register __a1 APTR entry);

	// Query file entry
	BOOL		(* __asm gc_FileQuery)
				(register __a0 APTR lister,
				 register __a1 APTR entry,
				 register __a2 struct TagItem *tags);

	// Show help
	void		(* __asm gc_ShowHelp)
				(register __a0 char *file_name,
				 register __a1 char *node_name);


	// Convert entry pointer from one type to another
	APTR		(* __asm gc_ConvertEntry)
				(register __a0 APTR entry);


	// Get lister pointer from a path handle
	APTR		(* __asm gc_GetLister)
				(register __a0 APTR path);


	// Get first source lister
	APTR		(* __asm gc_GetSource)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// Get next source lister
	APTR		(* __asm gc_NextSource)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// Unlock source listers
	void		(* __asm gc_UnlockSource)
				(register __a0 APTR handle);


	// Get next destination lister
	APTR		(* __asm gc_GetDest)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// End use of source lister
	void		(* __asm gc_EndSource)
				(register __a0 APTR handle,
				 register __d0 long complete);


	// End use of destination lister
	void		(* __asm gc_EndDest)
				(register __a0 APTR handle,
				 register __d0 long complete);


	// Get next selected entry
	APTR		(* __asm gc_GetEntry)
				(register __a0 APTR handle);


	// Examine an entry
	ULONG		(* __asm gc_ExamineEntry)
				(register __a0 APTR entry,
				 register __d0 long type);


	// End use of an entry
	void		(* __asm gc_EndEntry)
				(register __a0 APTR handle,
				 register __a1 APTR entry,
				 register __d0 BOOL deselect);


	// Mark an entry for removal
	void		(* __asm gc_RemoveEntry)
				(register __a0 APTR entry);


	// Return count of selected entries
	long		(* __asm gc_EntryCount)
				(register __a0 APTR handle);


	// Mark an entry to be reloaded
	void		(* __asm gc_ReloadEntry)
				(register __a0 APTR handle,
				 register __a1 APTR entry);


	// Add a file to a lister
	void		(* __asm gc_AddFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 struct FileInfoBlock *fib,
				 register __a3 APTR lister,
				 register __d0 BPTR lock);


	// Delete a file from a lister
	void		(* __asm gc_DelFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 char *name,
				 register __a3 APTR lister,
				 register __d0 BPTR lock);

	// Load/reload a file in a lister
	void		(* __asm gc_LoadFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 char *name,
				 register __d0 BPTR lock,
				 register __d1 long flags,
				 register __d2 BOOL reload);


	// Perform changes on a lister
	void		(* __asm gc_DoChanges)
				(register __a0 APTR handle);


	// Check for user abort
	BOOL		(* __asm gc_CheckAbort)
				(register __a0 APTR handle);


	// Get window pointer from a lister pointer
	struct Window *	(* __asm gc_GetWindow)
				(register __a0 APTR lister);


	// Get Galileo ARexx port name
	struct MsgPort *	(* __asm gc_GetPort)
				(register __a0 char *name);


	// Get Galileo public screen name
	struct Screen *	(* __asm gc_GetScreen)
				(register __a0 char *name);


	// Get information about the Galileo screen
	struct GalileoScreenData *(* __asm gc_GetScreenData)(void);


	// Free screen data structure
	void		(* __asm gc_FreeScreenData)(void);


	// Open progress indicator in a lister
	void		(* __asm gc_OpenProgress)
				(register __a0 APTR path,
				 register __a1 char *operation,
				 register __d0 long total);


	// Update progress indicator
	void		(* __asm gc_UpdateProgress)
				(register __a0 APTR path,
				 register __a1 char *name,
				 register __d0 long count);


	// Close progress indicator
	void		(* __asm gc_CloseProgress)
				(register __a0 APTR path);


	// Show 'File exists - Replace?' requester
	long		(* __asm gc_ReplaceReq)
				(register __a0 struct Window *window,
				 register __a1 struct Screen *screen,
				 register __a2 IPCData *ipc,
				 register __a3 struct FileInfoBlock *file1,
				 register __a4 struct FileInfoBlock *file2,
				 register __d0 long flags);


	// Get pointer to Galileo internal things
	APTR		(* __asm gc_GetPointer)
				(register __a0 struct pointer_packet *);
	void		(* __asm gc_FreePointer)
				(register __a0 struct pointer_packet *);


	// Send an ARexx command direct to Galileo
	ULONG		(* __asm gc_SendCommand)
				(register __a0 APTR handle,
				 register __a1 char *command,
				 register __a2 char **result,
				 register __d0 ULONG flags);


	// Make Galileo check if the desktop needs updating
	void		(* __asm gc_CheckDesktop)
				(register __a0 char *path);


	// Get desktop path
	short		(* __asm gc_GetDesktop)
			    (register __a0 char *buffer);

	// Run script
	short		(* __asm gc_Script)
				(register __a0 char *name,
				 register __a1 char *data);

	// Popup desktop popup
	short		(* __asm gc_DesktopPopup)
				(register __a0 ULONG flags);

	// Reset file entries
	void		(* __asm gc_FirstEntry)
				(register __a0 APTR handle,
				 register __d0 ULONG flags);

	// Send rexx command direct to Galileo
	long		(* __asm gc_RexxCommand)
				(register __a0 char *command,
				 register __a1 char *result,
				 register __d0 long length,
				 register __a2 struct MsgPort *replyport,
				 register __d1 ULONG flags);

	// Show file requester
	long		(* __asm gc_FileRequest)
				(register __a0 struct Window *parent,
				 register __a1 char *title,
				 register __a2 char *initial_path,
				 register __a3 char *pathname,
				 register __d0 ULONG flags,
				 register __d1 char *pattern);

	// Get themes path
	void		(* __asm gc_GetThemes)
				(register __a0 char *buffer);

	// Free pointer without struct pointer_packet
	void		(* __asm gc_FreePointerDirect)
				(register __a0 APTR pointer,
	    			 register __d0 ULONG type,
	    			 register __d1 ULONG flags);

	void		(* __asm gc_ListerSet)
				(register __a0 APTR lister,
				 register __a1 struct TagItem *tags);

	BOOL		(* __asm gc_ListerGet)
				(register __a0 APTR lister,
				 register __a1 struct TagItem *tags);

	APTR		(* __asm gc_GetFileEntry)
				(register __a0 APTR lister,
				 register __a1 APTR from_entry,
				 register __d0 BOOL selected);

	APTR		(* __asm gc_NewLister)
				(register __a0 STRPTR path,
				 register __a1 APTR parent,
			         register __a2 STRPTR toolbar_path,
			         register __d0 BPTR path_lock,
			         register __d1 ULONG flags,
			         register __d2 WORD Left,
			         register __d3 WORD Top,
			         register __d4 WORD Width,
				 register __d5 WORD Height);

	void		(* __asm gc_ReadDir)
				(register __a0 APTR lister,
				 register __a1 STRPTR path,
				 register __d0 ULONG flags,
				 register __d1 BPTR path_lock);

	void		(* __asm gc_ClearDir)
				(register __a0 APTR lister,
				 register __d0 ULONG flags);

} GalileoCallbackInfo;

int __saveds __asm L_Module_Entry(register __a0 char *args,
				  register __a1 struct Screen *screen,
				  register __a2 IPCData *ipc,
				  register __a3 IPCData *main_ipc,
				  register __d0 ULONG mod_id,
				  register __d1 CONST GalileoCallbackInfo *gci);


#endif
