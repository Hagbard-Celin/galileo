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

#include "galileofm.h"
#include <gfm/strings_protos.h>
#include <gfm/drag_routines_protos.h>
#include "lister_protos.h"
#include "dirlist_protos.h"
#include "misc_protos.h"
#include "app_msg_protos.h"
#include "function_launch_protos.h"
#include "rexx_protos.h"
#include "buffers_protos.h"
#include "path_routines.h"
#include "backdrop_protos.h"
#include "lsprintf_protos.h"

// Handle some dropped files
void lister_receive_drop(Lister *dest, GalileoListerAppMessage *msg)
{
	Lister *source=0;
	ULONG flags=0, launch_flags;
	long func_type;
	Cfg_Function *function;
	BackdropInfo *info;
	DirEntry *over_entry=0;
	UWORD qual;
	WORD ok = -1;
	BOOL read=0;
	BPTR dest_lock=0, source_lock;
	struct WBArg *arglist;
	struct GLAData *argdata = 0;
	struct AppMessage *amsg = 0;

	// Cache backdrop pointer
	info=dest->backdrop_info;

	if (msg->glam_Type == MTYPE_LISTER_APPWINDOW && msg->glam_Class == GLAMCLASS_LISTER)
	{
	    source = msg->glam_Lister;
	    flags = msg->glam_Flags;
	    over_entry = msg->glam_OverEntry;
	    arglist = msg->glam_ArgList;
	    argdata = msg->glam_ArgData;

	}
	else
	{
	    amsg = (struct AppMessage *)msg;
	    arglist = amsg->am_ArgList;
	}

	// Set launch flag
	launch_flags = FUNCF_DRAG_DROP;

	// Get qualifiers
	qual=(InputBase)?PeekQualifier():0;

	// Does current buffer have a custom handler?
	if (dest->cur_buffer->buf_CustomHandler[0])
	{
		char *tmp_path = 0;

		if (amsg && dest->cur_buffer->cust_flags&CUSTF_NO_EXT_APPMSG)
		{
		    DisplayBeep(dest->window->WScreen);
		    return;
		}

		// Does message have lister in it?
		if (source==dest)
		{
			// Clear lister pointer
			msg->glam_Lister = 0;
		}

		if (over_entry)
		    tmp_path = JoinString(NULL, dest->cur_buffer->buf_Path, over_entry->de_Node.dn_Name, NULL, NULL);
		else
		if (flags&DROPF_PARENT)
		{
		    if (tmp_path = CopyString(NULL, dest->cur_buffer->buf_Path))
			path_parent(tmp_path);
		}

		// Send to rexx handler
		if (over_entry || flags&DROPF_PARENT) qual|=IEQUALIFIER_SUBDROP;
		rexx_custom_app_msg((struct AppMessage *)msg,dest->cur_buffer,"drop",source,(tmp_path)?tmp_path:dest->cur_buffer->buf_Path,qual);

		if (tmp_path)
		    FreeMemH(tmp_path);

		if (!amsg)
		    reply_lister_appmsg(msg);

		return;
	}

	// Get first entry lock
	if (!(source_lock = arglist[0].wa_Lock))
	    source_lock = msg->glam_Lock;

	// Is first entry a directory?
	if ((argdata && argdata[0].glad_Flags&(AAEF_DIR|AAEF_DEV)) || (!argdata && (WBArgDir(&arglist[0]))))
	{
	    // In icon mode?
	    if (dest->flags&LISTERF_VIEW_ICONS)
	    {
		// Showing device list?
	        if (dest->cur_buffer->more_flags&DWF_DEVICE_LIST)
	        {
			BPTR new_lock, org_dir = 0;

			if (argdata && argdata[0].glad_Flags&AAEF_DIR)
		        {
			    org_dir = CurrentDir(source_lock);
			    new_lock = Lock(arglist[0].wa_Name, ACCESS_READ);
		        }
		        else
			    new_lock = DupLock(source_lock);

		        // Do assign
		        // FIXME: Lock!!!!
		        function_launch(
			        FUNCTION_RUN_FUNCTION,
			        def_function_assign,
			        0,
			        launch_flags,
			        0,dest,
				0,0,
				new_lock,0,
			        0,
			        0,
				0);

		        if (org_dir)
			    CurrentDir(org_dir);

			if (!amsg)
			    reply_lister_appmsg(msg);
		        return;
	        }
	    }
	    // Can only read if lister is open, and not in icon mode
	    else
	    if (dest->window)
	    {
	        // If shift is down, we read it, no questions asked
	        if (qual&IEQUAL_ANYSHIFT)
		    read=1;
	        else
	        // Is it a device?
		if (argdata)
		{
		    if (argdata[0].glad_Flags&AAEF_DEV)
		        read=1;
		}
		else
		{
		    BPTR parentlock;

		    if (parentlock = ParentDir(arglist[0].wa_Lock))
			UnLock(parentlock);
		    else
			read = 1;
		}

	        // Want to read directory?
	        if (read)
	        {
			BPTR new_lock, org_dir = 0;

			if (argdata && argdata[0].glad_Flags&AAEF_DIR)
		        {
			    org_dir = CurrentDir(source_lock);
			    new_lock = Lock(arglist[0].wa_Name, ACCESS_READ);
		        }
		        else
			    new_lock = DupLock(source_lock);

		        if (new_lock)
		        {
		            // Load directory
		            read_directory(
			            dest,
			            arglist[0].wa_Name,
				    new_lock,
			            GETDIRF_CANCHECKBUFS|GETDIRF_CANMOVEEMPTY);
		        }
		        else
			    DisplayBeep(dest->window->WScreen);

		        if (org_dir)
			    CurrentDir(org_dir);

			if (!amsg)
			    reply_lister_appmsg(msg);

		        return;
	        }
	    }
	}

	if (amsg || flags&DROPF_ICON_MODE)
	    launch_flags |= FUNCF_ICONS;

	// Lister must have a valid buffer
	if (!(dest->cur_buffer->flags&DWF_VALID)) return;

	// Normal filetype action to start with
	func_type=FUNCTION_FILETYPE;
	function=0;

	// Is destination lister in icon mode?
	if (dest->flags&LISTERF_VIEW_ICONS)
	{
		BackdropObject *drop_on;

		// Lock backdrop list
		lock_listlock(&info->objects,0);

		// See if it's dropped on anything
		if (drop_on=backdrop_get_object(info,msg->glam_MouseX,msg->glam_MouseY,0))
		{
			// Is shift/alt down?
			if ((qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))==(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))
			{
				char disk[5] = "Disk";

				if (!argdata && WBArgDir(&arglist[0]))
			        {
				    BPTR parent_lock;


				    // Try to get parent dir
				    if (parent_lock = ParentDir(source_lock))
				    {
				        D_S(struct FileInfoBlock,tmp_fib);

				        // Get directory name
					Examine(source_lock,tmp_fib);
				        backdrop_replace_icon_image(info,tmp_fib->fib_FileName,parent_lock,drop_on);
				        UnLock(parent_lock);
				    }
				    else
					backdrop_replace_icon_image(info,disk,source_lock,drop_on);
			        }
				else
				{
				    if (argdata && argdata[0].glad_Flags == AAEF_DEV)
					backdrop_replace_icon_image(info,disk,source_lock,drop_on);
				    else
					backdrop_replace_icon_image(info,arglist[0].wa_Name,source_lock,drop_on);
				}

				ok=0;
			}

			// Is this a drawer?
			else
			if (drop_on->bdo_icon->do_Type==WBDRAWER || drop_on->bdo_icon->do_Type==WBGARBAGE)
			{
				BPTR new_lock;

				if (new_lock = Lock(drop_on->bdo_name, ACCESS_READ))
				{
				    dest_lock = new_lock;

				    launch_flags |= FUNCF_DROPON_LOCK;

				    // Move into this drawer
				    func_type=FUNCTION_RUN_FUNCTION_EXTERNAL;
				    function=def_function_copy;
				    ok = 1;
				}
				else
				    ok = -2;
			}

			// Dropped on a tool?
			else
			if (drop_on->bdo_icon->do_Type==WBTOOL)
			{
				// Run program with args
				backdrop_object_open(
					info,
					drop_on,
					0,
					0,
					(amsg)?0:msg,
					amsg);
				ok = -2;
			}
		}

		// If not, just use copy command
		else
		{
			function=def_function_copy;
			func_type=FUNCTION_RUN_FUNCTION_EXTERNAL;
			dest_lock = dest->cur_buffer->buf_Lock;

			ok = 1;
		}

		// Unlock backdrop list
		unlock_listlock(&info->objects);
	}

	// Drop was in name mode; if drop was into a sub-directory, add that to path
	else
	if (over_entry)
	{
		BPTR over_lock;

		if (over_lock = Lock(over_entry->de_Node.dn_Name,ACCESS_READ))
		{
		    dest_lock = over_lock;
		    launch_flags |= FUNCF_DROPON_LOCK;
		    ok = 1;
		}
	}

	// Or drop into parent area
	else
	if (flags&DROPF_PARENT)
	{
		BPTR over_lock;
		if (over_lock = ParentDir(dest->cur_buffer->buf_Lock))
		{
		    dest_lock = over_lock;
		    launch_flags |= FUNCF_DROPON_LOCK;
		    ok = 1;
		}
		else
		    ok = -2;
	}

	// Ok to do filetype?
	if (ok > 0 || ok == -1)
	{
		short action;

		if (!dest_lock)
		    dest_lock = dest->cur_buffer->buf_Lock;
/* ACTION_CHANGE
		// If source is in icon mode, don't use it
		if (source && (source->flags&LISTERF_VIEW_ICONS) && !(source->flags&LISTERF_ICON_ACTION))
			source=0;
*/

		// Get filetype action
		if (qual&IEQUALIFIER_CONTROL) action=FTTYPE_CTRL_DRAGDROP;
		else
		if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) action=FTTYPE_ALT_DRAGDROP;
		else
		action=FTTYPE_DRAG_DROP;

		// Do filetype action on a file
		function_launch(
			func_type,
			function,
			action,
			launch_flags,
			source,dest,
			0,0,
			0,dest_lock,
			0,
			0,
			(amsg)?(Buttons *)CopyAppMessage(amsg,dest->lister_memory):(Buttons *)msg);
	}
	else
	if (!amsg && ok == 0)
	    reply_lister_appmsg(msg);
}



// Test to see whether a drag is over a lister
DirEntry *lister_test_drag(
	Lister *our_lister,
	DragInfo *drag,
	short x,
	short y,
	Lister **last_lister)
{
	struct Window *window;
	Lister *lister=0;
	DirEntry *entry=0;

	// Lock the lister list
	lock_listlock(&GUI->lister_list,FALSE);

	// Find window we're over
	if (x>-1 && (window=WhichWindow(GUI->screen_pointer,x,y,WWF_NO_LOCK)))
	{
		ULONG id;

		// Is this a lister?
		if ((id=GetWindowID(window))==WINDOW_LISTER)
		{
			IPCData *ipc;

			// Get IPC pointer
			if (ipc=(IPCData *)GetWindowAppPort(window))
			{
				// Get lister pointer
				if (lister=(Lister *)IPCDATA(ipc))
				{
					// If lister is busy, can't use it
					if (lister->flags&LISTERF_BUSY) lister=0;

					// Custom handler, not enabled?
					else
					if (lister->cur_buffer->buf_CustomHandler[0] &&
						!(lister->cur_buffer->cust_flags&CUSTF_SUBDROP)) lister=0;
				}
			}
		}

/*
		// Or a start-menu?
		else
		if (id==WINDOW_START)
		{
			StartMenu *menu;

			// Get start menu pointer
			if (menu=(StartMenu *)GetWindowAppPort(window))
			{
				// Menu not already popped?
				if (!(menu->flags&STARTMENUF_POPPED))
				{
					// Send open command
					IPC_Command(menu->ipc,IPC_OPEN,0,0,0,0);
				}
			}
		}
*/
	}

	// Had a 'last lister' that was different?
	if (*last_lister && *last_lister!=lister)
	{
		// Was the last lister our lister?
		if (*last_lister==our_lister)
		{
			// Remove highlight
			lister_highlight(our_lister,-1,-1,drag);
		}

		// Is old lister still valid?
		else
		if (lister_check_valid(*last_lister))
		{
			// Tell old lister to stop highlighting
			IPC_Command(
				(*last_lister)->ipc,
				LISTER_HIGHLIGHT,
				(ULONG)-1,
				drag,
				0,
				REPLY_NO_PORT);
		}
	}

	// Got a new lister?
	if (lister)
	{
		// Is it our lister?
		if (lister==our_lister)
		{
			// Show highlight
			entry=lister_highlight(our_lister,x,y,drag);
		}

		// Another lister
		else
		{
			// Tell lister to highlight
			entry=(DirEntry *)IPC_Command(
				lister->ipc,
				LISTER_HIGHLIGHT,
				(y<<16)|x,
				drag,
				0,
				REPLY_NO_PORT);
		}
	}

	// Save lister pointer
	*last_lister=lister;

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);

	return entry;
}


// Highlight an entry if mouse is over it
DirEntry *lister_highlight(Lister *lister,short x,short y,DragInfo *drag)
{
	DirEntry *entry=0;
	short line=0,scroll=0;
	BYTE vert_space = environment->env->lister_vert_space;

	// Ignore if in icon mode or no window
	if (lister->flags&LISTERF_VIEW_ICONS || !lister_valid_window(lister))
		return 0;

	// Convert coordinates to lister-relative
	x-=lister->window->LeftEdge;
	y-=lister->window->TopEdge;

	// Lock buffer
	buffer_lock(lister->cur_buffer,FALSE);

	// Can scroll?
	if (x>=lister->text_area.rect.MinX &&
		x<=lister->text_area.rect.MaxX)
	{
		// Scroll down?
		if (y>lister->text_area.rect.MaxY &&
			y<lister->text_area.rect.MaxY+(lister->text_area.font->tf_YSize + vert_space))
		{
			// Ok to scroll?
			if (lister->cur_buffer->buf_VertOffset+lister->text_height<lister->cur_buffer->buf_TotalEntries[0])
			{
				scroll=1;
				line=lister->last_highpos;
			}
		}

		// Scroll up?
		else
		if (y<lister->text_area.rect.MinY &&
			y>lister->text_area.rect.MinY-(lister->text_area.font->tf_YSize + vert_space))
		{
			// Ok to scroll?
			if (lister->cur_buffer->buf_VertOffset>0)
			{
				scroll=-1;
				line=0;
			}
		}

		// Is mouse within text area?
		else
		if (point_in_element(&lister->text_area,x,y))
		{
			short num;

			// Get line clicked on
			line=(y-lister->text_area.rect.MinY)/(lister->text_area.font->tf_YSize + vert_space);
			num=line+lister->cur_buffer->buf_VertOffset;

			// Valid entry?
			if (num>=0 && num<lister->cur_buffer->buf_TotalEntries[0])
			{
				// Find entry
				for (entry=(DirEntry *)lister->cur_buffer->entry_list.mlh_Head;
					num>0 && entry->de_Node.dn_Succ;
					entry=(DirEntry *)entry->de_Node.dn_Succ,num--);

				// Invalid entry, or not a directory?
				if (!entry->de_Node.dn_Succ || entry->de_Node.dn_Type<=0) entry=0;
			}
		}
	}

	// Has highlighted entry changed?
	if (lister->last_highlight!=entry || scroll)
	{
		DirEntry *last;

		// Cache old and store new last highlight pointer
		last=lister->last_highlight;
		lister->last_highlight=entry;

		// Got draginfo?
		if (drag)
		{
			// Save drag position
			x=drag->sprite.X;
			y=drag->sprite.Y;

			// Clear drag stuff
			HideDragImage(drag);
		}
		
		// Was there a valid last highlight?
		if (last)
		{
			// Clear flag in last highlight
			last->de_Flags&=~ENTF_HIGHLIGHT;

			// Show last highlight again
			display_entry(last,lister,lister->last_highpos);
		}

		// Want to scroll?
		if (scroll) lister_scroll(lister,0,scroll);

		// Got a valid entry?
		if (entry)
		{
			// Save position
			lister->last_highpos=line;

			// Set highlight flag
			entry->de_Flags|=ENTF_HIGHLIGHT;

			// Show new highlight
			display_entry(entry,lister,line);
		}

		// Show drag again
		if (drag) ShowDragImage(drag,x,y);
	}

	// Unlock buffer
	buffer_unlock(lister->cur_buffer);

	return entry;
}
