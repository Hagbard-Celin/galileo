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

#ifndef GALILEOFM_CALLBACK_PROTOS
#define GALILEOFM_CALLBACK_PROTOS

#include "function_launch.h"
#include "callback.h"

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
	register __d0 ULONG flags,
	register __d1 BOOL wait);

void __asm HookLockFileList(
	register __a0 Lister *lister,
	register __d0 BOOL exclusive);

void __asm HookUnlockFileList(
	register __a0 Lister *lister);

DirEntry *__asm HookFindFileEntry(
	register __a0 Lister *lister,
	register __a1 char *name);

DirEntry *__asm HookGetFileEntry(
	register __a0 Lister *lister,
	register __a1 DirEntry *from_entry,
	register __d0 BOOL selected);

BOOL __asm HookSetFileComment(
	register __a0 Lister *lister,
	register __a1 char *name,
	register __a2 char *comment);

void __asm HookRemoveFileEntry(
	register __a0 Lister *lister,
	register __a1 DirEntry *entry);

void __asm HookListerSet(
	register __a0 Lister *lister,
	register __a1 struct TagItem *tags);

BOOL __asm HookListerGet(
	register __a0 Lister *lister,
	register __a1 struct TagItem *tags);

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
	register __a0 FunctionHandle *handle,
	register __d0 ULONG flags);

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
	register __a3 Lister *lister,
	register __d0 BPTR lock);

void __asm HookDelFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __a3 Lister *lister,
	register __d0 BPTR lock);

void __asm HookLoadFile(
	register __a0 FunctionHandle *handle,
	register __a1 char *path,
	register __a2 char *name,
	register __d0 BPTR lock,
	register __d1 long flags,
	register __d2 BOOL reload);

void __asm HookDoChanges(
	register __a0 FunctionHandle *handle);

BOOL __asm HookCheckAbort(
	register __a0 FunctionHandle *handle);

struct Window *__asm HookGetWindow(
	register __a0 Lister *lister);

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

BOOL __asm HookMatchLockDesktop(
	register __a0 BPTR lock);

BOOL __asm HookMatchDesktop(
	register __a0 char *path);

UWORD __asm HookGetDesktop(
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

Lister * __asm HookNewLister(
	register __a0 STRPTR path,
	register __a1 Lister *parent,
	register __a2 STRPTR toolbar_path,
	register __d0 BPTR path_lock,
	register __d1 ULONG flags,
	register __d2 WORD Left,
	register __d3 WORD Top,
	register __d4 WORD Width,
	register __d5 WORD Height);


void __asm __saveds HookFreePointerDirect(
	register __a0 APTR pointer,
	register __d0 ULONG type,
	register __d1 ULONG flags);

void __asm __saveds HookReadDir(
	register __a0 Lister *lister,
	register __a1 STRPTR path,
	register __d0 ULONG flags,
	register __d1 BPTR path_lock);

void __asm __saveds HookClearDir(
	register __a0 Lister *lister,
	register __d0 ULONG flags);

#endif
