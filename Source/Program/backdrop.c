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

// Create a new backdrop handle
BackdropInfo *backdrop_new(IPCData *ipc,ULONG flags)
{
	BackdropInfo *info;

	// Allocate structure
	if (!(info=AllocVec(sizeof(BackdropInfo),MEMF_CLEAR)))
		return 0;

	// Create memory pool
	info->memory=NewMemHandle(4096,1024,MEMF_CLEAR);

	// Initialise stuff
	InitListLock(&info->objects,0);
	InitSemaphore(&info->window_lock);
	InitSemaphore(&info->idcmp_lock);
	InitSemaphore(&info->icon_lock);
	NewList(&info->boopsi_list);
	info->ipc=ipc;
	info->flags=flags;

	return info;
}


// Free backdrop info
void backdrop_free(BackdropInfo *info)
{
	if (info)
	{
		// Free icons
		backdrop_free_list(info);

		// Free font
		if (info->font) CloseFont(info->font);

		// Free memory
		FreeMemHandle(info->memory);

		// Free structure
		FreeVec(info);
	}
}


// Initialise backdrop info
void backdrop_init_info(
	BackdropInfo *info,
	struct Window *window,
	short no_icons)
{
	// Backdrop window pointer
	info->window=window;

	// Initialise size
	info->size.MinX=window->BorderLeft+info->left_border;
	info->size.MinY=window->BorderTop+info->top_border;
	info->size.MaxX=window->Width-window->BorderRight-info->right_border-1;
	info->size.MaxY=window->Height-window->BorderBottom-info->bottom_border-1;

	// Not re-initialising?
	if (no_icons!=-1)
	{
		BackdropObject *icon;

		// Clone rastport
		info->rp=*window->RPort;

		// Get font
		backdrop_get_font(info);

		// If not backdrop window, add scroll bars
		if (!(window->Flags&WFLG_BACKDROP))
		{
			// Create scroll bars
			if (AddScrollBars(window,&info->boopsi_list,GUI->draw_info,SCROLL_VERT|SCROLL_HORIZ|SCROLL_NOIDCMP))
			{
				// Get scrollers
				info->vert_scroller=FindBOOPSIGadget(&info->boopsi_list,GAD_VERT_SCROLLER);
				info->horiz_scroller=FindBOOPSIGadget(&info->boopsi_list,GAD_HORIZ_SCROLLER);

				// Add to window
				AddGList(window,info->vert_scroller,-1,-1,0);
				RefreshGList(info->vert_scroller,window,0,-1);
			}
		}

		// Get clip region
		info->clip_region=NewRegion();
		info->temp_region=NewRegion();

		// Allocate notification port
		if (info->notify_port=CreateMsgPort())
		{
			// Add notify request
			info->notify_req=AddNotifyRequest(GN_WRITE_ICON,0,info->notify_port);
		}

		// Lock icon list
		lock_listlock(&info->objects,0);

		// Remap existing icons
		for (icon=(BackdropObject *)info->objects.list.lh_Head;
			icon->bdo_node.ln_Succ;
			icon=(BackdropObject *)icon->bdo_node.ln_Succ)
		{
			// Remap the icon
			if (RemapIcon(icon->bdo_icon,info->window->WScreen,0))
			{
				// Had icon never been remapped before?
				if (!(icon->bdo_flags&BDOF_REMAPPED))
				{
					// Get new object size, etc
					backdrop_get_icon(info,icon,GETICON_KEEP|GETICON_POS_ONLY|GETICON_SAVE_POS|GETICON_REMAP);

					// Get new masks
					backdrop_get_masks(icon);
				}
			}
			
			// Set flag to say we've been remapped
			icon->bdo_flags|=BDOF_REMAPPED;
		}

		// Unlock icon list
		unlock_listlock(&info->objects);
	}

	// Install clip
	if (info->clip_region)
		backdrop_install_clip(info);
}


// Free icon remapping
void backdrop_free_remap(BackdropInfo *info,struct Window *window)
{
	BackdropObject *icon;

	// Lock icon list
	lock_listlock(&info->objects,0);

	// Free icon remapping
	for (icon=(BackdropObject *)info->objects.list.lh_Head;
		icon->bdo_node.ln_Succ;
		icon=(BackdropObject *)icon->bdo_node.ln_Succ)
	{
		// Remap the icon
		RemapIcon(icon->bdo_icon,(window)?window->WScreen:0,1);
	}

	// Unlock icon list
	unlock_listlock(&info->objects);
}


// Free backdrop info stuff
void backdrop_free_info(BackdropInfo *info)
{
	// Free boopsi objects
	BOOPSIFree(&info->boopsi_list);

	// Free regions
	if (info->clip_region)
	{
		DisposeRegion(info->clip_region);
		info->clip_region=0;
	}
	if (info->temp_region)
	{
		DisposeRegion(info->temp_region);
		info->temp_region=0;
	}

	// Clear gadget pointers
	info->horiz_scroller=0;
	info->vert_scroller=0;
	info->window=0;

	// Free notification port
	if (info->notify_port)
	{
		struct Message *msg;

		// Remove request
		if (info->notify_req)
		{
			RemoveNotifyRequest(info->notify_req);
			info->notify_req=0;
		}

		// Clear port of messages
		while (msg=GetMsg(info->notify_port))
			ReplyFreeMsg(msg);

		// Delete port
		DeleteMsgPort(info->notify_port);
		info->notify_port=0;
	}
}


// Allocate a new object
BackdropObject *backdrop_new_object(
	BackdropInfo *info,
	char *name,
	char *extra,
	short type)
{
	BackdropObject *object;
	short len=0;

	// Extra space?
	if (extra)
	{
		if (!*extra) extra=0;
		else len=strlen(extra)+1;
	}

	// Allocate object
	if (!(object=AllocMemH(info->memory,sizeof(BackdropObject)+GUI->def_filename_length+1+len)))
		return 0;

	// Get name pointer
	object->bdo_name=(char *)(object+1);

	// Set name
	object->bdo_node.ln_Name=object->bdo_name;
	if (name) stccpy(object->bdo_name,name,GUI->def_filename_length);

	// Set type
	object->bdo_type=type;

	// Any extra?
	if (extra)
	{
		// Copy extra
		object->bdo_device_name=object->bdo_name+GUI->def_filename_length+1;
		strcpy(object->bdo_device_name,extra);
	}

	return object;
}


// Free list of backdrop objects
void backdrop_free_list(BackdropInfo *info)
{
	BackdropObject *object;

	// Invalid info?
	if (!info) return;

	// Lock backdrop list
	lock_listlock(&info->objects,1);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;)
	{
		BackdropObject *next=(BackdropObject *)object->bdo_node.ln_Succ;

		// Remove object
		backdrop_remove_object(info,object);

		// Get next object
		object=next;
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}


// Remove an object
void backdrop_remove_object(BackdropInfo *info,BackdropObject *object)
{
	short a;

	// Remove from list
	Remove(&object->bdo_node);

	// Not an appicon?
	if (object->bdo_type!=BDO_APP_ICON)
	{
		if (object->bdo_icon)
		{
			// Free icon remapping
			RemapIcon(object->bdo_icon,(info->window)?info->window->WScreen:0,1);

			// Free icon
			FreeCachedDiskObject(object->bdo_icon);
		}
	}

	// Mask plane?
	for (a=0;a<2;a++)
	if (object->bdo_image_mask[a])
			FreeVec(object->bdo_image_mask[a]);

	// Free object
	FreeMemH(object);
}


// Get icon for an object
void backdrop_get_icon(BackdropInfo *info,BackdropObject *object,short flags)
{
	short x,y,border_x=0,border_y_top=0,border_y_bottom=0;
	BOOL new_icon=1;

	// Keeping icon?
	if (flags&GETICON_KEEP) new_icon=0;

	// Not AppIcon?
	if (object->bdo_type!=BDO_APP_ICON && !(flags&GETICON_POS_ONLY))
	{
		BPTR lock=0,old=0;

		// Already got icon?
		if (object->bdo_icon && !(flags&GETICON_KEEP))
		{
			// Free icon remapping
			RemapIcon(object->bdo_icon,(info->window)?info->window->WScreen:0,1);

			// Free icon
			FreeCachedDiskObject(object->bdo_icon);
			object->bdo_icon=0;

			// We'll be getting a new one
			new_icon=1;
		}

		// Bad disk?
		if (object->bdo_type==BDO_BAD_DISK)
		{
			// Get default disk
			if (!object->bdo_icon && !(object->bdo_icon=GetCachedDefDiskObject(WBKICK)))
				object->bdo_icon=GetCachedDefDiskObject(WBDISK);
		}

		// Default directory icon?
		else
		if (flags&GETICON_DEFDIR)
		{
			// Get default drawer icon
			if (!object->bdo_icon) object->bdo_icon=GetCachedDefDiskObject(WBDRAWER);
		}

		// Get lock on directory
		else
		if (!(flags&GETICON_CD) || (lock=backdrop_icon_lock(object)))
		{
			// Go to icon directory
			if (lock) old=CurrentDir(lock);

			// Disk?
			if (object->bdo_type==BDO_DISK)
			{
				// No icon already?
				if (!object->bdo_icon)
				{
					BOOL ok=1;
					Cfg_Filetype *type=0;
					char name[256],*ptr;

					// Find a filetype-defined icon
					if (object->bdo_device_name &&
						(type=filetype_identify(object->bdo_device_name,FTTYPE_ICON,0,0)))
					{
						// Copy icon path, strip .info
						strcpy(name,type->icon_path);
						if (ptr=isicon(name)) *ptr=0;

						// Over-ride flag set in filetype?
						if (type->type.flags&FILETYPEF_OVERRIDE)
						{
							// Try for filetype icon first
							object->bdo_icon=GetCachedDiskObject(name,0);
						}
					}

					// Don't have icon yet?
					if (!object->bdo_icon)
					{
						// Is this a MSDOS disk?
						if (object->bdo_misc_data==ID_MSDOS_DISK)
						{
							// See if icon actually exists
							if (type && !(SetProtection("Disk.info",FIBF_ARCHIVE)) &&
								IoErr()==ERROR_OBJECT_NOT_FOUND)
							{
								// We'll use default icon
								ok=0;
							}
						}
						
						// Get disk icon
						if (ok)
							object->bdo_icon=GetCachedDiskObject("Disk",0);
					}

					// Got icon?
					if (object->bdo_icon)
					{
#ifdef _DEBUG_ICONS
			KPrintF("backdrop_get_icon  path: %s name: %s \n", object->bdo_path, object->bdo_name);
			KPrintF("backdrop_get_icon  do_Gadget.Width: %ld do_Gadget.Height %ld  \n", object->bdo_icon->do_Gadget.Width, object->bdo_icon->do_Gadget.Height );
			if (object->bdo_icon->do_Gadget.GadgetRender)
			    KPrintF("backdrop_get_icon  do_Gadget.GadgetRender->Width: %ld do_Gadget.GadgetRender->Height %ld \n", ((struct Image *)object->bdo_icon->do_Gadget.GadgetRender)->Width, ((struct Image *)object->bdo_icon->do_Gadget.GadgetRender)->Height );
#endif
			// If it's a drawer icon, turn it into a disk
						if (object->bdo_icon->do_Type==WBDRAWER)
							object->bdo_icon->do_Type=WBDISK;

						// Check it is for a disk
						if (object->bdo_icon->do_Type!=WBDISK)
						{
							// It's not, free it and use default
							FreeCachedDiskObject(object->bdo_icon);
							object->bdo_icon=0;
						}
					}

					// Still no icon? Get default
					if (!object->bdo_icon && !(flags&GETICON_FAIL))
					{
						// Got type-defined?
						if (type)
						{
							// Try for filetype icon
							object->bdo_icon=GetCachedDiskObject(name,0);
						}

						// Still none? Get default
						if (!object->bdo_icon)
							object->bdo_icon=GetCachedDefDiskObject(WBDISK);

						// Set fake flag
						if (object->bdo_icon)
							object->bdo_flags|=BDOF_FAKE_ICON;
					}
				}
			}

			// Left out or group
			else
			if (object->bdo_type==BDO_LEFT_OUT || object->bdo_type==BDO_GROUP)
			{
				// Try for icon
				if (!object->bdo_icon)
				{
					short fake=0;

					// Want real icon?
					if (flags&GETICON_FAIL) object->bdo_icon=GetCachedDiskObject(object->bdo_name,0);

					// Get default icon if fails
					else object->bdo_icon=GetProperIcon(object->bdo_name,&fake,0);

					// Ended up fake?
					if (fake)
					{
						// Set flag
						object->bdo_flags|=BDOF_FAKE_ICON;
					}
				}

				// Got group icon?
				if (object->bdo_icon && object->bdo_type==BDO_GROUP)
				{
					// Auto-open group?
					if (FindToolType(object->bdo_icon->do_ToolTypes,"OPEN"))
						object->bdo_flags|=BDOF_AUTO_OPEN;
				}
			}

			// Had a lock?
			if (lock)
			{
				// Got icon?
				if (object->bdo_icon)
				{
					// Clear custom position flag
					object->bdo_flags&=~BDOF_CUSTOM_POS;

#ifdef DISTINCT_OK
					// Main desktop, distinct positions?
					if (info->flags&BDIF_MAIN_DESKTOP &&
						environment->env->desktop_flags&DESKTOPF_DISTINCT)
					{
						char path[256];

						// Get icon path
						if (desktop_icon_path(object,path,256,lock))
						{
							// See if position is available
							if (desktop_find_icon(path,&object->bdo_custom_pos))
							{
								// Set "custom position" flag
								object->bdo_flags|=BDOF_CUSTOM_POS;
							}
						}
					}
#endif
				}

				// Restore current dir
				CurrentDir(old);

				// Unlock object lock
				UnLock(lock);
			}
		}

		// Failed to even get a lock; get default icon if a disk
		else
		if (object->bdo_type==BDO_DISK)
		{
			// Get default icon
			if (!object->bdo_icon && (object->bdo_icon=GetCachedDefDiskObject(WBDISK)))
				object->bdo_flags|=BDOF_FAKE_ICON;
		}
	}

	// Got an icon?
	if (object->bdo_icon)
	{
		// Ended up fake?
		if (object->bdo_flags&BDOF_FAKE_ICON)
		{
			// Make sure default icon has no position
			if (object->bdo_icon)
			{
				// Clear 'position ok' flag, set invalid position
				SetIconFlags(object->bdo_icon,GetIconFlags(object->bdo_icon)&~ICONF_POSITION_OK);
				SetIconPosition(object->bdo_icon,-1,-1);
				object->bdo_icon->do_CurrentX=NO_ICON_POSITION;
				object->bdo_icon->do_CurrentY=NO_ICON_POSITION;
			}
		}

		// If this is a group, make sure icon is a drawer
		if (object->bdo_type==BDO_GROUP)
			object->bdo_icon->do_Type=WBDRAWER;

		// Is it a new icon?
		if (new_icon)
		{
			// Window open?
			if (info->window)
			{
				// Remap the icon
				RemapIcon(object->bdo_icon,info->window->WScreen,0);

				// Set flag
				object->bdo_flags|=BDOF_REMAPPED;
			}
		}

		// Transparent icon?
		if (!backdrop_icon_border(object))
		{
			border_x=0;
			border_y_top=0;
			border_y_bottom=0;
		}
		else
		{
			border_x=ICON_BORDER_X;
			border_y_top=ICON_BORDER_Y_TOP;
			border_y_bottom=ICON_BORDER_Y_BOTTOM;
		}

		// No label?
		if ((GetIconFlags(object->bdo_icon)&ICONF_NO_LABEL) && !(environment->env->desktop_flags&DESKTOPF_NO_NOLABELS))
			object->bdo_flags|=BDOF_NO_LABEL;
		else
			object->bdo_flags&=~BDOF_NO_LABEL;
	}

	// No icon, or no size stuff?
	if (!object->bdo_icon || flags&GETICON_NO_POS) return;

	// Get masks
	if (!(flags&GETICON_KEEP)) backdrop_get_masks(object);

	// Get object size
	object->bdo_pos.Width=object->bdo_icon->do_Gadget.Width;
	object->bdo_pos.Height=object->bdo_icon->do_Gadget.Height;

	// (Re)position?
	if (!(flags&GETICON_SAVE_POS))
	{
		// No position initially
		object->bdo_flags|=BDOF_NO_POSITION;

		// Auto position?
		if (object->bdo_flags&BDOF_AUTO_POSITION)
		{
			return;
		}

		// Custom position?
		else
		if (object->bdo_flags&(BDOF_CUSTOM_POS|BDOF_LEFTOUT_POS))
		{
			// Get custom position
			x=(object->bdo_custom_pos>>16)&0xffff;
			y=object->bdo_custom_pos&0xffff;
		}

		// Valid position?
		else
		if (object->bdo_type!=BDO_APP_ICON &&
			(!(environment->env->display_options&DISPOPTF_ICON_POS) || environment->env->display_options&DISPOPTF_REMGALILEOPOS) &&
			GetIconFlags(object->bdo_icon)&ICONF_POSITION_OK)
		{
			// Get position
			GetIconPosition(object->bdo_icon,&x,&y);

			// No position?
			if (x==-1 && y==-1) return;
		}

		// Otherwise, get original Workbench position
		else
		if (object->bdo_icon->do_CurrentX!=NO_ICON_POSITION)
		{
			// Get position
			x=object->bdo_icon->do_CurrentX+WBICONMAGIC_X;
			y=object->bdo_icon->do_CurrentY+WBICONMAGIC_Y;

			// Pretend we have an Opus snapshot now (won't matter unless we Snapshot in which case we will anyway)
			SetIconPosition(object->bdo_icon,x,y);
			SetIconFlags(object->bdo_icon,GetIconFlags(object->bdo_icon)|ICONF_POSITION_OK);
		}

		// No position
		else
		{
			return;
		}
	}

	// Keep old position
	else
	{
		// Remapping?
		if (flags&GETICON_REMAP)
		{
			// Save old position
			x=object->bdo_pos.Left;
			y=object->bdo_pos.Top;
		}

		// Normal
		else
		{
			// Get old position
			x=object->bdo_image_rect.MinX+border_x-(info->size.MinX-info->offset_x);
			y=object->bdo_image_rect.MinY+border_y_top-(info->size.MinY-info->offset_y);

			// Store position
			object->bdo_pos.Left=x;
			object->bdo_pos.Top=y;
			object->bdo_flags&=~BDOF_NO_POSITION;
		}
	}

	// Get full-size rectangle
	backdrop_show_rect(info,object,x-border_x,y-border_y_top);

	// Copy to full-size rectangle
	object->bdo_full_size=object->bdo_show_rect;

	// Adjust show rectangle
	object->bdo_show_rect.MinX+=info->size.MinX-info->offset_x;
	object->bdo_show_rect.MinY+=info->size.MinY-info->offset_y;
	object->bdo_show_rect.MaxX+=info->size.MinX-info->offset_x;
	object->bdo_show_rect.MaxY+=info->size.MinY-info->offset_y;

	// Place object?
	if (!(flags&GETICON_SAVE_POS))
	{
		// Store position
		object->bdo_pos.Left=x;
		object->bdo_pos.Top=y;
		object->bdo_flags&=~BDOF_NO_POSITION;

		// Place it
		backdrop_place_object(info,object);
	}
}


// Find an object in the list
BackdropObject *find_backdrop_object(
	BackdropInfo *info,
	BackdropObject *look)
{
	BackdropObject *object;

	if (!look) return 0;

	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		if (object==look) return look;
	}

	return 0;
}


// Calculate the virtual window size
void backdrop_calc_virtual(BackdropInfo *info)
{
	struct Rectangle rect;
	BOOL virtual_ok=0;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Lock window
	GetSemaphore(&info->window_lock,SEMF_EXCLUSIVE,0);

	// Non-backdrop window open?
	if (info->window && !(info->window->Flags&WFLG_BACKDROP))
		virtual_ok=1;

	// Otherwise, reset offset
	else
	{
		info->offset_x=0;
		info->offset_y=0;
	}

	// Ok to calculate?
	if (virtual_ok)
	{
		// Calculate size
		backdrop_calc_virtual_size(info,&rect);
	}

	// Otherwise
	else
	{
		// Virtual size is  physical size
		rect.MinX=0;
		rect.MinY=0;
		rect.MaxX=RECTWIDTH(&info->size)-1;
		rect.MaxY=RECTHEIGHT(&info->size)-1;
	}

	// Store virtual size
	info->virt_width=RECTWIDTH(&rect);
	info->virt_height=RECTHEIGHT(&rect);
	info->area=rect;

	// Valid non-backdrop window open?
	if (virtual_ok &&
		(!info->lister || !(info->lister->more_flags&LISTERF_TITLEBARRED)))
	{
		// Update scrollers
		if (info->horiz_scroller)
		{
			SetGadgetAttrs(info->horiz_scroller,info->window,0,
				PGA_Total,info->virt_width,
				PGA_Visible,RECTWIDTH(&info->size),
				PGA_Top,info->offset_x-rect.MinX,
				TAG_END);
		}
		if (info->vert_scroller)
		{
			SetGadgetAttrs(info->vert_scroller,info->window,0,
				PGA_Total,info->virt_height,
				PGA_Visible,RECTHEIGHT(&info->size),
				PGA_Top,info->offset_y-rect.MinY,
				TAG_END);
		}
	}

	// Unlock window
	FreeSemaphore(&info->window_lock);

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Update position from sliders
	if (virtual_ok)
		backdrop_pos_slider(info,SLIDER_HORZ|SLIDER_VERT);
}


// Calculate size of icon area
void backdrop_calc_virtual_size(BackdropInfo *info,struct Rectangle *rect)
{
	BackdropObject *object;

	// Virtual size starts out being physical size
	rect->MinX=0;
	rect->MinY=0;
	rect->MaxX=RECTWIDTH(&info->size)-1;
	rect->MaxY=RECTHEIGHT(&info->size)-1;

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Valid icon, with valid position?
		if (object->bdo_icon && !(object->bdo_flags&BDOF_NO_POSITION))
		{
			// See if icon is outside current window
			if (object->bdo_full_size.MinX<rect->MinX)
				rect->MinX=object->bdo_full_size.MinX;
			if (object->bdo_full_size.MinY<rect->MinY)
				rect->MinY=object->bdo_full_size.MinY;
			if (object->bdo_full_size.MaxX>rect->MaxX)
				rect->MaxX=object->bdo_full_size.MaxX;
			if (object->bdo_full_size.MaxY>rect->MaxY)
				rect->MaxY=object->bdo_full_size.MaxY;
		}
	}
}


// Scroll backdrop
void backdrop_scroll(BackdropInfo *info,short dx,short dy)
{
	short old_x,old_y;

	// Lock window
	GetSemaphore(&info->window_lock,SEMF_EXCLUSIVE,0);

	// Window open?
	if (info->window &&
		(!info->lister || !(info->lister->more_flags&LISTERF_TITLEBARRED)))
	{
		// Backdrop window?
		if (info->window->Flags&WFLG_BACKDROP)
		{
			// 0,0 offset
			info->offset_x=0;
			info->offset_y=0;
		}

		// Otherwise
		else
		{
			// Save old offsets
			old_x=info->offset_x;
			old_y=info->offset_y;

			// Horizontal scroll?
			if (dx)
			{
				// Shift offset
				old_x+=dx*32;

				// Bounds check
				if (old_x+RECTWIDTH(&info->size)>=info->area.MaxX)
					old_x=info->area.MaxX-(info->size.MaxX-info->size.MinX);
				if (old_x<info->area.MinX)
					old_x=info->area.MinX;
			}

			// Vertical scroll?
			if (dy)
			{
				// Shift offset
				old_y+=dy*32;

				// Bounds check
				if (old_y+RECTHEIGHT(&info->size)>=info->area.MaxY)
					old_y=info->area.MaxY-(info->size.MaxY-info->size.MinY);
				if (old_y<info->area.MinY)
					old_y=info->area.MinY;
			}


			// Horizontal change?
			if (old_x!=info->offset_x)
			{
				// Update scroller
				SetGadgetAttrs(info->horiz_scroller,info->window,0,
					PGA_Top,old_x-info->area.MinX,
					TAG_END);
			}

			// Vertical change?
			if (old_y!=info->offset_y)
			{
				// Update scroller
				SetGadgetAttrs(info->vert_scroller,info->window,0,
					PGA_Top,old_y-info->area.MinY,
					TAG_END);
			}

			// Need to scroll?
			if (old_x!=info->offset_x || old_y!=info->offset_y)
			{
				backdrop_pos_slider(info,SLIDER_HORZ|SLIDER_VERT);
			}
		}
	}

	// Unlock window
	FreeSemaphore(&info->window_lock);
}


// Position from sliders
void backdrop_pos_slider(BackdropInfo *info,short which)
{
	short old_x,old_y;

	// Lock window
	GetSemaphore(&info->window_lock,SEMF_EXCLUSIVE,0);

	// Non-backdrop window open?
	if (info->window && !(info->window->Flags&WFLG_BACKDROP) &&
		(!info->lister || !(info->lister->more_flags&LISTERF_TITLEBARRED)))
	{
		// Store old positions
		old_x=info->offset_x;
		old_y=info->offset_y;

		// Horizontal?
		if (which&SLIDER_HORZ)
		{
			// Get horizontal position
			GetAttr(PGA_Top,info->horiz_scroller,(ULONG *)&info->offset_x);
			info->offset_x+=info->area.MinX;
		}

		// Vertical?
		if (which&SLIDER_VERT)
		{
			// Get vertical position
			GetAttr(PGA_Top,info->vert_scroller,(ULONG *)&info->offset_y);
			info->offset_y+=info->area.MinY;
		}

		// Has position changed?
		if (old_x!=info->offset_x || old_y!=info->offset_y)
		{
			// Real-time icon scrolling?
			if (environment->env->display_options&DISPOPTF_REALTIME_SCROLL)
			{
				// Scroll
				backdrop_scroll_objects(info,info->offset_x-old_x,info->offset_y-old_y);
			}

			// Normal
			else
			{
				// Refresh
				backdrop_show_objects(info,BDSF_CLEAR);
			}
		}
	}

	// Unlock window
	FreeSemaphore(&info->window_lock);
}


// Return a lock on an icon
BPTR backdrop_icon_lock(BackdropObject *object)
{
	BPTR lock=0;

	// Bad disk?
	if (object->bdo_type==BDO_BAD_DISK) return 0;

	// Is object a disk?
	else
	if (object->bdo_type==BDO_DISK &&
		object->bdo_device_name)
	{
		// Lock device
		if (lock=Lock(object->bdo_device_name,ACCESS_READ))
		{
			// Cache drawer?
			if (object->bdo_flags&BDOF_CACHE)
			{
				BPTR parent;

				// Use parent
				if (parent=ParentDir(lock))
				{
					// Free first lock
					UnLock(lock);
					lock=parent;
				}
			}
		}
	}

	// Otherwise, lock parent directory
	else
	if (object->bdo_path) lock=Lock(object->bdo_path,ACCESS_READ);

	// And if no parent directory, lock volume name if it's a disk
	else
	if (object->bdo_type==BDO_DISK)
	{
		char name[80];

		// Build volume name
		lsprintf(name,"%s:",object->bdo_name);

		// Lock volume
		lock=Lock(name,ACCESS_READ);
	}

	return lock;
}


// Get all objects
void backdrop_get_objects(BackdropInfo *info,short flags)
{
	// Getting anything at all?
	if (flags&BGOF_ALL)
	{
		// Lock icon list
		lock_listlock(&info->objects,1);

		// Devices?
		if (flags&BGOF_DISKS) backdrop_get_devices(info);

		// Desktop?
		if (flags&BGOF_DESKTOP) desktop_refresh_icons(info,FALSE);

		// AppIcons?
		if (flags&BGOF_APPICONS) backdrop_read_appicons(info,0);

		// Leftouts?
		if (flags&BGOF_LEFTOUT)
		{
			backdrop_add_leftouts(info);
			read_desktop_folder(info,FALSE);
		}

		// Groups?
		if (flags&BGOF_GROUPS) backdrop_read_groups(info);

		// Unlock icon list
		unlock_listlock(&info->objects);
	}

	// Show objects
	if (flags&BGOF_SHOW)
	{
		// Arrange backdrop objects
		backdrop_arrange_objects(info);

		// Show backdrop objects (including newly arranged icons)
		backdrop_show_objects(info,(flags&BGOF_FIRST)?BDSF_RECALC:BDSF_CLEAR|BDSF_RECALC);

		// Check for auto-open groups (if we did groups)
		if (flags&BGOF_GROUPS) backdrop_check_groups(info);
	}
}


// Get icon font
void backdrop_get_font(BackdropInfo *info)
{
	char name[60];
	short which;

	// No window?
	if (!info->window) return;

	// Get which font
	which=(info->flags&BDIF_MAIN_DESKTOP)?0:1;

	// Get font name
	strcpy(name,environment->env->font_name[FONT_ICONS+which]);

	// Add .font if necessary
	if (!(strstri(name,".font")))
		strcat(name,".font");

	// Check font is different from current one
	if (info->rp.Font &&
		info->rp.Font->tf_Message.mn_Node.ln_Name &&
		stricmp(name,info->rp.Font->tf_Message.mn_Node.ln_Name)==0 &&
		info->rp.Font->tf_YSize==environment->env->font_size[FONT_ICONS+which])
	{
		// It's the same
		return;
	}

	// Use default font initially
	SetFont(&info->rp,GUI->screen_pointer->RastPort.Font);

	// Free existing font
	if (info->font)
	{
		CloseFont(info->font);	
		info->font=0;
	}

	// Valid font name?
	if (environment->env->font_name[FONT_ICONS+which][0])
	{
		struct TextAttr attr;

		// Fill in TextAttr
		attr.ta_Name=name;
		attr.ta_YSize=environment->env->font_size[FONT_ICONS+which];
		attr.ta_Style=0;
		attr.ta_Flags=0;

		// Get new font, set it in the window
		if (info->font=OpenDiskFont(&attr))
			SetFont(&info->rp,info->font);
	}
}
