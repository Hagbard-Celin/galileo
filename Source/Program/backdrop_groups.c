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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofm.h"
#include "misc_protos.h"
#include "app_msg_protos.h"
#include "backdrop_protos.h"
#include "groups.h"
#include "menu_data.h"
#include "help.h"
#include "scripts.h"
#include "icons.h"
#include "graphics.h"
#include "lsprintf_protos.h"

// Read program groups (icon list must be locked)
void backdrop_read_groups(BackdropInfo *info)
{
	BPTR lock;
	struct FileInfoBlock __aligned fib;
	struct AnchorPath *anchor;
	short error;

	// Allocate anchor
	if (!(anchor=AllocVec(sizeof(struct AnchorPath)+256,MEMF_CLEAR)))
		return;

	// Initialise anchor
	anchor->ap_Strlen=256;

	// Search for icons
	error=MatchFirst("PROGDIR:groups/#?.info",anchor);

	// Continue while there's files
	while (!error)
	{
		char *ptr;

		// Strip .info suffix
		if (ptr=sufcmp(anchor->ap_Buf,".info")) *ptr=0;

		// See if directory exists to go with it
		if (lock=Lock(anchor->ap_Buf,ACCESS_READ))
		{
			// Examine file
			Examine(lock,&fib);
			UnLock(lock);

			// Is it a directory?
			if (fib.fib_DirEntryType>0)
			{
				BackdropObject *object;
				struct List *search;

				// Lock icon list
				lock_listlock(&info->objects,FALSE);

				// See if group already exists
				search=&info->objects.list;
				while (object=(BackdropObject *)FindNameI(search,fib.fib_FileName))
				{
					// Is it a group?
					if (object->bdo_type==BDO_GROUP)
						break;

					// Continue search
					search=(struct List *)object;
				}

				// Unlock icon list
				unlock_listlock(&info->objects);

				// Didn't have it already?
				if (!object)
				{
					// Create a new icon for this
					if (object=backdrop_leftout_new(info,anchor->ap_Buf,0,0))
					{
						// Change type to group
						object->bdo_type=BDO_GROUP;

						// Get icon
						backdrop_get_icon(info,object,GETICON_CD);
					}
				}
			}
		}

		// Get next entry
		error=MatchNext(anchor);
	}

	// Clean up
	MatchEnd(anchor);
	FreeVec(anchor);
}


// Open a group
void backdrop_open_group(BackdropInfo *info,BackdropObject *object,BOOL activate)
{
	GroupData *group;
	IPCData *ipc=0;

	// Valid object?
	if (!object || !object->bdo_icon || object->bdo_icon->do_Type!=WBDRAWER) return;

	// See if it's already open
	lock_listlock(&GUI->group_list,0);
	if (group=backdrop_find_group(object))
	{
		// Send activate message
		if (activate) IPC_Command(group->ipc,IPC_ACTIVATE,0,0,0,0);
	}
	unlock_listlock(&GUI->group_list);

	// If it was open, return
	if (group) return;

	// Allocate group packet
	if (!(group=AllocVec(sizeof(GroupData),MEMF_CLEAR)))
		return;

	// Fill in group packet
	group->screen=info->window->WScreen;
	if (object->bdo_icon->do_DrawerData)
		group->dimensions=*((struct IBox *)&object->bdo_icon->do_DrawerData->dd_NewWindow.LeftEdge);
	else
	{
		group->dimensions.Left=20;
		group->dimensions.Top=20;
		group->dimensions.Width=200;
		group->dimensions.Height=150;
	}
	strcpy(group->name,object->bdo_name);
	group->object=object;

	// Launch process
	IPC_Launch(
		&GUI->group_list,
		&ipc,
		"galileo_group",
		(ULONG)backdrop_group_handler,
		STACK_DEFAULT,
		(ULONG)group);

	// Failed?
	if (!ipc) FreeVec(group);
}


// See if a group is open
GroupData *backdrop_find_group(BackdropObject *object)
{
	IPCData *ipc;

	// Go through processes (list must be locked)
	for (ipc=(IPCData *)GUI->group_list.list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Is this a group process?
		if (ipc->proc &&
			ipc->proc->pr_Task.tc_Node.ln_Name &&
			strcmp(ipc->proc->pr_Task.tc_Node.ln_Name,"galileo_group")==0)
		{
			// Is it the group for this icon?
			if (((GroupData *)IPCDATA(ipc))->object==object)
			{
				GroupData *group=(GroupData *)IPCDATA(ipc);

				// If window is open, update window position
				if (group->window) group->dimensions=*((struct IBox *)&group->window->LeftEdge);

				// Return pointer to group
				return group;
			}
		}
	}

	// Not found
	return 0;
}


// Group handler
void __saveds backdrop_group_handler(void)
{
	IPCData *ipc;
	GroupData *group=0;

	// Do group
	if (ipc=IPC_ProcStartup((ULONG *)&group,backdrop_group_init))
	{
		// Read objects
		SetBusyPointer(group->window);
		backdrop_read_group_objects(group);
		ClearPointer(group->window);

		// Event loop
		FOREVER
		{
			IPCMessage *msg;
			BOOL quit_flag=0;

			// Got an AppWindow?
			if (group->appwindow)
			{
				struct AppMessage *amsg;
				BOOL beep=0;
				char disk[5] = "Disk";

				// AppMessages?
				while (amsg=(struct AppMessage *)GetMsg(group->appport))
				{
					BackdropObject *drop_obj;
					GalileoListerAppMessage *lamsg = 0;
					//LONG numargs;
					struct WBArg *arglist;
					struct GLAData *argdata = 0;
					WORD mousex, mousey;
					WORD arg, numargs;

					if (amsg->am_Type == MTYPE_LISTER_APPWINDOW && amsg->am_Class == GLAMCLASS_LISTER)
					{
					    lamsg = (GalileoListerAppMessage *)amsg;
					    numargs = lamsg->glam_NumArgs;
					    arglist = lamsg->glam_ArgList;
					    argdata = lamsg->glam_ArgData;
					    mousex = lamsg->glam_MouseX;
					    mousey = lamsg->glam_MouseY;
					}
					else
					{
					    numargs = amsg->am_NumArgs;
					    arglist = amsg->am_ArgList;
					    mousex = amsg->am_MouseX;
					    mousey = amsg->am_MouseY;
					}
					    // Lock backdrop list
					    lock_listlock(&group->info->objects,1);

					    // Set busy pointer
					    if (group->window) SetBusyPointer(group->window);

					    // Dropped on an object?
					    if (drop_obj=backdrop_get_object(group->info,mousex,mousey,0))
					    {
						    UWORD qual;

						    // Get qualifiers
						    qual=(InputBase)?PeekQualifier():0;

						    // Is shift/alt down?
						    if ((qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))==(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))
						    {
							    BPTR source_lock = 0;

							    if (arglist[0].wa_Lock)
							        source_lock = arglist[0].wa_Lock;
							    else
							    if (lamsg)
							        source_lock = lamsg->glam_Lock;

							    if (!argdata && WBArgDir(&arglist[0]))
						            {
							        BPTR parent_lock;

							        // Try to get parent dir
							        if (parent_lock = ParentDir(arglist[0].wa_Lock))
							        {
							            D_S(struct FileInfoBlock,tmp_fib);

							            // Get directory name
								    Examine(arglist[0].wa_Lock,tmp_fib);
							            backdrop_replace_icon_image(group->info,tmp_fib->fib_FileName,parent_lock,drop_obj);
							            UnLock(parent_lock);
							        }
							        else
							            backdrop_replace_icon_image(group->info,disk,arglist[0].wa_Lock,drop_obj);
						            }
							    else
							    {
								if (argdata && argdata[0].glad_Flags == AAEF_DEV)
								    backdrop_replace_icon_image(group->info,disk,source_lock,drop_obj);
							        else
								    backdrop_replace_icon_image(group->info,arglist[0].wa_Name,source_lock,drop_obj);
							    }
						    }

						    // Run program with args
						    else
						    backdrop_object_open(
							    group->info,
							    drop_obj,
							    0,
							    0,
							    lamsg,
							    (lamsg)?0:amsg);
					    }

					    // Otherwise, adding objects to the group
					    else
					    for (arg=0;arg<numargs;arg++)
					    {
						    // Valid name?
						    if (*arglist[arg].wa_Name)
						    {
							    short x,y;
							    BPTR lock = 0;

							    if (arglist[arg].wa_Lock)
								lock = arglist[arg].wa_Lock;
							    else
							    if (lamsg)
								lock = lamsg->glam_Lock;

							    // Default to no position
							    x=-1;
							    y=-1;

							    if (argdata)
							    {
							        // Get icon position
							        x=lamsg->glam_DragOffset.x+lamsg->glam_MouseX+argdata[arg].glad_DropPos.x;
							        y=lamsg->glam_DragOffset.y+lamsg->glam_MouseY+argdata[arg].glad_DropPos.y;
							    }

							    // Add group object
							    backdrop_group_add_object(group->name,group->info,arglist[arg].wa_Name,lock,x,y);
						    }

						    // Otherwise, set beep flag for error
						    else
						    if (!beep)
						    {
							    beep=1;
							    DisplayBeep(group->window->WScreen);
						    }
					    }

					    // Clear busy pointer
					    if (group->window) ClearPointer(group->window);

					    // Unlock backdrop list
					    unlock_listlock(&group->info->objects);

					    if (lamsg)
						free_lister_appmsg(lamsg);
					    else
						ReplyMsg((struct Message *)amsg);
				}
			}

			// Icon notification
			if (group->info->notify_req)
			{
				GalileoNotify *notify;

				// Get notify message	
				if (notify=(GalileoNotify *)GetMsg(group->info->notify_port))
					backdrop_check_notify(group->info,notify,0);
			}

			// IPC messages?
			while (msg=(IPCMessage *)GetMsg(ipc->command_port))
			{
				// Look at message
				switch (msg->command)
				{
					// Activate
					case IPC_ACTIVATE:

						// Bring window to front
						if (group->window)
							backdrop_show_group(group);
						break;


					// Quit
					case IPC_QUIT:
						quit_flag=1;
						group->got_quit=1;
						break;


					// Hide
					case IPC_HIDE:
						backdrop_hide_group(group);
						break;


					// Show
					case IPC_SHOW:
						group->screen=(struct Screen *)msg->data;
						backdrop_show_group(group);
						break;


					// Reset (menus)
					case IPC_RESET:

						// Gotta window?
						if (group->window)
						{
							// Reset menus?
							if (msg->flags)
							{
								display_free_menu(group->window);
								display_get_menu(group->window);
							}

							// Fix menus
							display_fix_menu(group->window,WINDOW_GROUP,0);
						}
						break;


					// New font
					case GROUP_NEW_FONT:

						// Get new font
						backdrop_get_font(group->info);

						// Redraw objects
						backdrop_show_objects(group->info,BDSF_CLEAR|BDSF_RESET);
						break;


					// New name
					case GROUP_NEW_NAME:

						// Copy name
						strcpy(group->name,msg->data_free);

						// Update window title
						if (group->window) SetWindowTitles(group->window,group->name,(char *)-1);
						break;


					// Add a new icon
					case GROUP_ADD_ICON:

						// Lock backdrop list
						lock_listlock(&group->info->objects,1);

						// Set busy pointer
						if (group->window) SetBusyPointer(group->window);

						// Add object
						backdrop_group_add_object(group->name,group->info,msg->data_free,(BPTR)msg->data,-1,-1);

						// Clear busy pointer
						if (group->window) ClearPointer(group->window);

						if (msg->data)
						    UnLock((BPTR)msg->data);

						// Unlock backdrop list
						unlock_listlock(&group->info->objects);
						break;


					// New backfill pattern
					case LISTER_BACKFILL_CHANGE:

						// Window open?
						if (group->window)
						{
							// Install appropriate hook
							InstallLayerHook(
								group->window->WLayer,
								(msg->flags)?&group->pattern.hook:LAYERS_BACKFILL);

							// Redraw window
							erase_window(group->window);

							// Redraw icons
							backdrop_show_objects(group->info,0);
						}
						break;


					// Delete from group
					case GROUP_DELETE:
						SetBusyPointer(group->window);
						backdrop_remove_group_objects(group,(BackdropObject *)msg->data);
						ClearPointer(group->window);
						break;


					// Help
					case IPC_HELP:

						// Show help for group
						help_show_help(HELP_PROGRAM_GROUP,0);
						break;


					// Do a function
					case LISTER_DO_FUNCTION:

						// Arrange icons?
						if (msg->data>=(APTR)MENU_LISTER_ARRANGE_NAME &&
							msg->data<=(APTR)MENU_LISTER_ARRANGE_SIZE)
						{
							// Do cleanup
							backdrop_cleanup(group->info,BSORT_NAME+(((ULONG)msg->data)-MENU_LISTER_ARRANGE_NAME),0);
						}
						break;
				}

				// Reply to message
				IPC_Reply(msg);
			}

			// Is window open?
			if (group->window)
			{
				struct IntuiMessage *imsg;

				// Check timer
				if (CheckTimer(group->timer))
				{
					// Dragging something?
					if (group->info->flags&BDIF_DRAGGING)
					{
						// Check for deadlocks
						if (group->info->last_tick==group->info->tick_count)
						{
							// Stop drag
							backdrop_stop_drag(group->info);
						}

						// Remember tick count
						group->info->last_tick=group->info->tick_count;
					}

					// Re-start timer
					StartTimer(group->timer,0,500000);
				}

				// Window messages
				while (imsg=(struct IntuiMessage *)GetMsg(group->window->UserPort))
				{
					struct IntuiMessage msg_copy;
					struct MenuItem *item;

					// Copy message
					msg_copy=*imsg;

					// Menu verify?
					if (imsg->Class==IDCMP_MENUVERIFY)
					{
						// See if we want to swallow it
						backdrop_test_rmb(group->info,imsg, &msg_copy, BTRM_WINDOW);
					}

					// Resize/refresh?
					if (imsg->Class==IDCMP_NEWSIZE ||
						imsg->Class==IDCMP_REFRESHWINDOW)
					{
						// Handle message
						backdrop_idcmp(group->info,imsg,0);

						// Reply to message
						ReplyMsg((struct Message *)imsg);
						continue;
					}

					// Reply to message
					ReplyMsg((struct Message *)imsg);

					// Is it a backdrop message?
					if (backdrop_idcmp(group->info,&msg_copy,0))
						continue;

					// Look at message class
					switch (msg_copy.Class)
					{
						// Window closed
						case IDCMP_CLOSEWINDOW:
							quit_flag=1;
							break;


						// Window is inactive
						case IDCMP_INACTIVEWINDOW:

							// Abort timer if running
							StopTimer(group->timer);
							break;


						// Window is active
						case IDCMP_ACTIVEWINDOW:

							// Start timer if not running
							StartTimer(group->timer,1,0);
							break;


						// Key press
						case IDCMP_RAWKEY:

							// Help?
							if (msg_copy.Code==0x5f &&
								!(msg_copy.Qualifier&VALID_QUALIFIERS))	
							{
								help_get_help(
									msg_copy.MouseX+group->window->LeftEdge,
									msg_copy.MouseY+group->window->TopEdge,
									msg_copy.Qualifier);
							}

							// Close?
							else
							if (msg_copy.Code==0x45 &&
								msg_copy.Qualifier&IEQUAL_ANYSHIFT) quit_flag=1;
							break;


						// Button pressed
						case IDCMP_MOUSEBUTTONS:

							// Right button?
							if (msg_copy.Code==MENUDOWN)
							{
								UWORD res;

								// Do popup menu
								if ((res=group_do_popup(group))!=(UWORD)-1)
								{
									// Help?
									if (res&POPUP_HELPFLAG)
									{
										// Get help ID
										res&=~POPUP_HELPFLAG;

										// Do help
										help_menu_help(res,0);
										break;
									}

									// Do the function
									quit_flag=backdrop_group_do_function(group,res,0);
								}
							}
							break;


						// Menu event
						case IDCMP_MENUPICK:
						case IDCMP_MENUHELP:
							{
								struct Menu *oldstrip=group->window->MenuStrip;
								UWORD nextselect;

								// Get item
								nextselect=msg_copy.Code;
								while (item=ItemAddress(group->window->MenuStrip,nextselect))
								{
									// get next
									nextselect=item->NextSelect;

									// Help?
									if (msg_copy.Class==IDCMP_MENUHELP)
									{
										help_menu_help((long)GTMENUITEM_USERDATA(item),0);
										break;
									}

									// Do the function
									quit_flag=backdrop_group_do_function(group,(ULONG)GTMENUITEM_USERDATA(item),item);

									// Check valid next
									if (!nextselect || !group->window || oldstrip!=group->window->MenuStrip)
										break;
								}
							}
							break;
					}
				}
			}

			// Check quit flag
			if (quit_flag) break;

			// Wait for event
			Wait(	1<<ipc->command_port->mp_SigBit|
					1<<group->timer->port->mp_SigBit|
					((group->info->notify_req)?(1<<group->info->notify_port->mp_SigBit):0)|
					((group->window)?(1<<group->window->UserPort->mp_SigBit):0)|
					((group->appwindow)?(1<<group->appport->mp_SigBit):0));
		}

		// Close window
		backdrop_free_group(group);

		// Send goodbye
		IPC_Goodbye(ipc,&main_ipc,0);
	}

	// Free backdrop stuff
	backdrop_free(group->info);

	// Free group data
	FreeVec(group);

	// Exit
	IPC_Free(ipc);

#ifdef RESOURCE_TRACKING
	ResourceTrackingEndOfTask();
#endif
}


// Initialise group display
ULONG __asm __saveds backdrop_group_init(
	register __a0 IPCData *ipc,
	register __a1 GroupData *group)
{
	// Store IPC pointer
	group->ipc=ipc;
	SET_IPCDATA(ipc,group);

	// Create message port
	if (!(group->appport=CreateMsgPort()))
	    return 0;

	group->appport->mp_Node.ln_Type = GNT_LISTER_APPMSG_PORT;

	// Create backdrop info and timer
	if (!(group->info=backdrop_new(ipc,BDIF_GROUP)) ||
		!(group->timer=AllocTimer(UNIT_VBLANK,0)))
		return 0;

	// Open display
	if (!(backdrop_show_group(group)))
	{
		FreeTimer(group->timer);
		return 0;
	}

	// Run script
	RunScript(SCRIPT_OPEN_GROUP,group->name);

	return 1;
}


// Show group display
BOOL backdrop_show_group(GroupData *group)
{
	// Already open?
	if (group->window)
	{
		// Pop to front
		WindowToFront(group->window);
		ActivateWindow(group->window);
		return 1;
	}

	// Check dimensions
	if (group->dimensions.Left<0) group->dimensions.Left=0;
	if (group->dimensions.Top<0) group->dimensions.Top=0;
	if (group->dimensions.Width<50) group->dimensions.Width=260;
	if (group->dimensions.Height<20) group->dimensions.Height=80;

	// Initialise backfill pattern
	group->pattern.hook.h_Entry=(ULONG (*)())PatternBackfill;
	group->pattern.hook.h_Data=group->info;
	group->pattern.pattern=&GUI->pattern[PATTERN_LISTER];

	// Open window
	if (!(group->window=OpenWindowTags(0,
		WA_Left,group->dimensions.Left,
		WA_Top,group->dimensions.Top,
		WA_Width,group->dimensions.Width,
		WA_Height,group->dimensions.Height,
		WA_Title,group->name,
		WA_MinWidth,100,
		WA_MinHeight,50,
		WA_MaxWidth,-1,
		WA_MaxHeight,-1,
		WA_IDCMP,
				IDCMP_ACTIVEWINDOW|
				IDCMP_CLOSEWINDOW|
				IDCMP_GADGETDOWN|
				IDCMP_GADGETUP|
				IDCMP_INACTIVEWINDOW|
				IDCMP_MENUHELP|
				IDCMP_MENUPICK|
				IDCMP_MENUVERIFY|
				IDCMP_MOUSEBUTTONS|
				IDCMP_MOUSEMOVE|
				IDCMP_NEWSIZE|
				IDCMP_RAWKEY|
				IDCMP_REFRESHWINDOW,
		WA_Activate,TRUE,
		WA_CloseGadget,TRUE,
		WA_DepthGadget,TRUE,
		WA_DragBar,TRUE,
		WA_SimpleRefresh,TRUE,
		WA_SizeGadget,TRUE,
		WA_SizeBBottom,TRUE,
		WA_SizeBRight,TRUE,
		WA_MenuHelp,TRUE,
		WA_NewLookMenus,TRUE,
		WA_PubScreen,group->screen,
		WA_PubScreenFallBack,TRUE,
		WA_ScreenTitle,GUI->screen_title,
		WA_BackFill,&group->pattern,
		TAG_END))) return 0;

	// Initialise ID
	SetWindowID(group->window,&group->id,WINDOW_GROUP,(struct MsgPort *)group->ipc);

	// Get menus
	display_get_menu(group->window);
	display_fix_menu(group->window,WINDOW_GROUP,0);

	// Initialise backdrop
	NewList(&group->info->boopsi_list);
	backdrop_init_info(group->info,group->window,0);

	// Add AppWindow
	if (group->appport)
		group->appwindow=AddAppWindowA(WINDOW_GROUP,0,group->window,group->appport,0);

	// Get font to use
	backdrop_get_font(group->info);

	// Initialise backdrop objects
	backdrop_get_objects(group->info,BGOF_SHOW);

	// Start timer
	StartTimer(group->timer,1,0);
	return 1;
}


// Hide group display
void backdrop_hide_group(GroupData *group)
{
	// Window open?
	if (group->window)
	{
		// Abort timer
		StopTimer(group->timer);

		// Remove appwindow
		if (group->appwindow)
		{
			RemoveAppWindow(group->appwindow);
			group->appwindow=0;
		}

		// Store dimensions
		group->dimensions=*((struct IBox *)&group->window->LeftEdge);

		// Free menus
		display_free_menu(group->window);

		// Free icon remapping
		backdrop_free_remap(group->info,group->window);

		// Close window
		CloseWindow(group->window);
		group->window=0;

		// Free backdrop stuff
		backdrop_free_info(group->info);
	}
}


// Free group display
void backdrop_free_group(GroupData *group)
{
	if (group)
	{
		// If window is open, run script
		if (group->window && !group->got_quit)
			RunScript(SCRIPT_CLOSE_GROUP,group->name);

		// Close display
		backdrop_hide_group(group);

		// Didn't get quit from main program?
		if (!group->got_quit)
		{
			// Lock main backdrop list
			lock_listlock(&GUI->backdrop->objects,0);

			// See if owner is still valid
			if (find_backdrop_object(GUI->backdrop,group->object))
			{
				// Update window position
				if (group->object->bdo_icon &&
					group->object->bdo_icon->do_DrawerData)
				{
					*((struct IBox *)&group->object->bdo_icon->do_DrawerData->dd_NewWindow.LeftEdge)=
						group->dimensions;
				}
			}

			// Unlock main backdrop list
			unlock_listlock(&GUI->backdrop->objects);
		}

		// Free port
		if (group->appport)
		{
			struct Message *msg;
			while (msg=GetMsg(group->appport))
				ReplyMsg(msg);
			DeleteMsgPort(group->appport);
			group->appport=0;
		}

		// Close timer
		FreeTimer(group->timer);
	}
}


// Read the contents of a group
void backdrop_read_group_objects(GroupData *group)
{
	short ok;
	BPTR lock,old;
	struct FileInfoBlock __aligned fib;

	if (!(lock =Lock("PROGDIR:groups",ACCESS_READ)))
	    return;

	old = CurrentDir(lock);

	// Lock path
	if (lock=Lock(group->name,ACCESS_READ))
	{
		// Change to this directory
		UnLock(CurrentDir(lock));

		// Search directory
		ok=Examine(lock,&fib);
		while (ok && ExNext(lock,&fib))
		{
			BackdropObject *object;
			group_record *record;

			// Dereference object
			if (record = group_dereference(0,fib.fib_FileName))
			{
			    BPTR volume_lock, tmp;

			    // Lock backdrop list
			    lock_listlock(&group->info->objects,1);

			    if (volume_lock = LockFromVolIdPath(record->name, NULL, &record->date, record->len, LFPF_TRY_ICON))
			    {
				tmp = CurrentDir(volume_lock);

				// Create a new icon for this
				if (object=backdrop_leftout_new(group->info,record->name,0,fib.fib_FileName))
				{
					BPTR parent_lock, tmp_lock;

					// Got a position?
					if (record->pos.x!=-1 && record->pos.y!=-1)
					{
						// Set position
						object->bdo_flags|=BDOF_LEFTOUT_POS;
						object->bdo_custom_pos=(record->pos.x<<16)|record->pos.y;
					}

					// No initial position
					else object->bdo_flags|=BDOF_AUTO_POSITION;

					object->bdo_date = record->date;
					object->bdo_vol_name_len = record->len;

					if (tmp_lock = LockFromPath(record->name + record->len + 1, NULL, FALSE))
					{
					    parent_lock = ParentDir(tmp_lock);
					    UnLock(tmp_lock);
					    UnLock(CurrentDir(parent_lock));
					    // Add object
					    backdrop_new_group_object(group->info,object,NULL);
					    UnLock(CurrentDir(tmp));
					}
					else
					    UnLock(CurrentDir(tmp));
				}
			        else
				    UnLock(CurrentDir(tmp));
			    }

			    FreeVec(record);

			    // Unlock backdrop list
			    unlock_listlock(&group->info->objects);
			}

			// Window open?
			if (group->window)
			{
				struct IntuiMessage *msg;

				// Any messages?
				while (msg=(struct IntuiMessage *)GetMsg(group->window->UserPort))
				{
					// Close = abort
					if (msg->Class==IDCMP_CLOSEWINDOW) ok=0;

					// Refresh or resize?
					else
					if (msg->Class==IDCMP_REFRESHWINDOW ||
						msg->Class==IDCMP_NEWSIZE)
						backdrop_idcmp(group->info,msg,0);

					// Reply
					ReplyMsg((struct Message *)msg);
				}
			}
		}

		// Restore directory
		UnLock(CurrentDir(old));
	}

	// Lock backdrop list
	lock_listlock(&group->info->objects,1);

	// Update virtual size
	backdrop_calc_virtual(group->info);

	// Unlock backdrop list
	unlock_listlock(&group->info->objects);
}


// Check for auto-open groups
void backdrop_check_groups(BackdropInfo *info)
{
	BackdropObject *object;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Auto-open group?
		if (object->bdo_type==BDO_GROUP && (object->bdo_flags&BDOF_AUTO_OPEN))
		{
			// Clear auto-open flag
			object->bdo_flags&=~BDOF_AUTO_OPEN;

			// Open it
			backdrop_open_group(info,object,0);
		}
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}


// Add an object to a group
void backdrop_group_add_object(
	char *groupname,
	BackdropInfo *info,
	char *name,
	BPTR parent_dir,
	short x,
	short y)
{
	struct DiskObject *icon;
	char *path = 0;
	BPTR lock, org_dir;
	BOOL fail=0;

	if (!parent_dir || !name)
	    return;

	org_dir = CurrentDir(parent_dir);


	// Get object icon, must be tool or project
	if (!(icon=GetProperIcon(name,0,0)) ||
		(icon->do_Type!=WBTOOL && icon->do_Type!=WBPROJECT))
	{
		// Failure
		if (icon) FreeCachedDiskObject(icon);
		DisplayBeep(GUI->screen_pointer);
		return;
	}

	if (lock=Lock("PROGDIR:groups",ACCESS_READ))
	{
	    BPTR group_dir, tmp;

	    CurrentDir(lock);

	    // Change to group directory
	    if (group_dir = Lock(groupname, ACCESS_READ))
	    {
		UnLock(CurrentDir(group_dir));

		// Check an object of this name isn't already in the group
	        if (tmp = Lock(name, ACCESS_READ))
	        {
		    // Already exists
		    UnLock(tmp);
		    fail=1;
		}
	    }
	    else
	    {
		UnLock(CurrentDir(org_dir));
		fail = 1;
	    }
	}
	else
	    fail = 1;

	if (!fail)
	{
	    if (path = PathFromLock(NULL, parent_dir, PFLF_END_SLASH, NULL))
	    {
	        struct FileLock *fl;
	        struct DosList *volume;
		group_record object;

	        // Get filelock pointer
		fl=(struct FileLock *)BADDR(parent_dir);

	        // Get volume entry
	        if (volume=(struct DosList *)BADDR(fl->fl_Volume))
	        {
		    object.date	= volume->dol_misc.dol_volume.dol_VolumeDate;
		    object.len = *(UBYTE *)BADDR(volume->dol_Name);

		    object.pos.x =x;
		    object.pos.y =y;
		    object.flags = GetIconFlags(icon);

	            // Create pointer file
		    if (!(group_write_data(name, path, name, &object)))
		        fail = 1;
	        }
		else
		    fail = 1;
	    }
	    else
		fail = 1;
	}

	// Free icon
	FreeCachedDiskObject(icon);

	// Written successfully?
	if (!fail && info)
	{
	    BPTR volume_dir;
	    BackdropObject *object;

	    UnLock(CurrentDir(parent_dir));
	    if (volume_dir = Lock(":", ACCESS_READ))
	    {
		CurrentDir(volume_dir);
		// Create backdrop entry
		if (object=backdrop_leftout_new(info,name,path,name))
		{
			// Got a position?
			if (x!=-1 && y!=-1)
			{
				// Set position
				object->bdo_flags|=BDOF_LEFTOUT_POS;
				object->bdo_custom_pos=(x<<16)|y;
			}

			// No initial position
			else object->bdo_flags|=BDOF_AUTO_POSITION;

			CurrentDir(parent_dir);
			// Add object
			backdrop_new_group_object(info,object,BDNF_RECALC);

		}
		else
		{
		    CurrentDir(parent_dir);
		    fail=1;
		}

		UnLock(volume_dir);
	    }
	}

	if (path)
	    FreeMemH(path);

	CurrentDir(org_dir);

	if (lock)
	    UnLock(lock);

	// Did we fail?
	if (fail)
	{
		// Flash screen
		DisplayBeep(GUI->screen_pointer);
	}
}


// Delete a group
void backdrop_delete_group(BackdropInfo *info,BackdropObject *object)
{
	struct FileInfoBlock __aligned fib;
	BOOL delok=1;
	BPTR lock,old,dir;

	// Lock group path
	if (!(lock=Lock("PROGDIR:groups",ACCESS_READ)))
		return;

	// CD to directory
	old=CurrentDir(lock);

	// Lock group directory
	if (!(dir=Lock(object->bdo_name,ACCESS_READ)))
	{
		// Failed
		UnLock(CurrentDir(old));
		return;
	}

	// Examine directory
	Examine(dir,&fib);

	// CD to directory
	CurrentDir(dir);

	// Go through group directory
	while (ExNext(dir,&fib))
	{
		// Delete this file
		if (!DeleteFile(fib.fib_FileName))
		{
			delok=0;
			break;
		}
	}

	// Restore CD
	UnLock(CurrentDir(lock));

	// Deleted ok?
	if (delok)
	{
		GroupData *group;

		// See if group is open
		lock_listlock(&GUI->group_list,0);
		if (group=backdrop_find_group(object))
		{
			// Send quit message
			IPC_Command(group->ipc,IPC_QUIT,0,0,0,0);
		}
		unlock_listlock(&GUI->group_list);
	}

	// Delete group directory
	if (DeleteFile(object->bdo_name))
	{
		// Delete icon
		DeleteDiskObject(object->bdo_name);
	}

	// Restore CD
	UnLock(CurrentDir(old));
}


// Remove group objects
void backdrop_remove_group_objects(GroupData *data,BackdropObject *only_one)
{
	BackdropObject *object;
	BPTR lock,old;
	short count=0;
	char buf[80];

	// CD to group directory
	lsprintf(buf,"PROGDIR:groups/%s",data->name);
	if (!(lock=Lock(buf,ACCESS_READ)))
		return;
	old=CurrentDir(lock);

	// Lock backdrop list
	lock_listlock(&data->info->objects,1);

	// Go through list
	for (object=(BackdropObject *)data->info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;)
	{
		BackdropObject *next=(BackdropObject *)object->bdo_node.ln_Succ;

		// Selected?
		if ((only_one && only_one==object) ||
			(!only_one && object->bdo_state))
		{
			// Delete from group directory
			if (DeleteFile(object->bdo_device_name))
			{
				// Erase object
				backdrop_erase_icon(data->info,object,0);

				// Remove and free object
				backdrop_remove_object(data->info,object);
				++count;
			}

			// Only doing one?
			if (only_one) break;
		}

		// Get next
		object=next;
	}

	// Unlock backdrop list
	unlock_listlock(&data->info->objects);

	// Restore current directory
	UnLock(CurrentDir(old));

	// Refresh backdrop objects
	if (count) backdrop_calc_virtual(data->info);
}


// Snapshot a group window
void backdrop_snapshot_group(BackdropInfo *info,char *name)
{
	struct DiskObject *icon;

	// Get icon
	if (icon=GetDiskObject(name))
	{
		// Valid drawer icon?
		if (icon->do_DrawerData)
		{
			// Update window position
			*((struct IBox *)&icon->do_DrawerData->dd_NewWindow.LeftEdge)=
				*((struct IBox *)&info->window->LeftEdge);

			// Store icon
			if (!(WriteIcon(name,icon)))
				DisplayBeep(info->window->WScreen);
		}

		// Free icon
		FreeDiskObject(icon);
	}
	else DisplayBeep(info->window->WScreen);
}


// Do function
BOOL backdrop_group_do_function(GroupData *group,ULONG id,struct MenuItem *item)
{
	BOOL quit_flag=0;
	MenuEvent *event;

	// Look at ID
	switch (id)
	{
		// Open icons
		case MENU_ICON_OPEN:
			{
				iconopen_packet *packet;

				// Allocate packet
				if (packet=get_icon_packet(group->info,0,0,0))
				{
					// Start process
					if (!(misc_startup(
						"galileo_group_open",
						MENU_ICON_OPEN,
						group->window,
						packet,
						1))) FreeVec(packet);
				}
			}
			break;


		// Snapshot backdrop objects
		case MENU_ICON_SNAPSHOT_ICON:
		case MENU_ICON_UNSNAPSHOT:

			// Set busy pointer
			SetBusyPointer(group->window);

			// Snapshot icons
			backdrop_snapshot(group->info,(id==MENU_ICON_UNSNAPSHOT),0,0);

			// Clear busy pointer
			ClearPointer(group->window);
			break;


		// Snapshot window/all
		case MENU_ICON_SNAPSHOT_WINDOW:
		case MENU_ICON_SNAPSHOT_ALL:
			{
				char buf[80];

				// Set busy pointer
				SetBusyPointer(group->window);

				// Snapshot icons
				if (id==MENU_ICON_SNAPSHOT_ALL)
					backdrop_snapshot(group->info,0,1,0);

				// Snapshot owner
				lsprintf(buf,"PROGDIR:groups/%s",group->name);
				backdrop_snapshot_group(group->info,buf);

				// Clear busy pointer
				ClearPointer(group->window);
			}
			break;


		// Select All
		case MENU_ICON_SELECT_ALL:
			backdrop_select_all(group->info,1);
			break;


		// Clean Up
		case MENU_ICON_CLEANUP:
			backdrop_cleanup(group->info,0,0);
			break;


		// Rename
		case MENU_ICON_RENAME:
		case MENU_ICON_DELETE:
			{
				iconopen_packet *packet;

				// Get packet
				if (packet=get_icon_packet(group->info,0,0,0))
				{
					if (!(misc_startup(
						(id==MENU_ICON_RENAME)?"galileo_group_rename":"galileo_group_delete",
						id,
						group->window,
						packet,0))) FreeVec(packet);
				}
			}
			break;


		// Information
		case MENU_ICON_INFO:
			backdrop_info(group->info,0);
			break;


		// Leave objects out
		case MENU_ICON_LEAVE_OUT:

			SetBusyPointer(group->window);
			backdrop_leave_icons_out(group->info,0,0);
			ClearPointer(group->window);
			break;


		// Reset icons
		case MENU_ICON_RESET:

			// Set busy pointer
			SetBusyPointer(group->window);

			// Free objects
			backdrop_free_list(group->info);

			// Clear display
			backdrop_show_objects(group->info,BDSF_CLEAR_ONLY|BDSF_RECALC);

			// Read objects
			backdrop_read_group_objects(group);

			// Clear busy pointer
			ClearPointer(group->window);
			break;


		// Close
		case MENU_LISTER_CLOSE:
			quit_flag=1;
			break;


		// Pass on
		default:

			// Allocate event
			if (event=AllocVec(sizeof(MenuEvent),0))
			{
				// Fill out event
				event->id=id;
				event->menu=item;
				event->window=group->window;

				// Send command to main process
				IPC_Command(&main_ipc,MAIN_MENU_EVENT,0,0,event,0);
			}
			break;
	}

	return quit_flag;
}


UWORD group_do_popup(GroupData *group)
{
	PopUpHandle *menu;
	UWORD res=(UWORD)-1;

	// Initialise popup menu
	if (menu=PopUpNewHandle(0,0,&locale))
	{
		PopUpItem *item;

		// Snapshot sub-item
		if (item=PopUpNewItem(menu,MSG_ICON_SNAPSHOT_MENU,0,POPUPF_SUB|POPUPF_LOCALE))
		{
			// Create list for submenus
			if (PopUpItemSub(menu,item))
			{
				short icons=POPUPF_DISABLED,selicons=POPUPF_DISABLED;

				// Lock backdrop list
				lock_listlock(&group->info->objects,0);

				// Got icons?
				if (!IsListEmpty(&group->info->objects.list))
				{
					BackdropObject *object;
					icons=0;

					// Go through backdrop list
					for (object=(BackdropObject *)group->info->objects.list.lh_Head;
						object->bdo_node.ln_Succ;
						object=(BackdropObject *)object->bdo_node.ln_Succ)
					{
						// Selected icon?
						if (object->bdo_state)
						{
							selicons=0;
							break;
						}
					}
				}

				// Unlock backdrop list
				unlock_listlock(&group->info->objects);

				// Snapshot items
				PopUpNewItem(menu,MSG_ICON_SNAPSHOT_ICONS,MENU_ICON_SNAPSHOT_ICON,selicons);
				PopUpNewItem(menu,MSG_ICON_SNAPSHOT_WINDOW,MENU_ICON_SNAPSHOT_WINDOW,0);
				PopUpSeparator(menu);
				PopUpNewItem(menu,MSG_ICON_SNAPSHOT_ALL,MENU_ICON_SNAPSHOT_ALL,icons);

				// End sub menu
				PopUpEndSub(menu);
			}
		}

		// Add items
		PopUpNewItem(menu,MSG_ICON_CLEANUP,MENU_ICON_CLEANUP,0);

		// Do popup
		res=DoPopUpMenu(group->window,&menu->ph_Menu,NULL,MENUDOWN);

		// Free popup menu
		PopUpFreeHandle(menu);
	}

	return res;
}
