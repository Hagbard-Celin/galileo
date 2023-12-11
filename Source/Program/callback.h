/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023 Hagbard Celine

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

#define MAINCMD_GET_CALLBACKS	0x40100

DirEntry *__asm HookCreateFileEntry(
	register __a0 Lister *lister,
	register __a1 struct FileInfoBlock *fib,
	register __d0 BPTR lock);

DirEntry *__asm HookAddFileEntry(
	register __a0 Lister *lister,
	register __a1 DirEntry *entry,
	register __d0 BOOL sort);

void __asm HookFileSet(
	register __a0 Lister *lister,
	register __a1 DirEntry *entry,
	register __a2 struct TagItem *tags);

void __asm HookSortFileList(
	register __a0 Lister *lister,
	register __a1 struct List *list,
	register __d0 long file_count,
	register __d1 long dir_count);

void __asm HookResortLister(
	register __a0 Lister *lister,
	register __a1 ListFormat *format);

void __asm HookRefreshLister(
	register __a0 Lister *lister,
	register __d0 ULONG flags);

void __asm HookLockFileList(
	register __a0 Lister *lister,
	register __d0 BOOL exclusive);

void __asm HookUnlockFileList(
	register __a0 Lister *lister);

DirEntry *__asm HookFindFileEntry(
	register __a0 Lister *lister,
	register __a1 char *name);

BOOL __asm HookSetFileComment(
	register __a0 Lister *lister,
	register __a1 char *name,
	register __a2 char *comment);

void __asm HookRemoveFileEntry(
	register __a0 Lister *lister,
	register __a1 DirEntry *entry);

BOOL __asm HookFileQuery(
	register __a0 Lister *lister,
	register __a1 DirEntry *entry,
	register __a2 struct TagItem *tags);

void __asm HookShowHelp(
	register __a0 char *file_name,
	register __a1 char *node_name);


APTR __asm HookConvertEntry(
	register __a0 FunctionEntry *entry);

Lister *__asm HookGetLister(
	register __a0 PathNode *path);

PathNode *__asm HookGetSource(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf);

PathNode *__asm HookNextSource(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf);

void __asm HookUnlockSource(
	register __a0 FunctionHandle *handle);

PathNode *__asm HookGetDest(
	register __a0 FunctionHandle *handle,
	register __a1 char *pathbuf);

void __asm HookEndSource(
	register __a0 FunctionHandle *handle,
	register __d0 long complete);

void __asm HookEndDest(
	register __a0 FunctionHandle *handle,
	register __d0 long complete);

void __asm HookFirstEntry(
	register __a0 FunctionHandle *handle);

APTR __asm HookGetEntry(
	register __a0 FunctionHandle *handle);

ULONG __asm HookExamineEntry(
	register __a0 FunctionEntry *entry,
	register __d0 long type);

void __asm HookEndEntry(
	register __a0 FunctionHandle *handle,
	register __a1 APTR entry,
	register __d0 BOOL deselect);

void __asm HookRemoveEntry(
	register __a0 FunctionEntry *entry);

long __asm HookEntryCount(
	register __a0 FunctionHandle *handle);

void __asm HookReloadEntry(
	register __a0 FunctionHandle *handle,
	register __a1 FunctionEntry *entry);

void __asm HookAddFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 struct FileInfoBlock *fib,
	register __a3 Lister *lister);

void __asm HookDelFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __a3 Lister *lister);

void __asm HookLoadFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __d0 long flags,
	register __d1 BOOL reload);

void __asm HookDoChanges(
	register __a0 FunctionHandle *handle);

BOOL __asm HookCheckAbort(
	register __a0 FunctionHandle *handle);

struct Window *__asm HookGetWindow(
	register __a0 PathNode *path);

struct MsgPort *__asm HookGetPort(
	register __a0 char *portname);

struct Screen *__asm HookGetScreen(
	register __a0 char *screenname);

GalileoScreenData *__asm HookGetScreenData(void);

void __asm HookFreeScreenData(register __a0 APTR data);

void __asm HookOpenProgress(
	register __a0 PathNode *path,
	register __a1 char *operation,
	register __d0 long total);

void __asm HookUpdateProgress(
	register __a0 PathNode *path,
	register __a1 char *name,
	register __d0 long count);

void __asm HookCloseProgress(
	register __a0 PathNode *path);

long __asm HookReplaceReq(
	register __a0 struct Window *window,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 struct FileInfoBlock *file1,
	register __a4 struct FileInfoBlock *file2,
	register __d0 long default_option);

ULONG __asm HookGetPointer(
	register __a0 struct pointer_packet *ptr);

void __asm HookFreePointer(
	register __a0 struct pointer_packet *ptr);

ULONG __asm HookSendCommand(
	register __a0 FunctionHandle *handle,
	register __a1 char *command,
	register __a2 char **result,
	register __d0 ULONG flags);

void __asm HookCheckDesktop(
	register __a0 char *path);

BOOL __asm HookMatchDesktop(
	register __a0 char *path);

USHORT __asm HookGetDesktop(
	register __a0 char *path);

void __asm HookGetThemes(
	register __a0 char *path);

BOOL __asm HookScript(
	register __a0 char *name,
	register __a1 char *data);

short __asm HookDesktopPopup(
	register __a0 ULONG flags);

long __asm HookRexxCommand(
	register __a0 char *command,
	register __a1 char *result,
	register __d0 long length,
	register __a2 struct MsgPort *replyport,
	register __d1 ULONG flags);

long __asm HookFileRequest(
	register __a0 struct Window *parent,
	register __a1 char *title,
	register __a2 char *initial_path,
	register __a3 char *pathname,
	register __d0 ULONG flags,
	register __d1 char *pattern);

void __asm __saveds HookFreePointerDirect(
	register __a0 APTR pointer,
    register __d0 ULONG type,
    register __d1 ULONG flags);

BOOL __asm __saveds HookIsSourceDestLock(
    register __a0 Lister *lister);

typedef struct
{
	USHORT	gc_Count;

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
	APTR	gc_GetThemes;
    APTR    gc_FreePointerDirect;
    APTR    gc_IsSourceDestLock;
} GalileoCallbackInfo;

#define GALILEOFM_HOOK_COUNT	54

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



#define HOOKREFRESH_DATE	(1<<0)
#define HOOKREFRESH_FULL	(1<<1)

long HookInitHooks(GalileoCallbackInfo *);

enum
{
	EE_NAME,		// Get name pointer (READ ONLY!)
	EE_TYPE,		// Get type (<0 = file, >0 = dir)
};
