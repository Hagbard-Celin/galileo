/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software

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

// Read AppIcon list
void backdrop_read_appicons(BackdropInfo *info,short flags)
{
	AppEntry *icon;
	BackdropObject *object;

	// Lock backdrop list
	if (flags&BDAF_LOCK) lock_listlock(&info->objects,1);

	// Lock AppList
	icon=LockAppList();

	// Go through AppIcons
	while (icon=NextAppEntry(icon,APP_ICON))
	{
		// Showing AppIcons, or a local icon?
		if (environment->env->display_options&DISPOPTF_SHOW_APPICONS ||
			icon->ae_flags&APPENTF_LOCAL)
		{
			// See if icon is already in list
			if (!(object=backdrop_find_appicon(info,icon)))
			{
				// Add this AppIcon
				object=backdrop_add_appicon(icon,info,flags&BDAF_SHOW);
			}

			// Mark as ok if we have an icon
			if (object) object->bdo_flags|=BDOF_OK;
		}
	}

	// Unlock AppList
	UnlockAppList();

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;)
	{
		BackdropObject *next=(BackdropObject *)object->bdo_node.ln_Succ;

		// AppIcon?
		if (object->bdo_type==BDO_APP_ICON)
		{
			// Marked as ok?
			if (object->bdo_flags&BDOF_OK) object->bdo_flags&=~BDOF_OK;

			// Not marked; remove icon
			else
			{
				// Erase icon
				if (flags&BDAF_SHOW) backdrop_erase_icon(info,object,0);

				// Remove from list
				backdrop_remove_object(info,object);
			}
		}

		// Get next
		object=next;
	}

	// Unlock backdrop list
	if (flags&BDAF_LOCK) unlock_listlock(&info->objects);
}


// Add an AppIcon backdrop object
BackdropObject *backdrop_add_appicon(AppEntry *appicon,BackdropInfo *info,short flags)
{
	BackdropObject *object;
	leftout_record *left;

	// Lock backdrop list
	if (flags&BDAF_LOCK) lock_listlock(&info->objects,0);

	// See if AppIcon is already in list
	if (object=backdrop_find_appicon(info,appicon))
	{
		// Image changed?
		if (flags&BDAF_CHANGE)
		{
			// Has name changed?
			if (strcmp(object->bdo_name,appicon->ae_text))
			{
				// Erase object label
				backdrop_render_object(info,object,BRENDERF_CLEAR|BRENDERF_CLIP|BRENDERF_LABEL);

				// Get new name
				strcpy(object->bdo_name,appicon->ae_text);
			}

			// Update locked state
			if (appicon->ae_flags&APPENTF_LOCKED) object->bdo_flags|=BDOF_LOCKED;
			else object->bdo_flags&=~BDOF_LOCKED;

			// Update busy state
			if (appicon->ae_flags&APPENTF_BUSY) object->bdo_flags|=BDOF_BUSY;
			else object->bdo_flags&=~BDOF_BUSY;

			// Update ghosted state
			if (appicon->ae_flags&APPENTF_GHOSTED) object->bdo_flags|=BDOF_GHOSTED;
			else object->bdo_flags&=~BDOF_GHOSTED;

			// Did image change?
			if (flags&BDAF_NEW_IMAGE)
			{
				// Need new masks?
				if (!backdrop_icon_border(object))
				{
					// Get new masks
					backdrop_get_masks(object);
				}
			}

			// Show image
			backdrop_render_object(info,object,BRENDERF_CLIP);
		}
	}

	// Unlock backdrop list
	if (flags&BDAF_LOCK) unlock_listlock(&info->objects);

	// Already in list?
	if (object || flags&BDAF_CHANGE) return object;

	// Valid text?
	if (appicon->ae_text && *appicon->ae_text)
	{
		// Lock position list
		lock_listlock(&GUI->positions,FALSE);

		// Look for entry for icon
		for (left=(leftout_record *)&GUI->positions.list.lh_Head;
			left->node.ln_Succ;
			left=(leftout_record *)left->node.ln_Succ)
		{
			// Leftout?
			if (left->node.ln_Type==PTYPE_APPICON)
			{
				// Match this icon?
				if (strcmp(left->icon_label,appicon->ae_text)==0)
				{
					// Set position in icon
					((struct DiskObject *)appicon->ae_object)->do_CurrentX=left->icon_x;
					((struct DiskObject *)appicon->ae_object)->do_CurrentY=left->icon_y;
					break;
				}
			}
		}

		// Unlock position list
		unlock_listlock(&GUI->positions);
	}

	// Allocate a new object
	if (object=backdrop_new_object(
		info,
		appicon->ae_text,
		0,
		BDO_APP_ICON))
	{
		// Lock backdrop list
		if (flags&BDAF_LOCK) lock_listlock(&info->objects,1);

		// Store icon and owner
		object->bdo_icon=appicon->ae_object;
		object->bdo_misc_data=(ULONG)appicon;

		// Background colour?
		if (((AppEntry *)appicon)->ae_flags&APPENTF_BACKGROUND)
		{
			// Store background colour
			object->bdo_size=((AppEntry *)appicon)->ae_data;
			object->bdo_flags|=BDOF_BACKGROUND;
		}

		// Locked?
		if (((AppEntry *)appicon)->ae_flags&APPENTF_LOCKED)
			object->bdo_flags|=BDOF_LOCKED;

		// Special?
		if (((AppEntry *)appicon)->ae_flags&APPENTF_SPECIAL)
			object->bdo_flags|=BDOF_SPECIAL;

		// Fix size and position
		backdrop_get_icon(info,object,GETICON_CD);

		// Add to backdrop list
		AddTail(&info->objects.list,&object->bdo_node);

		// Unlock backdrop list
		if (flags&BDAF_LOCK) unlock_listlock(&info->objects);

		// Refresh display?
		if (flags&BDAF_SHOW)
		{
			// Position new icon
			backdrop_place_object(info,object);

			// Show desktop
			backdrop_show_objects(info,BDSF_RECALC);
		}
	}

	return object;
}


// Remove an AppIcon backdrop object
void backdrop_rem_appicon(BackdropInfo *info,AppEntry *icon)
{
	BackdropObject *object;
	BOOL redraw=0;

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// Find AppIcon
	if (object=backdrop_find_appicon(info,icon))
	{
		// Erase object
		backdrop_erase_icon(info,object,0);

		// Remove object
		backdrop_remove_object(info,object);
		redraw=1;
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Recalculate backdrop
	if (redraw) backdrop_calc_virtual(info);
}


// Look for an AppIcon (list must be locked)
BackdropObject *backdrop_find_appicon(BackdropInfo *info,AppEntry *appicon)
{
	BackdropObject *object;

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Check if we already have this appicon
		if (object->bdo_type==BDO_APP_ICON &&
			object->bdo_misc_data==(ULONG)appicon) return object;
	}

	return 0;
}


// Build an AppMessage from dropped objects
GalileoAppMessage *backdrop_appmessage(BackdropInfo *info,BOOL need_obj)
{
	GalileoAppMessage *msg;
	BackdropObject *object,*first;
	short arg=0,count=0,beep=0;

	// Go through backdrop list, count selections
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Set beep if wrong type
		if (object->bdo_type==BDO_GROUP ||
			object->bdo_type==BDO_BAD_DISK) beep=1;

		// Selected?
		else if (object->bdo_state) ++count;
	}

	// No objects?
	if (!count && need_obj)
	{
		// Beep?
		if (beep) DisplayBeep(info->window->WScreen);
		return 0;
	}

	// Allocate AppMessage
	if (!(msg=AllocAppMessage(global_memory_pool,GUI->appmsg_port,count)))
		return 0;

	// Set icon flag
	msg->ga_Flags|=GAPPF_ICON_DROP;

	// Pointer to first selected object
	if (first=info->last_sel_object)
	{
		// Save drag offset
		msg->ga_DragOffset.x=first->bdo_image_rect.MinX-first->bdo_pos.Left+first->bdo_drag_x_offset-info->offset_x;
		msg->ga_DragOffset.y=first->bdo_image_rect.MinY-first->bdo_pos.Top+first->bdo_drag_y_offset-info->offset_y;
	}

	// Go through backdrop list, fill in arguments
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ && arg<count;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Selected?
		if (object->bdo_state &&
			object->bdo_type!=BDO_BAD_DISK &&
			object->bdo_type!=BDO_GROUP)
		{
			// Got position array?
			if (first)
			{
				// Store object position relative to first object
				msg->ga_DropPos[arg].x=object->bdo_pos.Left-first->bdo_pos.Left;
				msg->ga_DropPos[arg].y=object->bdo_pos.Top-first->bdo_pos.Top;
			}

			// AppIcon?
			if (object->bdo_type==BDO_APP_ICON)
			{
				// Copy name
				SetWBArg(msg,arg,0,object->bdo_name,global_memory_pool);
			}

			// Is object a directory?
			else
			if (object->bdo_icon->do_Type==WBDRAWER || object->bdo_icon->do_Type==WBGARBAGE)
			{
				BPTR old,lock;

				// Get icon lock
				if (lock=backdrop_icon_lock(object))
				{
					// Change directory
					old=CurrentDir(lock);

					// Lock sub-directory
					if (!(msg->ga_Msg.am_ArgList[arg].wa_Lock=Lock(object->bdo_name,ACCESS_READ)))
					{
						char *name;

						// Couldn't lock directory, so steal lock for parent
						if (name=AllocVec(strlen(object->bdo_name)+8,0))
						{
							msg->ga_Msg.am_ArgList[arg].wa_Lock=lock;
							lock=0;

							// Get filename
							strcpy(name,object->bdo_name);
							strcat(name,".info");
							SetWBArg(msg,arg,0,name,global_memory_pool);
						}
					}

					// Restore directory
					CurrentDir(old);
					if (lock) UnLock(lock);
				}
			}

			// Otherwise
			else
			{
				// Duplicate lock (unless we've already got one)
				if (!msg->ga_Msg.am_ArgList[arg].wa_Lock)
					msg->ga_Msg.am_ArgList[arg].wa_Lock=backdrop_icon_lock(object);

				// If not a disk, copy filename
				if (object->bdo_icon->do_Type!=WBDISK)
				{
					SetWBArg(msg,arg,0,object->bdo_name,global_memory_pool);
				}
			}

			// If no filename given, create a dummy one
			if (!msg->ga_Msg.am_ArgList[arg].wa_Name)
				SetWBArg(msg,arg,0,0,global_memory_pool);
			++arg;
		}
	}

	// Set flags in message
	set_appmsg_data(msg,(ULONG)info->lister,DROPF_ICON_MODE,0);
	return msg;
}


// Drop stuff on an AppWindow (nb: will be in Forbid() when we get here)
void backdrop_drop_appwindow(BackdropInfo *info,struct AppWindow *appwindow,short x,short y)
{
	GalileoAppMessage *msg;
	struct MsgPort *port;

	// Build AppMessage
	if (msg=backdrop_appmessage(info,1))
	{
		// Complete message
		msg->ga_Msg.am_Type=MTYPE_APPWINDOW;
		msg->ga_Msg.am_MouseX=x;
		msg->ga_Msg.am_MouseY=y;
		port=WB_AppWindowData(
			appwindow,
			(ULONG *)&msg->ga_Msg.am_ID,
			(ULONG *)&msg->ga_Msg.am_UserData);

		// Send the message
		PutMsg(port,(struct Message *)msg);
	}
}


// Send message to an AppIcon
void backdrop_appicon_message(BackdropObject *object,unsigned short flags)
{
	struct AppSnapshotMsg *msg;
	struct MsgPort *port;

	// Allocate message
	if (!(msg=AllocMemH(global_memory_pool,sizeof(struct AppSnapshotMsg))))
		return;

	// Fill out message
	msg->ap_msg.am_Message.mn_ReplyPort=GUI->appmsg_port;
	msg->ap_msg.am_Version=AM_VERSION;
	CurrentTime(&msg->ap_msg.am_Seconds,&msg->ap_msg.am_Micros);

	// Message type
	msg->ap_msg.am_Type=MTYPE_APPSNAPSHOT;

	// Set icon position
	msg->position_x=object->bdo_pos.Left;
	msg->position_y=object->bdo_pos.Top;

	// Adjust for border
	if (backdrop_icon_border(object))
	{
		msg->position_x-=ICON_BORDER_X;
		msg->position_y-=ICON_BORDER_Y_TOP;
	}

	// Adjust 
	// Menu operation?
	if (flags&BAPPF_MENU)
	{
		// Get ID
		msg->id=BAPPF_ID(flags);

		// Set flags
		msg->flags|=APPSNAPF_MENU;
		if (flags&BAPPF_HELP) msg->flags|=APPSNAPF_HELP;
	}

	// Close?
	else
	if (flags&BAPPF_CLOSE) msg->flags|=APPSNAPF_CLOSE;

	// Unsnapshot?
	else
	if (flags&BAPPF_UNSNAPSHOT) msg->flags|=APPSNAPF_UNSNAPSHOT;

	// Info?
	else
	if (flags&BAPPF_INFO) msg->flags|=APPSNAPF_INFO;

	// Get AppInfo
	port=WB_AppWindowData(
		(struct AppWindow *)object->bdo_misc_data,
		&msg->ap_msg.am_ID,
		&msg->ap_msg.am_UserData);

	// Send the message
	PutMsg(port,(struct Message *)msg);
}
