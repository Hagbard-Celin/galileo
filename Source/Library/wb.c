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

/*
	04 Nov 99  Changed diskobjectcopy to use DupDiskObject for V44


*/

#include "galileofmlib.h"
#include "/Modules/modules_internal_protos.h"
#include <proto/newicon.h>


// Patch functions
static const PatchList
	wb_patches[WB_PATCH_COUNT]={
		{-0x30,L_WB_AddAppWindowTr,WB_PATCH_WORKBENCH},
		{-0x36,L_WB_RemoveAppWindowTr,WB_PATCH_WORKBENCH},
		{-0x3c,L_WB_AddAppIconTr,WB_PATCH_WORKBENCH},
		{-0x42,L_WB_RemoveAppIconTr,WB_PATCH_WORKBENCH},
		{-0x48,L_WB_AddAppMenuItemTr,WB_PATCH_WORKBENCH},
		{-0x4e,L_WB_RemoveAppMenuItemTr,WB_PATCH_WORKBENCH},
		{-0x4e,L_WB_CloseWorkBenchTr,WB_PATCH_INTUITION},
		{-0xd2,L_WB_OpenWorkBenchTr,WB_PATCH_INTUITION},
		{-0x54,L_WB_PutDiskObjectTr,WB_PATCH_ICON},
		{-0x8a,L_WB_DeleteDiskObjectTr,WB_PATCH_ICON},
		{-0x162,L_WB_AddPortTr,WB_PATCH_EXEC},
		{-0x48,L_WB_CloseWindowTr,WB_PATCH_INTUITION},
		{-0x78,L_PatchedCreateDirTr,WB_PATCH_DOSFUNC},
		{-0x48,L_PatchedDeleteFileTr,WB_PATCH_DOSFUNC},
		{-0x18c,L_PatchedSetFileDateTr,WB_PATCH_DOSFUNC},
		{-0xb4,L_PatchedSetCommentTr,WB_PATCH_DOSFUNC},
		{-0xba,L_PatchedSetProtectionTr,WB_PATCH_DOSFUNC},
		{-0x4e,L_PatchedRenameTr,WB_PATCH_DOSFUNC},
		{-0x1e,L_PatchedOpenTr,WB_PATCH_DOSFUNC},
		{-0x24,L_PatchedCloseTr,WB_PATCH_DOSFUNC},
		{-0x30,L_PatchedWriteTr,WB_PATCH_DOSFUNC},
		{-0x2d0,L_PatchedRelabelTr,WB_PATCH_DOS},
		{-0x5a,L_PatchedWBInfoTr,WB_PATCH_WORKBENCH},
		{-0x11a,L_PatchedAddTaskTr,WB_PATCH_EXEC},
		{-0x120,L_PatchedRemTaskTr,WB_PATCH_EXEC},
		{-0x126,L_PatchedFindTaskTr,WB_PATCH_EXEC},
		{-0x25e,L_PatchedOpenWindowTagsTr,WB_PATCH_INTUITION},
/*
		{-0x396,L_PatchedAllocBitmap,WB_PATCH_GFX},
*/
		};

// Install workbench emulation patches
void __asm __saveds L_WB_Install_Patch(register __a6 struct Library *GalileoFMBase)
{
	// Lock patch information
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_EXCLUSIVE,0, GalileoFMBase);

	// Are patches not installed?
	if (gfmlib_data.wb_data.patch_count==0)
	{
		short patch;

		// Allocate patch table
		if (gfmlib_data.wb_data.old_function=AllocVec(sizeof(APTR)*WB_PATCH_COUNT,MEMF_CLEAR))
		{
			// Forbid while we install the patches
			Forbid();

			// Install patches
			for (patch=0;patch<WB_PATCH_COUNT;patch++)
			{
				struct Library *lib;

				// Get library
				if (lib=wb_get_patchbase(wb_patches[patch].type))
				{
					// Patch this function
					gfmlib_data.wb_data.old_function[patch]=
						SetFunction(lib,wb_patches[patch].offset,wb_patches[patch].function);
				}
			}

			// Clear the cache and Permit
			CacheClearU();
			Permit();

			// Increment patch count
			++gfmlib_data.wb_data.patch_count;
		}
	}

	// Unlock patch information
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);
}


// Remove workbench patches
BOOL __asm __saveds L_WB_Remove_Patch(register __a6 struct Library *GalileoFMBase)
{
	short patch;
	BOOL fail=0;
	APTR old_patch_val[WB_PATCH_COUNT];

	// Lock patch information
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_EXCLUSIVE,0, GalileoFMBase);

	// Really remove patches?
	if (gfmlib_data.wb_data.patch_count==1)
	{
		// Try to remove patches
		Forbid();
		for (patch=0;patch<WB_PATCH_COUNT;patch++)
		{
			// Patch installed for this function?
			if (gfmlib_data.wb_data.old_function[patch])
			{
				struct Library *lib;

				// Get library
				if (lib=wb_get_patchbase(wb_patches[patch].type))
				{
					// Restore old value
					old_patch_val[patch]=
						SetFunction(lib,wb_patches[patch].offset,gfmlib_data.wb_data.old_function[patch]);

					// The value returned should have been our patch
					if (old_patch_val[patch]!=wb_patches[patch].function)
					{
						// Something's in there
						fail=1;
						break;
					}
				}
			}
		}

		// Did remove fail?
		if (fail)
		{
			// Restore the vectors that we've changed
			for (;patch>=0;patch--)
			{
				// Patch installed for this function?
				if (gfmlib_data.wb_data.old_function[patch])
				{
					struct Library *lib;

					// Get library
					if (lib=wb_get_patchbase(wb_patches[patch].type))
					{
						// Reinstall patch
						SetFunction(lib,wb_patches[patch].offset,gfmlib_data.wb_data.old_function[patch]);
					}
				}
			}
		}

		// Otherwise, clear our patch pointers
		else
		{
			FreeVec(gfmlib_data.wb_data.old_function);
			gfmlib_data.wb_data.old_function=0;
		}

		CacheClearU();
		Permit();
	}

	// If we didn't fail, decrement patch count
	if (!fail && gfmlib_data.wb_data.patch_count>0)
		--gfmlib_data.wb_data.patch_count;

	// Unlock patch information
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);

	return (BOOL)(!fail);
}


// AddAppWindow()
struct AppWindow *__asm __saveds L_WB_AddAppWindow(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 struct Window *window,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *app_entry;

	// Check valid data
	if (!window || !port) return 0;

	// Get new AppEntry
	if (app_entry=new_app_entry(APP_WINDOW,id,userdata,window,0,port,&gfmlib_data.wb_data))
	{
		// Local AppThing?
		if (tags && GetTagData(GAE_Local,0,tags))
		{
			// Set local flag
			app_entry->ae_flags|=APPENTF_LOCAL;
		}

		// Otherwise pass to OS
		else
		{
			app_entry->ae_os_object=
				((struct AppWindow * (* __asm)
					(register __d0 ULONG,
					register __d1 ULONG,
					register __a0 struct Window *,
					register __a1 struct MsgPort *,
					register __a2 struct TagItem *,
					register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_ADDAPPWINDOW])
					(id,userdata,window,port,tags,WorkbenchBase);
		}

		// Send notification
		L_SendNotifyMsg(GN_APP_WINDOW_LIST,(ULONG)app_entry,0,FALSE,0,0,GalileoFMBase);
	}

	// Return object
	return (struct AppWindow *)app_entry;
}


// AddAppIcon()
struct AppIcon *__asm __saveds L_WB_AddAppIcon(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 BPTR lock,
	register __a3 struct DiskObject *icon,
	register __a4 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *app_entry=0;
	BOOL osonly=0;
	APTR object=0;
	short local=2;

	// Check valid data
	if (!icon || !port) return 0;

	// Pass straight through to OS?
	if (tags && (local=GetTagData(GAE_Local,2,tags))==0) osonly=1;

	// Otherwise, if it's not a local icon and the redirection flag is set, send to Tools menu
	else
	if (local==2 && gfmlib_data.flags&LIBDF_REDIRECT_TOOLS)
	{
		// Add as menu item
		return (struct AppIcon *)L_WB_AddAppMenuItem(
			id,
			userdata,
			text,
			port,
			tags,GalileoFMBase);
	}

	// Get new AppEntry
	if (osonly || (app_entry=new_app_entry(APP_ICON,id,userdata,icon,text,port,&gfmlib_data.wb_data)))
	{
		short a;
		struct TagItem *tag;

		// Any tags?
		if (tags && app_entry)
		{
			struct TagItem *tstate;

			// Support snapshot?
			if (GetTagData(GAE_SnapShot,0,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_SNAPSHOT;
			}

			// Support info?
			if (GetTagData(GAE_Info,0,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_INFO;
			}

			// Locked?
			if (GetTagData(GAE_Locked,0,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_LOCKED;
			}

			// Close item?
			if (a=GetTagData(GAE_Close,0,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_CLOSE;

				// No open?
				if (a==2) app_entry->ae_flags|=APPENTF_NO_OPEN;
			}

			// Background colour?
			if (tag=FindTagItem(GAE_Background,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_BACKGROUND;

				// Store colour;
				app_entry->ae_data=tag->ti_Data;
			}

			// Special?
			if (GetTagData(GAE_Special,0,tags))
			{
				// Set flag
				app_entry->ae_flags|=APPENTF_SPECIAL;
			}

			// Set menu base
			app_entry->ae_menu_id_base=GetTagData(GAE_MenuBase,0,tags);

			// Go through tags
			tstate=tags;
			while (tag=NextTagItem(&tstate))
			{
				// Menu item?
				if ((tag->ti_Tag==GAE_Menu ||
					 tag->ti_Tag==GAE_ToggleMenu ||
					 tag->ti_Tag==GAE_ToggleMenuSel) && tag->ti_Data)
				{
					struct Node *node;

					// Allocate space for node and name
					if (node=AllocVec(sizeof(struct Node)+strlen((char *)tag->ti_Data)+1,MEMF_CLEAR))
					{
						// Copy name
						node->ln_Name=(char *)(node+1);
						strcpy(node->ln_Name,(char *)tag->ti_Data);

						// Set flags
						if (tag->ti_Tag==GAE_ToggleMenu) node->ln_MenuFlags=MNF_TOGGLE;
						else
						if (tag->ti_Tag==GAE_ToggleMenuSel) node->ln_MenuFlags=MNF_TOGGLE|MNF_SEL;

						// Add to list
						AddTail((struct List *)&app_entry->ae_menu,(struct Node *)node);
					}
				}
			}
		}

		// Local AppThing?
		if (local==1 && !osonly)
		{
			// Set local flag
			app_entry->ae_flags|=APPENTF_LOCAL;
		}

		// Otherwise, pass to OS
		else
		{
			object=
				((APTR (* __asm)
					(register __d0 ULONG,
					register __d1 ULONG,
					register __a0 char *,
					register __a1 struct MsgPort *,
					register __a2 BPTR,
					register __a3 struct DiskObject *,
					register __a4 struct TagItem *,
					register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_ADDAPPICON])
					(id,userdata,text,port,lock,icon,tags,WorkbenchBase);
			if (app_entry) app_entry->ae_os_object=object;
		}

		// Send notification
		if (app_entry)
			L_SendNotifyMsg(GN_APP_ICON_LIST,(ULONG)app_entry,0,FALSE,0,0,GalileoFMBase);
	}

	// Return object
	return (struct AppIcon *)((app_entry)?(APTR)app_entry:object);
}


// AddAppMenuItem()
struct AppMenuItem *__asm __saveds L_WB_AddAppMenuItem(
	register __d0 ULONG id,
	register __d1 ULONG userdata,
	register __a0 char *text,
	register __a1 struct MsgPort *port,
	register __a2 struct TagItem *tags,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *app_entry=0;
	BOOL osonly=0;
	APTR object=0;

	// Check valid data
	if (!port) return 0;

	// Pass straight through?
	if (tags && !(GetTagData(GAE_Local,1,tags))) osonly=1;

	// Get new AppEntry
	if (osonly || (app_entry=new_app_entry(APP_MENU,id,userdata,0,text,port,&gfmlib_data.wb_data)))
	{
		// Local AppThing?
		if (tags && GetTagData(GAE_Local,0,tags))
		{
			// Set local flag
			app_entry->ae_flags|=APPENTF_LOCAL;
		}

		// Otherwise pass to OS
		else
		{
			object=
				((APTR (* __asm)
					(register __d0 ULONG,
					register __d1 ULONG,
					register __a0 char *,
					register __a1 struct MsgPort *,
					register __a2 struct TagItem *,
					register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_ADDAPPMENU])
					(id,userdata,text,port,tags,WorkbenchBase);
			if (app_entry) app_entry->ae_os_object=object;
		}

		// Send notification
		if (app_entry)
		{
			struct Task *thistask;
			short toolmanger=0;

			// Get current task
			thistask=FindTask(0);

			// Is this from ToolManager?
			if (thistask->tc_Node.ln_Name &&
				strcmp(thistask->tc_Node.ln_Name,"ToolManager Handler")==0)
				toolmanger=1;

			// Send notify message
			L_SendNotifyMsg(GN_APP_MENU_LIST,(ULONG)app_entry,toolmanger,FALSE,0,0,GalileoFMBase);
		}
	}

	// Return object
	return (struct AppMenuItem *)((app_entry)?(APTR)app_entry:object);
}


// RemoveAppWindow()
BOOL __asm __saveds L_WB_RemoveAppWindow(
	register __a0 struct AppWindow *window,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *entry;
	APTR os_object;

	// Valid pointer?
	if (!(entry=(AppEntry *)window)) return 1;

	// Free entry
	os_object=rem_app_entry(entry,&gfmlib_data.wb_data,0);

	// Workbench object?
	if (os_object)
	{
		// Remove workbench object
		((BOOL (* __asm)
			(register __a0 struct AppWindow *,
			register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_REMAPPWINDOW])
			(os_object,WorkbenchBase);
	}

	// Send notification
	L_SendNotifyMsg(GN_APP_WINDOW_LIST,(ULONG)window,GNF_WINDOW_REMOVED,FALSE,0,0,GalileoFMBase);
	return 1;
}


// RemoveAppIcon()
BOOL __asm __saveds L_WB_RemoveAppIcon(
	register __a0 struct AppIcon *icon,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *entry;
	APTR os_object;
	BOOL local;

	// Valid pointer?
	if (!(entry=(AppEntry *)icon)) return 1;

	// Check valid object
	if (find_app_entry(entry,&gfmlib_data.wb_data))
	{
		// Is it a menu (must have been redirected)?
		if (entry->ae_type==APP_MENU)
		{
			// Unlock list
			L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);

			// Remove as menu item
			return L_WB_RemoveAppMenuItem((struct AppMenuItem *)icon,GalileoFMBase);
		}

		// Send notification
		L_SendNotifyMsg(GN_APP_ICON_LIST,(ULONG)entry,GNF_ICON_REMOVED,FALSE,0,0,GalileoFMBase);

//*********************************************************************TRUE ???

		// Unlock list
		L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);

	}

	// Free entry
	if (os_object=rem_app_entry(entry,&gfmlib_data.wb_data,&local))
	{
		// Remove workbench object
		((BOOL (* __asm)
			(register __a0 struct AppIcon *,
			register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_REMAPPICON])
			(os_object,WorkbenchBase);
	}

	return 1;
}


// RemoveAppMenuItem()
BOOL __asm __saveds L_WB_RemoveAppMenuItem(
	register __a0 struct AppMenuItem *item,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *entry;
	APTR os_object;
	BOOL local;

	// Valid pointer?
	if (!(entry=(AppEntry *)item)) return 1;

	// Free entry
	os_object=rem_app_entry(entry,&gfmlib_data.wb_data,&local);

	// Workbench object?
	if (os_object)
	{
		// Remove workbench object
		((BOOL (* __asm)
			(register __a0 struct AppMenuItem *,
			register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_REMAPPMENU])
			(os_object,WorkbenchBase);
	}

	// Send notification
	if (os_object!=(APTR)entry)
		L_SendNotifyMsg(GN_APP_MENU_LIST,(ULONG)entry,1,!local,0,0,GalileoFMBase);

	return 1;
}


// Find AppWindow
struct AppWindow __asm __saveds *L_WB_FindAppWindow(
	register __a0 struct Window *window,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *app_entry,*app_window=0;

	// Lock list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_SHARED,0, GalileoFMBase);

	// Go through app objects
	for (app_entry=(AppEntry *)gfmlib_data.wb_data.app_list.mlh_Head;
		app_entry->ae_node.mln_Succ;
		app_entry=(AppEntry *)app_entry->ae_node.mln_Succ)
	{
		// Is this the window?
		if (app_entry->ae_type==APP_WINDOW &&
			app_entry->ae_object==window)
		{
			// Forbid so window can't go
			Forbid();

			// Save pointer
			app_window=app_entry;
			break;
		}
	}

	// Unlock list
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);
	return (struct AppWindow *)app_window;
}


// Get AppWindow data
struct MsgPort *__asm __saveds L_WB_AppWindowData(
	register __a0 struct AppWindow *window,
	register __a1 ULONG *id,
	register __a2 ULONG *userdata,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *entry;

	// Valid entry?
	if (entry=(AppEntry *)window)
	{
		if (id) *id=entry->ae_id;
		if (userdata) *userdata=entry->ae_userdata;
		return entry->ae_port;
	}
	return 0;
}


// Is an AppWindow "local"?
BOOL __asm __saveds L_WB_AppWindowLocal(
	register __a0 struct AppWindow *window,
	register __a6 struct Library *GalileoFMBase)
{
	return (BOOL)((((AppEntry *)window)->ae_flags&APPENTF_LOCAL)?1:0);
}


// Get AppWindow window pointer
struct Window *__asm __saveds L_WB_AppWindowWindow(
	register __a0 struct AppWindow *window,
	register __a6 struct Library *GalileoFMBase)
{
	return (struct Window *)(((AppEntry *)window)->ae_object);
}


// Get AppIcon flags
unsigned long __asm __saveds L_WB_AppIconFlags(
	register __a0 struct AppIcon *icon,
	register __a6 struct Library *GalileoFMBase)
{
	return ((AppEntry *)icon)->ae_flags;
}


// Add a new AppEntry
AppEntry *new_app_entry(
	ULONG type,
	ULONG id,
	ULONG userdata,
	APTR object,
	char *text,
	struct MsgPort *port,
	WB_Data *wb_data)
{
	AppEntry *entry;


	// Allocate new entry
	if (!(entry=AllocVec(sizeof(AppEntry),MEMF_CLEAR)))
		return 0;

	// Fill out AppEntry
	entry->ae_type=type;
	entry->ae_id=id;
	entry->ae_userdata=userdata;
	entry->ae_text=text;
	entry->ae_port=port;
	NewList((struct List *)&entry->ae_menu);

	// AppIcon?
	if (type==APP_ICON)
	{
		struct DiskObject *icon_copy;

		// Copy icon
		if (icon_copy=L_CopyDiskObject((struct DiskObject *)object,0, getreg(REG_A6)))
		{
			// Use copy
			entry->ae_object=icon_copy;
			entry->ae_flags|=APPENTF_ICON_COPY;
		}

		// Couldn't copy
		else
		{
			FreeVec(entry);
			return 0;
		}
	}

	// Otherwise, save object pointer
	else entry->ae_object=object;

#ifdef RESOURCE_TRACKING
	KPrintF("!!!!!!wb.c line: %ld ADDING app_entry \n", __LINE__);
#endif
	// Lock patch list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_EXCLUSIVE,0, getreg(REG_A6));

	// Is this the first entry?
	if ((IsListEmpty((struct List *)&gfmlib_data.wb_data.app_list))&&(gfmlib_data.wb_data.first_app_entry))
	{
#ifdef RESOURCE_TRACKING
	    KPrintF("!!!!!!wb.c line: %ld INCREASING before %ld \n", __LINE__, ((struct Library *)getreg(REG_A6))->lib_OpenCnt);
#endif
	    // Set flag to avoid runaway opencount
	    gfmlib_data.wb_data.first_app_entry=FALSE;

	    // Bump library open count so we won't get expunged
	    ++((struct Library *)getreg(REG_A6))->lib_OpenCnt;

#ifdef RESOURCE_TRACKING
	    KPrintF("!!!!!!wb.c line: %ld INCREASING after %ld \n", __LINE__, ((struct Library *)getreg(REG_A6))->lib_OpenCnt);
#endif
	}

#ifdef RESOURCE_TRACKING
	if (IsListEmpty((struct List *)&gfmlib_data.wb_data.app_list))
	    KPrintF("!!!!!!wb.c  Claims List empty at %lx \n", &gfmlib_data.wb_data.app_list);
	else
	    KPrintF("!!!!!!wb.c  List NOT empty at %lx \n", &gfmlib_data.wb_data.app_list);
#endif

	// Add to list
	AddTail((struct List *)&gfmlib_data.wb_data.app_list,(struct Node *)entry);

	// Unlock list
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, getreg(REG_A6));
	return entry;
}


// Remove an AppEntry
APTR rem_app_entry(
	AppEntry *entry,
	WB_Data *wb_data,
	BOOL *local)
{
	AppEntry *app_entry;
	APTR retval;

	// Save pointer
	retval=entry;

#ifdef RESOURCE_TRACKING
	KPrintF("!!!!!!wb.c line: %ld REMOVING app_entry \n",__LINE__);
#endif

	// Clear local flag
	if (local) *local=0;

	// Lock AppEntry list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_EXCLUSIVE,0, getreg(REG_A6));

	// Look for entry in list
	for (app_entry=(AppEntry *)gfmlib_data.wb_data.app_list.mlh_Head;
		app_entry!=entry && app_entry->ae_node.mln_Succ;
		app_entry=(AppEntry *)app_entry->ae_node.mln_Succ);

	// Found match?
	if (app_entry==entry)
	{
		// Remove from list
		Remove((struct Node *)entry);

		// Return OS object pointer
		retval=entry->ae_os_object;

		// Local?
		if (local && entry->ae_flags&APPENTF_LOCAL) *local=1;

		// Add to removal list
		AddTail((struct List *)&gfmlib_data.wb_data.rem_app_list,(struct Node *)entry);

		// Zero the count
		entry->ae_menu_id_base=0;
	}

	// Unlock list
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, getreg(REG_A6));

	// Return pointer to OS object
	return retval;
}


// Free an entry from the removal list
void free_app_entry(AppEntry *entry,WB_Data *wb_data)
{
	struct MinNode *node;

	// Remove entry from list
	Remove((struct Node *)entry);

	// Free menu nodes
	for (node=entry->ae_menu.mlh_Head;node->mln_Succ;)
	{
		// Cache next pointer
		struct MinNode *next=node->mln_Succ;

		// Free node
		FreeVec(node);

		// Get next
		node=next;
	}

	// Free icon
	if (entry->ae_flags&APPENTF_ICON_COPY)
		L_FreeDiskObjectCopy((struct DiskObject *)entry->ae_object,getreg(REG_A6));

	// Free data structure
	FreeVec(entry);
}


// Find an AppEntry
AppEntry *find_app_entry(AppEntry *entry,WB_Data *wb_data)
{
	AppEntry *app_entry;

	// Lock AppEntry list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_SHARED,0, getreg(REG_A6));

	// Look for entry in list
	for (app_entry=(AppEntry *)gfmlib_data.wb_data.app_list.mlh_Head;
		app_entry!=entry && app_entry->ae_node.mln_Succ;
		app_entry=(AppEntry *)app_entry->ae_node.mln_Succ);

	// Found match?
	if (app_entry==entry) return entry;

	// Unlock list
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, getreg(REG_A6));

	// Not found
	return 0;
}


// Lock the AppList
APTR __asm __saveds L_LockAppList(register __a6 struct Library *GalileoFMBase)
{
	// Get a lock on the list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_SHARED,0, GalileoFMBase);

	// Increment lock count
	++gfmlib_data.wb_data.lock_count;

	// Return indicates start of list
	return &gfmlib_data.wb_data;
}


// Get the next app entry of a certain type
APTR __asm __saveds L_NextAppEntry(
	register __a0 APTR last,
	register __d0 ULONG type,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *entry;

	// Invalid "last" pointer?
	if (!last) return 0;

	// Start of list?
	if (last==&gfmlib_data.wb_data) entry=(AppEntry *)gfmlib_data.wb_data.app_list.mlh_Head;

	// Otherwise, go from next entry
	else entry=(AppEntry *)((AppEntry *)last)->ae_node.mln_Succ;

	// Scan list
	while (entry->ae_node.mln_Succ)
	{
		// Correct type?
		if (entry->ae_type==type) return entry;

		// Get next entry
		entry=(AppEntry *)entry->ae_node.mln_Succ;
	}

	// Not found
	return 0;
}


// Unlock the AppList
void __asm __saveds L_UnlockAppList(register __a6 struct Library *GalileoFMBase)
{
	// Check it's locked
	if (gfmlib_data.wb_data.lock_count>0)
	{
		// Unlock the list
		L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);

		// Decrement lock count
		--gfmlib_data.wb_data.lock_count;
	}
}


// Close Workbench screen
LONG __asm __saveds L_WB_CloseWorkBench(register __a6 struct Library *GalileoFMBase)
{
	LONG result;

	// Send notification
	L_SendNotifyMsg(GN_CLOSE_WORKBENCH,0,0,FALSE,0,0,GalileoFMBase);

	// Close workbench screen
	result=
		((LONG (* __asm)
			(register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_CLOSEWORKBENCH])
			(IntuitionBase);

	return result;
}


// Open Workbench screen
ULONG __asm __saveds L_WB_OpenWorkBench(register __a6 struct Library *GalileoFMBase)
{
	ULONG result;

	// Send notification
	L_SendNotifyMsg(GN_OPEN_WORKBENCH,0,0,FALSE,0,0,GalileoFMBase);

	// Open workbench screen
	result=
		((ULONG (* __asm)
			(register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_OPENWORKBENCH])
			(IntuitionBase);

	return result;
}


// Change an AppIcon's image
void __asm __saveds L_ChangeAppIcon(
	register __a0 APTR appicon,
	register __a1 struct Image *render,
	register __a2 struct Image *select,
	register __a3 char *title,
	register __d0 ULONG flags,
	register __a6 struct Library *GalileoFMBase)
{
	AppEntry *app_entry;

	// Lock AppEntry list
	L_GetSemaphore(&gfmlib_data.wb_data.patch_lock,SEMF_SHARED,0, GalileoFMBase);

	// Look for entry in list
	for (app_entry=(AppEntry *)gfmlib_data.wb_data.app_list.mlh_Head;
		app_entry!=(AppEntry *)appicon && app_entry->ae_node.mln_Succ;
		app_entry=(AppEntry *)app_entry->ae_node.mln_Succ);

	// Found it?
	if (app_entry==(AppEntry *)appicon)
	{
		struct DiskObject *icon;

		// Get icon pointer
		if (icon=(struct DiskObject *)app_entry->ae_object)
		{
			ULONG notify_flags=GNF_ICON_CHANGED;

			// Set image and title pointers
			if (flags&CAIF_RENDER)
			{
				icon->do_Gadget.GadgetRender=render;
				notify_flags|=GNF_ICON_IMAGE_CHANGED;
			}				
			if (flags&CAIF_SELECT)
			{
				icon->do_Gadget.SelectRender=select;
				notify_flags|=GNF_ICON_IMAGE_CHANGED;
			}
			if (flags&CAIF_TITLE) app_entry->ae_text=title;

			// Change locked state
			if (flags&CAIF_LOCKED)
			{
				// Set?
				if (flags&CAIF_SET) app_entry->ae_flags|=APPENTF_LOCKED;

				// Clear
				else app_entry->ae_flags&=~APPENTF_LOCKED;
			}

			// Change ghosted state
			if (flags&CAIF_GHOST)
			{
				// Set?
				if (flags&CAIF_SET) app_entry->ae_flags|=APPENTF_GHOSTED;

				// Clear
				else app_entry->ae_flags&=~APPENTF_GHOSTED;
			}

			// Change busy state?
			if (flags&CAIF_BUSY) app_entry->ae_flags|=APPENTF_BUSY;
			else
			if (flags&CAIF_UNBUSY) app_entry->ae_flags&=~APPENTF_BUSY;

			// Send notification
			L_SendNotifyMsg(GN_APP_ICON_LIST,(ULONG)app_entry,notify_flags,FALSE,0,0,GalileoFMBase);
		}
	}

	// Unlock list
	L_FreeSemaphore(&gfmlib_data.wb_data.patch_lock, GalileoFMBase);
}


// Set the state of a toggle appicon menu
long __asm __saveds L_SetAppIconMenuState(
	register __a0 APTR appicon,
	register __d0 long item,
	register __d1 long state)
{
	long res=-1,num;
	struct Node *node;
	AppEntry *app_entry=(AppEntry *)appicon;

	// Go through menus
	for (node=(struct Node *)app_entry->ae_menu.mlh_Head,num=0;
		node->ln_Succ && num<item;
		node=node->ln_Succ,num++);

	// Got item?
	if (node->ln_Succ)
	{
		// Get old value
		res=(node->ln_MenuFlags&MNF_SEL)?1:0;

		// Set new value
		if (state) node->ln_MenuFlags|=MNF_SEL;
		else node->ln_MenuFlags&=~MNF_SEL;
	}

	return res;
}


// Copy a DiskObject
struct DiskObject *__asm __saveds L_CopyDiskObject(
	register __a0 struct DiskObject *icon,
	register __d0 ULONG flags,
	register __a6 struct Library *GalileoFMBase)
{
	DiskObjectCopy *copy;
	long planesize;

	// Valid icon?
	if (!icon) return 0;

	if	(IconBase->lib_Version>=44)
	{	
		struct DiskObject * icon_copy;

		icon_copy=DupDiskObject(icon,TAG_DONE);

		return icon_copy;

	}


	// Allocate copy structure
	if (!(copy=L_AllocMemH(gfmlib_data.memory,sizeof(DiskObjectCopy))))
		return 0;

	// Copy DiskObject structure
	CopyMem((char *)icon,(char *)&copy->doc_DiskObject,sizeof(struct DiskObject));

	// Null-out pointers
	copy->doc_DiskObject.do_DefaultTool=0;
	copy->doc_DiskObject.do_ToolTypes=0;
	copy->doc_DiskObject.do_DrawerData=0;
	copy->doc_DiskObject.do_ToolWindow=0;

	// Copying things?
	if (flags&DOCF_COPYALL)
	{
		// Copy default tool
		if (icon->do_DefaultTool &&
			(copy->doc_DiskObject.do_DefaultTool=L_AllocMemH(gfmlib_data.memory,strlen(icon->do_DefaultTool)+1)))
			strcpy(copy->doc_DiskObject.do_DefaultTool,icon->do_DefaultTool);

		// Any tooltypes?
		if (icon->do_ToolTypes)
		{
			short count;

			// Count tooltypes
			for (count=0;icon->do_ToolTypes[count];count++);

			// Allocate array
			if (copy->doc_DiskObject.do_ToolTypes=L_AllocMemH(gfmlib_data.memory,(count+1)<<2))
			{
				// Copy tooltypes
				for (count=0;icon->do_ToolTypes[count];count++)
				{
					// Copy string
					if (copy->doc_DiskObject.do_ToolTypes[count]=
						L_AllocMemH(gfmlib_data.memory,strlen(icon->do_ToolTypes[count])+1))
						strcpy(copy->doc_DiskObject.do_ToolTypes[count],icon->do_ToolTypes[count]);
					else break;
				}
			}
		}

		// Drawer data?
		if (icon->do_DrawerData &&
			(copy->doc_DiskObject.do_DrawerData=L_AllocMemH(gfmlib_data.memory,sizeof(struct DrawerData))))
			CopyMem(
				(char *)icon->do_DrawerData,
				(char *)copy->doc_DiskObject.do_DrawerData,
				sizeof(struct DrawerData));

		// Tool window?
		if (icon->do_ToolWindow &&
			(copy->doc_DiskObject.do_ToolWindow=L_AllocMemH(gfmlib_data.memory,strlen(icon->do_ToolWindow)+1)))
			strcpy(copy->doc_DiskObject.do_ToolWindow,icon->do_ToolWindow);
	}

	// Allowed to copy images?
	if (!(flags&DOCF_NOIMAGE))
	{
		// Primary image?
		if (icon->do_Gadget.GadgetRender)
		{
			// Set image pointer
			copy->doc_DiskObject.do_Gadget.GadgetRender=&copy->doc_GadgetRender;

			// Copy image structure
			copy->doc_GadgetRender=*((struct Image *)icon->do_Gadget.GadgetRender);

			// Calculate plane size
			planesize=UMult32(
						UMult32(
							((copy->doc_GadgetRender.Width+15)>>3&0xfffe),
							copy->doc_GadgetRender.Height),
						copy->doc_GadgetRender.Depth);

			// Allocate image data and copy
			if (copy->doc_Image1=AllocVec(planesize,MEMF_CHIP|MEMF_CLEAR))
			{
				copy->doc_GadgetRender.ImageData=(UWORD *)copy->doc_Image1;
				CopyMem(
					(char *)(((struct Image *)icon->do_Gadget.GadgetRender)->ImageData),
					(char *)copy->doc_GadgetRender.ImageData,
					planesize);
			}

			// Failed
			else
			{
				L_FreeMemH(copy);	
				return 0;
			}
		}

		// Secondary image?
		if (icon->do_Gadget.SelectRender)
		{
			// Set image pointer
			copy->doc_DiskObject.do_Gadget.SelectRender=&copy->doc_SelectRender;

			// Copy image structure
			copy->doc_SelectRender=*((struct Image *)icon->do_Gadget.SelectRender);

			// Calculate plane size
			planesize=UMult32(
						UMult32(
							((copy->doc_SelectRender.Width+15)>>3&0xfffe),
							copy->doc_SelectRender.Height),
						copy->doc_SelectRender.Depth);

			// Allocate image data and copy
			if (copy->doc_Image2=AllocVec(planesize,MEMF_CHIP|MEMF_CLEAR))
			{
				copy->doc_SelectRender.ImageData=(UWORD *)copy->doc_Image2;
				CopyMem(
					(char *)(((struct Image *)icon->do_Gadget.SelectRender)->ImageData),
					(char *)copy->doc_SelectRender.ImageData,
					planesize);
			}

			// Failed
			else
			{
				copy->doc_DiskObject.do_Gadget.SelectRender=0;
				copy->doc_DiskObject.do_Gadget.Flags&=~GFLG_GADGHIMAGE;
			}
		}
	}

	// To signify that this is a copy, we point SpecialInfo at ourselves
	copy->doc_DiskObject.do_Gadget.SpecialInfo=(APTR)copy;

	return (struct DiskObject *)copy;
}


// Free a DiskObject copy
void __asm __saveds L_FreeDiskObjectCopy(
	register __a0 struct DiskObject *icon,
	register __a6 struct Library *GalileoFMBase)
{
	short type;

	// Valid icon?
	if (!icon) return;

	if	(IconBase->lib_Version>=44)
	{
		FreeDiskObject(icon);
		return;
	}


	// Is this a copy?
	if ((type=L_GetIconType(icon))==ICON_CACHED)
	{
		short count;
		DiskObjectCopy *copy=(DiskObjectCopy *)icon;

		// Free images
		FreeVec(copy->doc_Image1);
		FreeVec(copy->doc_Image2);

		// Free strings and drawerdata
		L_FreeMemH(copy->doc_DiskObject.do_DefaultTool);
		L_FreeMemH(copy->doc_DiskObject.do_DrawerData);
		L_FreeMemH(copy->doc_DiskObject.do_ToolWindow);

		// Tooltypes?
		if (copy->doc_DiskObject.do_ToolTypes)
		{
			// Free tooltypes
			for (count=0;copy->doc_DiskObject.do_ToolTypes[count];count++)
				L_FreeMemH(copy->doc_DiskObject.do_ToolTypes[count]);
			L_FreeMemH(copy->doc_DiskObject.do_ToolTypes);
		}

		// Free copy structure
		L_FreeMemH(copy);
	}

	// Or is it a NewIcon?
	else
	if (type==ICON_NEWICON)
	{
#ifdef _DEBUG_ICON
		if (((struct NewIconDiskObject *)icon)->nido_Flags&NIDOF_REMAPPED)
		    KPrintF("error! icon freed while still remapped\n");
#endif

		// Free the NewDiskObject part
		FreeNewDiskObject(((struct NewIconDiskObject *)icon)->nido_NewDiskObject);

		// Free our part
		FreeVec(icon);
	}

	// Assume it's a real icon
	else
	{
		FreeDiskObject(icon);
	}
}


// AddPort patch
void __asm __saveds L_WB_AddPort(register __a1 struct MsgPort *port, register __a6 struct Library *GalileoFMBase)
{
	// Invalid port?
	if (!port) return;

	// ARexx?
	if (port->mp_Node.ln_Name &&
		strcmp(port->mp_Node.ln_Name,"REXX")==0)
	{
		// Send notification
		L_SendNotifyMsg(GN_REXX_UP,0,0,0,0,0,GalileoFMBase);
	}

	// Pass through
	((void (* __asm)
		(register __a1 struct MsgPort *,register __a6 struct ExecBase *))gfmlib_data.wb_data.old_function[WB_PATCH_ADDPORT])
		(port,(struct Library *)*((ULONG *)4));
}


// CloseWindow patch
void __asm __saveds L_WB_CloseWindow(register __a0 struct Window *window, register __a6 struct Library *GalileoFMBase)
{
	struct PubScreenNode *node;
	struct Task *sigtask=0;
	UBYTE sigbit=0;

	// Invalid window?
	if (!window) return;

	// See if the screen is public
	if (node=FindPubScreen(window->WScreen,0))
	{
		// Is it the Galileo screen?
		if (strncmp(node->psn_Node.ln_Name,"GALILEO.",8)==0)
		{
			// Get task to signal
			sigtask=node->psn_SigTask;
			sigbit=node->psn_SigBit;
		}
	}

	// Close window
	((void (* __asm)
		(register __a0 struct Window *,register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_CLOSEWINDOW])
		(window,IntuitionBase);

	// Task to signal?
	if (sigtask) Signal(sigtask,1<<sigbit);
}


// Get library base to patch
struct Library *wb_get_patchbase(short type)
{
	struct Library *lib=0;

	// Look at type
	switch (type)
	{
		// Workbench
		case WB_PATCH_WORKBENCH:
			lib=WorkbenchBase;
			break;

		// DOS patched function
		case WB_PATCH_DOSFUNC:

			// Does flag allow these patches?
			if (gfmlib_data.flags&LIBDF_DOS_PATCH)
				lib=(struct Library *)DOSBase;
			break;

		// DOS
		case WB_PATCH_DOS:
			lib=(struct Library *)DOSBase;
			break;

		// Intuition
		case WB_PATCH_INTUITION:
			lib=(struct Library *)IntuitionBase;
			break;

		// Graphics
		case WB_PATCH_GFX:
			lib=(struct Library *)GfxBase;
			break;

		// Exec
		case WB_PATCH_EXEC:
			lib=(struct Library *)(struct Library *)*((ULONG *)4);
			break;

		// Icon
		case WB_PATCH_ICON:
			lib=IconBase;
			break;
	}

	return lib;
}


// CloseWindow patch
ULONG __asm __saveds L_PatchedWBInfo(
	register __a0 BPTR lock,
	register __a1 char *name,
	register __a2 struct Screen *screen,
	register __a6 struct Library *GalileoFMBase)
{
	struct GalileoSemaphore *sem;
	IPCData *main_ipc=0;
	char buf[10];

	Forbid();

	// Get Galileo semaphore?
	if (sem=(struct GalileoSemaphore *)FindSemaphore("Galileo Public Semaphore"))
		main_ipc=(IPCData *)sem->main_ipc;

	Permit();

	// Patch WBInfo() ?
	if (main_ipc && GetVar("Galileo/PatchWBInfo",buf,2,GVF_GLOBAL_ONLY)>0 && buf[0]=='1')
	{
		struct Library *InternalModuleBase;

		// Open the icon.gfmmodule
		if (InternalModuleBase=OpenLibrary("Galileo:modules/icon.gfmmodule",0))
		{
			BPTR old;
			struct List files;
			struct Node node;
			ULONG res;

			// Switch to this directory
			old=CurrentDir(lock);

			// Build fake file list
			NewList(&files);
			AddTail(&files,&node);
			node.ln_Name=name;

			// Call the module
			res=Module_Entry_Internal(&files,screen,0,main_ipc,0,0x96604497);

			// Restore current directory
			CurrentDir(old);

			// Close the module
			CloseLibrary(InternalModuleBase);
			return res;
		}
	}

	// Call original function
	return ((ULONG (* __asm)
		(register __a0 BPTR,register __a1 char *,register __a2 struct Screen *,register __a6 struct Library *))
			gfmlib_data.wb_data.old_function[WB_PATCH_WBINFO])
			(lock,name,screen,WorkbenchBase);
}


// Patched AddTask (for statistics)
APTR __asm __saveds L_PatchedAddTask(register __a1 struct Task *task,register __a2 APTR initialPC,register __a3 APTR finalPC, register __a6 struct Library *GalileoFMBase)
{
	// Increment task count
	++gfmlib_data.task_count;

	// Call original function
	return ((APTR (* __asm)
		(register __a1 struct Task *,register __a2 APTR,register __a3 APTR,register __a6 struct Library *))
			gfmlib_data.wb_data.old_function[WB_PATCH_ADDTASK])
			(task,initialPC,finalPC,(struct Library *)*((ULONG *)4));
}

// Patched RemTask (for statistics)
void __asm __saveds L_PatchedRemTask(register __a1 struct Task *task, register __a6 struct Library *GalileoFMBase)
{
	// Decrement task count
	--gfmlib_data.task_count;

	// Call original function
	((void (* __asm)
		(register __a1 struct Task *,register __a6 struct Library *))
			gfmlib_data.wb_data.old_function[WB_PATCH_REMTASK])
			(task,(struct Library *)*((ULONG *)4));
}


// Patched FindTask
struct Task *__asm __saveds L_PatchedFindTask(register __a1 char *name, register __a6 struct Library *GalileoFMBase)
{
	struct Task *task;

	// This task?
	if (!name) return ((struct ExecBase *)*((ULONG *)4))->ThisTask;

	// Call original function
	if (task=((struct Task * (* __asm)
			(register __a1 char *,register __a6 struct Library *))
				gfmlib_data.wb_data.old_function[WB_PATCH_FINDTASK])
				(name,(struct Library *)*((ULONG *)4)))
		return task;

	// Task not found, were they looking for workbench?
	if (strcmp(name,"Workbench")==0)
	{
		// Return the Launcher task (hopefully all they want is the path list)
		if (gfmlib_data.launcher)
			return (struct Task *)gfmlib_data.launcher->proc;
	}

	// Not found
	return 0;
}


// OpenWindowTags patch
struct Window *__asm __saveds L_PatchedOpenWindowTags(register __a0 struct NewWindow *newwin,register __a1 struct TagItem *tags, register __a6 struct Library *GalileoFMBase)
{
	// Is window being opened by the kludge task?
	if (gfmlib_data.open_window_kludge==(struct Process *)((struct ExecBase *)*((ULONG *)4))->ThisTask)
	{
		short x,y,w,h;
		struct Screen *scr;
		struct TagItem *tag;
		BOOL lock=0;

		// Get window size
		w=GetTagData(WA_Width,(newwin)?newwin->Width:0,tags);
		h=GetTagData(WA_Height,(newwin)?newwin->Height:0,tags);

		// Get screen
		if (!(scr=(struct Screen *)GetTagData(WA_CustomScreen,(newwin)?(ULONG)newwin->Screen:0,tags)))
		{
			scr=LockPubScreen(0);
			lock=1;
		}

		// Calculate position from mouse pointer		
		x=scr->MouseX-(w>>1);
		y=scr->MouseY-(h>>1);

		// Unlock screen if locked
		if (lock) UnlockPubScreen(0,scr);

		// Set new window position
		if (newwin)
		{
			newwin->LeftEdge=x;
			newwin->TopEdge=y;
		}
		if (tag=FindTagItem(WA_Left,tags)) tag->ti_Data=x;
		if (tag=FindTagItem(WA_Top,tags)) tag->ti_Data=y;

		// Kludge is only used once
		gfmlib_data.open_window_kludge=0;
	}

	// Open window
	return ((struct Window * (* __asm)
		(register __a0 struct NewWindow *,register __a1 struct TagItem *,register __a6 struct Library *))gfmlib_data.wb_data.old_function[WB_PATCH_OPENWINDOWTAGS])
		(newwin,tags,IntuitionBase);
}
