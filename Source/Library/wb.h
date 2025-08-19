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

#ifndef _GALILEOFM_WB
#define _GALILEOFM_WB

typedef struct
{
    short		    patch_count;
    struct SignalSemaphore  patch_lock;
    struct MinList	    app_list;
    APTR		    *old_function;

    UWORD		    lock_count;
    BOOL 		    first_app_entry;

    struct MinList	    rem_app_list;
} WB_Data;


typedef struct
{
    long    offset;
    APTR    function;
    short   type;
} PatchList;


enum
{
    WB_PATCH_ADDAPPWINDOW,
    WB_PATCH_REMAPPWINDOW,
    WB_PATCH_ADDAPPICON,
    WB_PATCH_REMAPPICON,
    WB_PATCH_ADDAPPMENU,
    WB_PATCH_REMAPPMENU,
    WB_PATCH_CLOSEWORKBENCH,
    WB_PATCH_OPENWORKBENCH,
    WB_PATCH_PUTDISKOBJECT,
    WB_PATCH_DELETEDISKOBJECT,
    WB_PATCH_ADDPORT,
    WB_PATCH_CLOSEWINDOW,
    WB_PATCH_CREATEDIR,
    WB_PATCH_DELETEFILE,
    WB_PATCH_SETFILEDATE,
    WB_PATCH_SETCOMMENT,
    WB_PATCH_SETPROTECTION,
    WB_PATCH_RENAME,
    WB_PATCH_OPEN,
    WB_PATCH_CLOSE,
    WB_PATCH_WRITE,
    WB_PATCH_RELABEL,
    WB_PATCH_WBINFO,
    WB_PATCH_ADDTASK,
    WB_PATCH_REMTASK,
    WB_PATCH_FINDTASK,
    WB_PATCH_OPENWINDOWTAGS,
//	WB_PATCH_ALLOCBITMAP,

    WB_PATCH_COUNT
};

enum
{
    WB_PATCH_WORKBENCH,
    WB_PATCH_INTUITION,
    WB_PATCH_ICON,
    WB_PATCH_EXEC,
    WB_PATCH_DOS,
    WB_PATCH_GFX,
    WB_PATCH_DOSFUNC,
    WB_PATCH_DATATYPES,
};


// patch.asm
struct AppWindow *__asm L_WB_AddAppWindowTr(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 struct Window *window,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_WB_RemoveAppWindowTr(
	register __a0 struct AppWindow *window,
	register __a6 struct Library *GalileoFMBase);
struct AppIcon *__asm L_WB_AddAppIconTr(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 BPTR lock,
	register __a3 struct DiskObject *object,
	register __a4 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_WB_RemoveAppIconTr(
	register __a0 struct AppIcon *icon,
	register __a6 struct Library *GalileoFMBase);
struct AppMenuItem *__asm L_WB_AddAppMenuItemTr(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_WB_RemoveAppMenuItemTr(
	register __a0 struct AppMenuItem *item,
	register __a6 struct Library *GalileoFMBase);
LONG __asm L_WB_CloseWorkBenchTr(register __a6 struct Library *GalileoFMBase);
ULONG __asm L_WB_OpenWorkBenchTr(register __a6 struct Library *GalileoFMBase);

BOOL __asm L_WB_PutDiskObjectTr(
	register __a0 char *name,
	register __a1 struct DiskObject *diskobj);
BOOL __asm L_WB_DeleteDiskObjectTr(register __a0 char *name,
	register __a6 struct Library *GalileoFMBase);

void __asm L_WB_AddPortTr(register __a1 struct MsgPort *port,
	register __a6 struct Library *GalileoFMBase);

void __asm L_WB_CloseWindowTr(register __a0 struct Window *window,
	register __a6 struct Library *GalileoFMBase);

BPTR __asm L_PatchedCreateDirTr(register __d1 char *name,
	register __a6 struct Library *GalileoFMBase);
long __asm L_PatchedDeleteFileTr(register __d1 char *name,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_PatchedSetFileDateTr(
	register __d1 char *name,
	register __d2 struct DateStamp *date,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_PatchedSetCommentTr(
	register __d1 char *name,
	register __d2 char *comment,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_PatchedSetProtectionTr(
	register __d1 char *name,
	register __d2 ULONG mask,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_PatchedRenameTr(
	register __d1 char *oldname,
	register __d2 char *newname,
	register __a6 struct Library *GalileoFMBase);
BPTR __asm L_PatchedOpenTr(
	register __d1 char *name,
	register __d2 LONG accessMode);
BOOL __asm L_PatchedCloseTr(register __d1 BPTR file,
	register __a6 struct Library *GalileoFMBase);
LONG __asm L_PatchedWriteTr(
	register __d1 BPTR file,
	register __d2 void *data,
	register __d3 LONG length,
	register __a6 struct Library *GalileoFMBase);
BOOL __asm L_PatchedRelabelTr(
	register __d1 char *volumename,
	register __d2 char *name,
	register __a6 struct Library *GalileoFMBase);

ULONG __asm L_PatchedWBInfoTr(
	register __a0 BPTR lock,
	register __a1 char *name,
	register __a2 struct Screen *screen,
	register __a6 struct Library *GalileoFMBase);

APTR __asm L_PatchedAddTaskTr(register __a1 struct Task *task,register __a2 APTR initialPC,register __a3 APTR finalPC,
	register __a6 struct Library *GalileoFMBase);
void __asm L_PatchedRemTaskTr(register __a1 struct Task *task,
	register __a6 struct Library *GalileoFMBase);
struct Task *__asm L_PatchedFindTaskTr(register __a1 char *name,
	register __a6 struct Library *GalileoFMBase);
struct Window *__asm L_PatchedOpenWindowTagsTr(register __a0 struct NewWindow *,register __a1 struct TagItem *,
	register __a6 struct Library *GalileoFMBase);



// wb.c
void __asm WB_Install_Patch(void);
BOOL __asm WB_Remove_Patch(void);
struct AppWindow *__asm WB_AddAppWindow(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 struct Window *window,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags);
BOOL __asm WB_RemoveAppWindow(
	register __a0 struct AppWindow *window);
struct AppIcon *__asm WB_AddAppIcon(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 BPTR lock,
	register __a3 struct DiskObject *object,
	register __a4 struct TagItem *tags);
BOOL __asm WB_RemoveAppIcon(
	register __a0 struct AppIcon *icon);
struct AppMenuItem *__asm WB_AddAppMenuItem(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags);
BOOL __asm WB_RemoveAppMenuItem(
	register __a0 struct AppMenuItem *item);

struct AppWindow __asm *WB_FindAppWindow(
	register __a0 struct Window *window);
struct MsgPort *__asm WB_AppWindowData(
	register __a0 struct AppWindow *window,
	register __a1 ULONG *id,
	register __a2 ULONG *userdata);
BOOL __asm WB_AppWindowLocal(
	register __a0 struct AppWindow *window);
BOOL __asm WB_AppIconSnapshot(
	register __a0 struct AppIcon *icon);

APTR __asm LockAppList(void);
APTR __asm NextAppEntry(
	register __a0 APTR last,
	register __d0 ULONG type);
void __asm UnlockAppList(void);

AppEntry *new_app_entry(
	ULONG type,
	ULONG id,
	ULONG userdata,
	APTR object,
	char *text,
	struct MsgPort *port,
	WB_Data *wb_data);
APTR rem_app_entry(
	AppEntry *entry,
	WB_Data *wb_data,
	BOOL *);
void free_app_entry(AppEntry *entry,WB_Data *);
AppEntry *find_app_entry(AppEntry *entry,WB_Data *wb_data);

LONG __asm WB_CloseWorkBench(void);
ULONG __asm WB_OpenWorkBench(void);

BOOL __asm WB_PutDiskObject(
	register __a0 char *name,
	register __a1 struct DiskObject *diskobj);

typedef struct
{
    struct DiskObject	    doc_DiskObject;
    struct Image	    doc_GadgetRender;
    struct Image	    doc_SelectRender;
    APTR		    doc_Image1;
    APTR		    doc_Image2;
    APTR		    doc_Data;
} DiskObjectCopy;

struct DiskObject *__asm CopyDiskObject(
	register __a0 struct DiskObject *icon,
	register __d0 ULONG flags);

void __asm FreeDiskObjectCopy(
	register __a0 struct DiskObject *icon);

BOOL __asm WB_DeleteDiskObject(register __a0 char *name);

void __asm ChangeAppIcon(
	register __a0 APTR appicon,
	register __a1 struct Image *render,
	register __a2 struct Image *select,
	register __a3 char *title,
	register __d0 ULONG flags);

long __asm SetAppIconMenuState(
	register __a0 APTR appicon,
	register __d0 long item,
	register __d1 long state);
void __asm WB_AddPort(register __a1 struct MsgPort *port);

void __asm WB_CloseWindow(register __a0 struct Window *window);

void icon_notify(struct LibData *data,char *name,ULONG flags,short);

#define INF_FULLNAME	(1<<0)

struct Library *wb_get_patchbase(short type);

ULONG __asm PatchedWBInfo(
	register __a0 BPTR lock,
	register __a1 char *name,
	register __a2 struct Screen *screen);

APTR __asm PatchedAddTask(register __a1 struct Task *task,register __a2 APTR initialPC,register __a3 APTR finalPC);
void __asm PatchedRemTask(register __a1 struct Task *task);
struct Task *__asm PatchedFindTask(register __a1 char *name);
struct Window *__asm PatchedOpenWindowTags(register __a0 struct NewWindow *,register __a1 struct TagItem *);

struct BitMap *__asm PatchedAllocBitmap(
	register __d0 ULONG,
	register __d1 ULONG,
	register __d2 ULONG,
	register __d3 ULONG,
	register __a0 struct BitMap *,
	register __a6 struct Library *);

#endif
