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

Opus® and Directory Opus® and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofm.h"

// Start dragging
BOOL backdrop_start_drag(BackdropInfo *info,short x,short y)
{
	BackdropObject *object;

	// Set drag flag
	info->flags|=BDIF_DRAGGING;

	// Copy RastPort and initialise GELs
	GUI->drag_screen_rp=GUI->screen_pointer->RastPort;
	GUI->drag_screen_rp.GelsInfo=&GUI->drag_info;
	InitGels(&GUI->drag_head,&GUI->drag_tail,&GUI->drag_info);

	// Custom dragging?
	if (DragCustomOk(GUI->drag_screen_rp.BitMap))
	{
		// Set custom flag
		info->flags|=BDIF_CUSTOM_DRAG;
	}

	// Normal dragging
	else info->flags&=~BDIF_CUSTOM_DRAG;

	// Reset saved coords
	info->last_x=x;
	info->last_y=y;

	// Stop menu actions, start mouse reporting
	info->window->Flags|=WFLG_RMBTRAP|WFLG_REPORTMOUSE;

	// Turn ticks on
	ModifyIDCMP(info->window,info->window->IDCMPFlags|IDCMP_INTUITICKS);
	info->tick_count=1;
	info->last_tick=0;

	// Get start time
	CurrentTime(&info->drag_sec,&info->drag_mic);

	// Last selected icon?
	if ((object=info->last_sel_object) && object->bdo_state)
	{
		// Check icon isn't locked
		if (object->bdo_flags&BDOF_LOCKED)
		{
			// Can't drag at all
			backdrop_stop_drag(info);
			return 0;
		}
	}

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Is object selected?
		if (object->bdo_state)
		{
			// Check icon isn't locked
			if (object->bdo_flags&BDOF_LOCKED) continue;

			// Start dragging
			if (!(backdrop_drag_object(info,object)))
			{
				// Failed
				backdrop_stop_drag(info);
				return 0;
			}
		}
	}

	return 1;
}


// Start dragging an object
BOOL backdrop_drag_object(
	BackdropInfo *info,
	BackdropObject *object)
{
	struct Image *image;
	ULONG flags=0;

	// Is object already being dragged?
	if (object->bdo_drag_info) return 1;

	// Is there a select image?
	if (object->bdo_icon->do_Gadget.SelectRender &&
		(object->bdo_icon->do_Gadget.Flags&GFLG_GADGHIMAGE))
	{
		image=(struct Image *)object->bdo_icon->do_Gadget.SelectRender;
	}

	// No
	else image=(struct Image *)object->bdo_icon->do_Gadget.GadgetRender;

	// Get flags
	if (info->flags&BDIF_CUSTOM_DRAG)
	{
		// Custom drag
		flags=DRAGF_CUSTOM;

		// Opaque?
		if (environment->env->desktop_flags&DESKTOPF_QUICK_DRAG) flags|=DRAGF_NO_MASK;
	}

	// Get drag info
	if (!(object->bdo_drag_info=
		GetDragInfo(
			info->window,
			&GUI->drag_screen_rp,
			(image->Width<object->bdo_pos.Width)?object->bdo_pos.Width:image->Width,
			(image->Height<object->bdo_pos.Height)?object->bdo_pos.Height:image->Height,
			flags)))
			return 0;

	// Get image to drag
	backdrop_draw_object(info,object,0,&object->bdo_drag_info->drag_rp,0,0);

	// Get mask
	if (environment->env->desktop_flags&DESKTOPF_QUICK_DRAG)
		object->bdo_drag_info->flags|=DRAGF_OPAQUE;
	else
	if (environment->env->desktop_flags&DESKTOPF_TRANSPARENCY)
		object->bdo_drag_info->flags|=DRAGF_TRANSPARENT;
	else
		object->bdo_drag_info->flags|=DRAGF_TRANSPARENT|DRAGF_OPAQUE;
	GetDragMask(object->bdo_drag_info);

	return 1;
}


// Stop dragging
BOOL backdrop_stop_drag(BackdropInfo *info)
{
	BackdropObject *object;
	BOOL ret=0;

	// Rubber-banding?
	if (info->flags&BDIF_RUBBERBAND)
	{
		// Erase box
		backdrop_rubber_band(info,0);

		// Clear flag and mouse reporting
		info->flags&=~BDIF_RUBBERBAND;
		info->window->Flags&=~(WFLG_REPORTMOUSE|WFLG_RMBTRAP);

		// Turn ticks off
		ModifyIDCMP(info->window,info->window->IDCMPFlags&~IDCMP_INTUITICKS);

		// If icon positioning, unlock layers
		if (GUI->flags&GUIF_DRAGGING)
		{
			UnlockLayers(&info->window->WScreen->LayerInfo);
			GUI->flags&=~GUIF_DRAGGING;
		}
	}

	// Dragging?
	else
	if (info->flags&BDIF_DRAGGING)
	{
		// Clear drag flag
		info->flags&=~BDIF_DRAGGING;

		// Are we dragging?
		if (GUI->flags&GUIF_DRAGGING)
		{
			ULONG sec,mic,mic1,mic2;

			// Get stop time
			CurrentTime(&sec,&mic);

			// Normal dragging?
			if (!(info->flags&BDIF_CUSTOM_DRAG))
			{
				// Go through backdrop list
				for (object=(BackdropObject *)info->objects.list.lh_Head;
					object->bdo_node.ln_Succ;
					object=(BackdropObject *)object->bdo_node.ln_Succ)
				{
					// Is object being dragged?
					if (object->bdo_drag_info)
					{
						// Hide object
						RemBob(&object->bdo_drag_info->bob);
					}
				}
			}

			// Draw GELs list
			SortGList(&GUI->drag_screen_rp);
			DrawDragList(&GUI->drag_screen_rp,&info->window->WScreen->ViewPort,(info->flags&BDIF_CUSTOM_DRAG)?DRAGF_CUSTOM|DRAGF_REMOVE:0);

			// Unlock layer
			UnlockLayers(&info->window->WScreen->LayerInfo);

			// Get drag time deltas
			mic1=(sec*1000000)+mic;
			mic2=(info->drag_sec*1000000)+info->drag_mic;

			// If drag was less than a fifth of a second, nothing moves
			if (mic1<mic2+200000) ret=1;
		}

		// Go through backdrop list
		for (object=(BackdropObject *)info->objects.list.lh_Head;
			object->bdo_node.ln_Succ;
			object=(BackdropObject *)object->bdo_node.ln_Succ)
		{
			// Was object being dragged?
			if (object->bdo_drag_info)
			{
				if ((info->flags & BDIF_CUSTOM_DRAG) && (GUI->flags & GUIF_DRAGGING))
				{
					// Now is the right time to remove the object from the GELs list. This is
					// done in a different place than RemBob for regular dragging. RemBob
					// needs to be done _before_ DrawDragList()->DrawGList(). With custom
					// dragging the object needs to _stay_ in the GELs list before the call to
					// DrawDragList(..., DRAGF_CUSTOM|DRAGF_REMOVE).
					// Thus, remove the object from the GELs list here instead.
					RemVSprite(&object->bdo_drag_info->sprite);
				}
				// Free drag
				FreeDragInfo(object->bdo_drag_info);
				object->bdo_drag_info=0;
			}
		}

		// Clear drag flag
		GUI->flags&=~GUIF_DRAGGING;

		// Start menu actions, stop mouse reporting
		info->window->Flags&=~(WFLG_RMBTRAP|WFLG_REPORTMOUSE);

		// Turn ticks off
		ModifyIDCMP(info->window,info->window->IDCMPFlags&~IDCMP_INTUITICKS);

		// Unlock backdrop list
		unlock_listlock(&info->objects);
	}

	return ret;
}


// Show drag objects
void backdrop_show_drag(
	BackdropInfo *info,
	BackdropObject *first,
	short x,
	short y)
{
	BackdropObject *object;
	BOOL ok=0;

	// We also won't move if the mouse position hasn't changed
	if (info->last_x==x && info->last_y==y) return;

	// Save coords
	info->last_x=x;
	info->last_y=y;
	
	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Is object being dragged?
		if (object->bdo_drag_info)
		{
			short ox,oy;

			// Get object position relative to first object
			ox=object->bdo_pos.Left-first->bdo_pos.Left;
			oy=object->bdo_pos.Top-first->bdo_pos.Top;

			// Offset by mouse coordinates
			ox+=x;
			oy+=y;

			// Offset by drag offset and window offset
			ox+=first->bdo_drag_x_offset+info->size.MinX-info->offset_x;
			oy+=first->bdo_drag_y_offset+info->size.MinY-info->offset_y;

			// Position image
			object->bdo_drag_info->sprite.X=ox;
			object->bdo_drag_info->sprite.Y=oy;
			ok=1;

			// Not dragging yet?
			if (!(GUI->flags&GUIF_DRAGGING))
			{
		// If not custom dragging..
				if (!(info->flags & BDIF_CUSTOM_DRAG))
				{
		    // ..Add bob to list
					AddBob(&object->bdo_drag_info->bob, &GUI->drag_screen_rp);
				}
				else
				{
					// Custom dragging is abusing the GELs list merely for keeping track of
					// the dragged objects, thus use AddVSprite() to link it into the list
					AddVSprite(&object->bdo_drag_info->sprite, &GUI->drag_screen_rp);
				}
			}
		}
	}

	// Update GEL list?
	if (ok)
	{
		// Just started dragging?
		if (!(GUI->flags&GUIF_DRAGGING))
		{
			// Set drag flag
			GUI->flags|=GUIF_DRAGGING;

			// Lock layer
			LockLayers(&info->window->WScreen->LayerInfo);
		}

		// Sort GELs list
		SortGList(&GUI->drag_screen_rp);
		DrawDragList(&GUI->drag_screen_rp,&info->window->WScreen->ViewPort,(info->flags&BDIF_CUSTOM_DRAG)?DRAGF_CUSTOM:0);
		WaitTOF();
	}

}


// Drop some objects on another one
void backdrop_drop_object(BackdropInfo *info,BackdropObject *on_object)
{
	BackdropObject *object;
	char *buf,*dbuf;
	struct InfoData __aligned data;
	BPTR lock;

	if (!(buf=AllocVec(512,0)))
		return;
	dbuf=buf+256;

	// Drop on an AppIcon
	if (on_object->bdo_type==BDO_APP_ICON)
	{
		GalileoAppMessage *msg;
		struct MsgPort *port;

		// Is the icon busy?
		if (on_object->bdo_flags&BDOF_BUSY)
		{
			DisplayBeep(info->window->WScreen);
			FreeVec(buf);
			return;
		}

		// Build AppMessage
		if (!(msg=backdrop_appmessage(info,1)))
		{
			FreeVec(buf);
			return;
		}

		// Set message type
		msg->ga_Msg.am_Type=MTYPE_APPICON;

		// Get AppInfo
		port=WB_AppWindowData(
			(struct AppWindow *)on_object->bdo_misc_data,
			&msg->ga_Msg.am_ID,
			&msg->ga_Msg.am_UserData);

		// Send the message
		PutMsg(port,(struct Message *)msg);
		FreeVec(buf);
		return;
	}

	// Only support dropping on disks at the moment
	if ((on_object->bdo_type!=BDO_DISK && on_object->bdo_type!=BDO_BAD_DISK) ||
		on_object->bdo_flags&(BDOF_ASSIGN|BDOF_CACHE))
	{
		DisplayBeep(info->window->WScreen);
		FreeVec(buf);
		return;
	}

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Selected disk?
		if (object->bdo_type==BDO_DISK && !(object->bdo_flags&(BDOF_ASSIGN|BDOF_CACHE)) && object->bdo_state)
		{
			BPTR lock;

			// Get disk lock
			if (lock=backdrop_icon_lock(object))
			{
				// This is the source
				Info(lock,&data);

				// Get device name
				DeviceFromLock(lock,buf);

				// Unlock lock
				UnLock(lock);
			}
			break;
		}
	}

	// Didn't get anything?
	if (!object->bdo_node.ln_Succ)
	{
		DisplayBeep(info->window->WScreen);
		FreeVec(buf);
		return;
	}

	// Drop on a bad disk?
	if (on_object->bdo_type==BDO_BAD_DISK)
	{
		// Get device name
		strcpy(dbuf,on_object->bdo_name);
	}

	// Real disk
	else
	{
		// Get destination disk lock
		if (!(lock=backdrop_icon_lock(on_object)))
		{
			FreeVec(buf);
			return;
		}

		// Destination is the one we dropped it on
		Info(lock,&data);

		// Get device name
		DeviceFromLock(lock,dbuf);
		UnLock(lock);
	}

	// Launch diskcopy
	function_launch(
		FUNCTION_RUN_FUNCTION,
		def_function_diskcopy,
		0,
		0,
		0,0,
		0,0,
		BuildArgArray(buf,dbuf,0),
		0,0);

	FreeVec(buf);
}
