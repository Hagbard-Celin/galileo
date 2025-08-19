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

#ifndef _GALILEOFM_INTERNALPROTO
#define _GALILEOFM_INTERNALPROTO


// filetypes.c
BOOL __asm __saveds L_MatchFiletype(
	register __a0 MatchHandle *handle,
	register __a1 Cfg_Filetype *type,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_ClearFiletypeCache(register __a6 struct Library *GalileoFMBase);


// drag_routines.c
BOOL __asm __saveds L_DragCustomOk(
	register __a0 struct BitMap *bm,
	register __a6 struct Library *GalileoFMBase);


// devices.c
struct DosList *__saveds __asm L_DeviceFromLock(
	register __a0 BPTR lock,
	register __a1 char *name,
	register __a6 struct Library *GalileoFMBase);

struct DosList *__saveds __asm L_DeviceFromHandler(
	register __a0 struct MsgPort *port,
	register __a1 char *name,
	register __a6 struct Library *GalileoFMBase);

BOOL __saveds __asm L_DevNameFromLock(
	register __d1 BPTR lock,
	register __d2 char *buffer,
	register __d3 long len,
	register __a6 struct Library *GalileoFMBase);


// dates.c
void __asm __saveds L_DateToStrings(register __a0 struct DateStamp *date,
				    register __a1 char *date_buf,
				    register __a2 char *time_buf,
				    register __d0 int flags,
				    register __a6 struct Library *GalileoFMBase);

UWORD __asm __saveds L_GetLocaleSettings(register __d0 UBYTE type, register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SetLocaleFlags(register __d0 UWORD flags,
				     register __d1 UWORD dateformat,
				     register __a6 struct Library *GalileoFMBase);



// ipc.c
BOOL __asm __saveds L_IPC_Launch(
	register __a0 struct ListLock *list,
	register __a1 IPCData **storage,
	register __a2 char *name,
	register __d0 ULONG entry,
	register __d1 ULONG stack,
	register __d2 ULONG data,
	register __a6 struct Library *GalileoFMBase);


// launcher.c
void __asm __saveds L_GetDosListCopy(
	register __a0 struct List *list,
	register __a1 APTR memory,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_FreeDosListCopy(
	register __a0 struct List *list,
	register __a6 struct Library *GalileoFMBase);

// layout_routines.c
struct Window *__asm __saveds L_OpenConfigWindow(
	register __a0 NewConfigWindow *newwindow,
	register __a6 struct Library *GalileoFMBase);

ObjectList *__asm __saveds L_AddObjectList(
	register __a0 struct Window *window,
	register __a1 ObjectDef *objects,
	register __a6 struct Library *GalileoFMBase);


// layout_support.c
long __asm __saveds L_GetGadgetValue(
	register __a0 ObjectList *list,
	register __a1 UWORD id,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_UpdateGadgetValue(
	register __a0 ObjectList *list,
	register __a1 struct IntuiMessage *msg,
	register __d0 UWORD id,
	register __a6 struct Library *GalileoFMBase);

// misc.c
CxObj *__asm __saveds L_GetCxSelectUpDown(register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SetCxSelectUpDown(register __a1 CxObj * cxo,register __a6 struct Library *GalileoFMBase);

ULONG __asm __saveds L_SetLibraryFlags(
	register __d0 ULONG flags,
	register __d1 ULONG mask,
	register __a6 struct Library *GalileoFMBase);

ULONG __asm __saveds L_GetLibraryFlags(
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SetReqBackFill(
	register __a0 struct Hook *hook,
	register __a1 struct Screen **screen,
	register __a6 struct Library *GalileoFMBase);

struct Hook *__asm __saveds L_LockReqBackFill(
	register __a0 struct Screen *screen,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_UnlockReqBackFill(
	register __a6 struct Library *GalileoFMBase);

long __asm L_GetStatistics(register __d0 long id,register __a6 struct Library *GalileoFMBase);


// notify.c
APTR __asm __saveds L_AddNotifyRequest(
	register __d0 ULONG type,
	register __d1 ULONG userdata,
	register __a0 struct MsgPort *port,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_RemoveNotifyRequest(
	register __a0 NotifyNode *node,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SendNotifyMsg(
	register __d0 ULONG type,
	register __d1 ULONG data,
	register __d2 ULONG flags,
	register __d3 short wait,
	register __a0 char *name,
	register __a1 struct FileInfoBlock *fib,
	register __a6 struct Library *GalileoFMBase);

// pathlist.c
BPTR __asm __saveds L_GetGalileoPathList(register __a6 struct Library *GalileoFMBase);


// popup_menus.c
UWORD __asm __saveds L_DoPopUpMenu(
	register __a0 struct Window *window,
	register __a1 PopUpMenu *menu,
	register __a2 PopUpItem **sel_item,
	register __d0 UWORD code,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SetPopUpDelay(register __d0 short delay,register __a6 struct Library *GalileoFMBase);


// progress_win.c

// requesters.c

// semaphores.c
long __asm __saveds L_GetSemaphore(
	register __a0 struct SignalSemaphore *sem,
	register __d0 long exclusive,
	register __a1 char *name,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_FreeSemaphore(
	register __a0 struct SignalSemaphore *sem,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_ShowSemaphore(
	register __a0 struct SignalSemaphore *sem,
	register __a6 struct Library *GalileoFMBase);


// timer.c
struct Library *__asm __saveds L_GetTimerBase(register __a6 struct Library *GalileoFMBase);


BOOL __asm __saveds L_WB_Remove_Patch(register __a6 struct Library *GalileoFMBase);

struct AppWindow *__asm L_WB_AddAppWindow(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 struct Window *window,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_WB_RemoveAppWindow(
	register __a0 struct AppWindow *window,
	register __a6 struct Library *GalileoFMBase);

struct AppIcon *__asm L_WB_AddAppIcon(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 BPTR lock,
	register __a3 struct DiskObject *icon,
	register __a4 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_WB_RemoveAppIcon(
	register __a0 struct AppIcon *icon,
	register __a6 struct Library *GalileoFMBase);

struct AppMenuItem *__asm L_WB_AddAppMenuItem(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_WB_RemoveAppMenuItem(
	register __a0 struct AppMenuItem *item,
	register __a6 struct Library *GalileoFMBase);

LONG __asm L_WB_CloseWorkBench(register __a6 struct Library *GalileoFMBase);

ULONG __asm L_WB_OpenWorkBench(register __a6 struct Library *GalileoFMBase);

void __asm L_WB_AddPort(register __a1 struct MsgPort *port, register __a6 struct Library *GalileoFMBase);

void __asm L_WB_CloseWindow(register __a0 struct Window *window, register __a6 struct Library *GalileoFMBase);

ULONG __asm L_WB_OpenWorkBench(register __a6 struct Library *GalileoFMBase);

BOOL __asm __saveds L_WB_PutDiskObject(
	register __a0 char *name,
	register __a1 struct DiskObject *diskobj,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm __saveds L_WB_DeleteDiskObject(register __a0 char *name, register __a6 struct Library *GalileoFMBase);

void __asm L_WB_AddPort(register __a1 struct MsgPort *port, register __a6 struct Library *GalileoFMBase);

void __asm L_WB_CloseWindow(register __a0 struct Window *window, register __a6 struct Library *GalileoFMBase);

BPTR __asm L_PatchedCreateDir(register __d1 char *name, register __a6 struct Library *GalileoFMBase);

long __asm L_PatchedDeleteFile(register __d1 char *name, register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedSetFileDate(
	register __d1 char *name,
	register __d2 struct DateStamp *date,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedSetComment(
	register __d1 char *name,
	register __d2 char *comment,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedSetProtection(
	register __d1 char *name,
	register __d2 ULONG mask,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedRename(
	register __d1 char *oldname,
	register __d2 char *newname,
	register __a6 struct Library *GalileoFMBase);

BPTR __asm L_PatchedOpen(
	register __d1 char *name,
	register __d2 LONG accessMode,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedClose(register __d1 BPTR file, register __a6 struct Library *GalileoFMBase);

LONG __asm L_PatchedWrite(
	register __d1 BPTR file,
	register __d2 void *wdata,
	register __d3 LONG length,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm L_PatchedRelabel(
	register __d1 char *volumename,
	register __d2 char *name,
	register __a6 struct Library *GalileoFMBase);

ULONG __asm L_PatchedWBInfo(
	register __a0 BPTR lock,
	register __a1 char *name,
	register __a2 struct Screen *screen,
	register __a6 struct Library *GalileoFMBase);

APTR __asm L_PatchedAddTask(register __a1 struct Task *task,register __a2 APTR initialPC,register __a3 APTR finalPC, register __a6 struct Library *GalileoFMBase);

void __asm L_PatchedRemTask(register __a1 struct Task *task, register __a6 struct Library *GalileoFMBase);

struct Task *__asm L_PatchedFindTask(register __a1 char *name, register __a6 struct Library *GalileoFMBase);

struct Window *__asm L_PatchedOpenWindowTags(register __a0 struct NewWindow *newwin,register __a1 struct TagItem *tags, register __a6 struct Library *GalileoFMBase);

// icon_cache.c
struct DiskObject *__asm __saveds L_GetCachedDefDiskObject(
	register __d0 long type,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_FreeCachedDiskObject(
	register __a0 struct DiskObject *icon,
	register __a6 struct Library *GalileoFMBase);

struct DiskObject *__asm __saveds L_GetCachedDiskObject(
	register __a0 char *name,
	register __d0 ULONG flags,
	register __a6 struct Library *GalileoFMBase);

struct DiskObject *__asm __saveds L_GetCachedDiskObjectNew(
	register __a0 char *name,
	register __d0 ULONG flags,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_SetNewIconsFlags(
	register __d0 ULONG flags,
	register __d1 short precision,
	register __a6 struct Library *GalileoFMBase);

struct DiskObject *__asm __saveds L_CopyDiskObject(
	register __a0 struct DiskObject *icon,
	register __d0 ULONG flags,
	register __a6 struct Library *GalileoFMBase);

void __asm __saveds L_FreeDiskObjectCopy(
	register __a0 struct DiskObject *icon,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm __saveds L_WriteIcon(
	register __a0 char *name,
	register __a1 struct DiskObject *icon,
	register __a6 struct Library *GalileoFMBase);

BOOL __asm __saveds L_DeleteIcon(
	register __a0 char *name,
	register __a6 struct Library *GalileoFMBase);

#endif

