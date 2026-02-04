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
#include <gfm/paths_protos.h>
#include <gfm/popup_protos.h>
#include <gfm/config_display.h>
#include <gfm/appmessage.h>
#include "app_msg_protos.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "backdrop_protos.h"
#include "desktop.h"
#include "menu_data.h"
#include "icons.h"

ULONG desktop_drop(BackdropInfo *info,struct AppMessage *msg,UWORD qual)
{
	BackdropObject *drop_obj;
	short ok=0,x,y;
	BPTR source_lock = 0;
	short action=DESKTOP_POPUP_LEFTOUT;
	Lister *source=0;
	GalileoListerAppMessage *lamsg = 0;
	WORD arg,numargs;
	struct WBArg *arglist;
	struct GLAData *argdata = 0;
	ULONG reply;
	char disk[5] = "Disk";

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	if (msg->am_Type == MTYPE_LISTER_APPWINDOW && msg->am_Class == GLAMCLASS_LISTER)
	{
	    lamsg = (GalileoListerAppMessage *)msg;
	    source = lamsg->glam_Lister;
	    //flags = lamsg->glam_Flags;
	    //over_entry = lamsg->glam_OverEntry;
	    numargs = lamsg->glam_NumArgs;
	    arglist = lamsg->glam_ArgList;
	    argdata = lamsg->glam_ArgData;
	    reply = 2;

	}
	else
	{
	    //amsg = (struct AppMessage *)msg;
	    numargs = msg->am_NumArgs;
	    arglist = msg->am_ArgList;
	    reply = 1;
	}

	// See if we dropped stuff on an object
	if ((drop_obj=backdrop_get_object(info,msg->am_MouseX,msg->am_MouseY,0)))
	{
		short drop_ok;

	        // Is shift/alt down?
		if ((qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))==(IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT))
	        {
			if (arglist[0].wa_Lock)
			    source_lock = arglist[0].wa_Lock;
			else
			if (lamsg)
			    source_lock = lamsg->glam_Lock;

			if (!argdata && WBArgDir(&arglist[0]))
		        {
			    BPTR parent_lock;

			    // Try to get parent dir
			    if (parent_lock = ParentDir(source_lock))
			    {
			        D_S(struct FileInfoBlock,tmp_fib);

			        // Get directory name
				Examine(source_lock,tmp_fib);
			        backdrop_replace_icon_image(info,tmp_fib->fib_FileName,parent_lock,drop_obj);
			        UnLock(parent_lock);
			    }
			    else
			        backdrop_replace_icon_image(info,disk,source_lock,drop_obj);
		        }
			else

			if (argdata && argdata[0].glad_Flags == AAEF_DEV)
		        {
			    backdrop_replace_icon_image(info,disk,source_lock,drop_obj);
		        }
		        else
			    backdrop_replace_icon_image(info,arglist[0].wa_Name,source_lock,drop_obj);
	        }

	        // Handle drop on an object
	        else
		if (drop_ok = desktop_drop_on_object(info,lamsg,(lamsg)?0:msg,drop_obj,qual))
	        {
		    if (drop_ok == 1 && lamsg)
		    {
		        // Reply the message
			reply_lister_appmsg(lamsg);
			reply = 0;
		    }
	        }

	        // Not dropped
	        else drop_obj=0;
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Dropped on an object?
	if (drop_obj) return reply;

	// Popup menu?
	if (environment->env->env_flags&ENVF_DESKTOP_FOLDER)
	{
		UWORD res;

		// Activate the underlying menu
		ActivateWindow(info->window);

		// Default action set?
		if (environment->env->desktop_popup_default>DESKTOP_POPUP_NONE &&
			!(qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)))
		{
			// Use default action
			action=environment->env->desktop_popup_default;
		}

		// Ask user what they want to do
		else
		if ((res=DoPopUpMenu(info->window,&GUI->desktop_menu->ph_Menu,0,SELECTDOWN))==(UWORD)-1 || res==MENU_DESKTOP_CANCEL)
		{
			return reply;
		}

		// Help?
		else
		if (res&POPUP_HELPFLAG)
		{
			// Do help
			help_menu_help(res&~POPUP_HELPFLAG,0);

			return reply;
		}

		// Get action
		else
		if (res==MENU_DESKTOP_COPY) action=DESKTOP_POPUP_COPY;
		else
		if (res==MENU_DESKTOP_MOVE) action=DESKTOP_POPUP_MOVE;
	}

	// Set busy pointer
	SetBusyPointer(info->window);

	// Go through arguments
	for (arg=0;arg<numargs;arg++)
	{
	    BOOL rootdir = FALSE;

	    if (!argdata)
	    {
		if (WBArgDir(&arglist[0]))
		{
		    BPTR parentlock;

		    if (parentlock = ParentDir(arglist[arg].wa_Lock))
		        UnLock(parentlock);
		    else
		        rootdir = TRUE;
		}
	    }
	    else
	    if (argdata && argdata[arg].glad_Flags&AAEF_DEV)
		rootdir = TRUE;

	    if (!rootdir)
	    {
	        // What operation?
	        switch (action)
	        {

		        // Leave out?
		        case DESKTOP_POPUP_LEFTOUT:

			        // Ignore if it's an icon or a disk
				if (!(isicon(arglist[arg].wa_Name)))
			        {
					short perm=0;
				        BackdropObject *icon;

				        // Permanent?
				        if (GUI->flags2&GUIF2_BENIFY)
					        perm=1;

				        if (!argdata)
				        {
				            // Get position
				            x=msg->am_MouseX;
				            y=msg->am_MouseY;
				        }
				        else
				        {
				            // Get position
				            x=lamsg->glam_MouseX;
				            y=lamsg->glam_MouseY;

				            // Adjust position for icon offset
				            x+=lamsg->glam_DragOffset.x+argdata[arg].glad_DropPos.x;
				            y+=lamsg->glam_DragOffset.y+argdata[arg].glad_DropPos.y;
				        }

				        if (arglist[arg].wa_Lock)
					    source_lock = arglist[arg].wa_Lock;
				        else
				        if (lamsg)
					    source_lock = lamsg->glam_Lock;

				        // Leave this out
				        if (icon=backdrop_leave_out(info,arglist[arg].wa_Name,source_lock,(perm)?BLOF_PERMANENT:0,x,y))
				        {
					        // Position it
					        backdrop_place_object(info,icon);

					        // Save for permanent leftout
					        if (perm) backdrop_save_leftouts(info);
					        ok=1;
				        }
			        }
			        break;

		        // Copy/Move
		        case DESKTOP_POPUP_COPY:
		        case DESKTOP_POPUP_MOVE:

				ok = 1;

			        break;
	        }
	    }
	}

	// Successful?
	if (ok)
	{
	        // Left-outs?
	        if (action==DESKTOP_POPUP_LEFTOUT)
	        {
		        // Refresh
		        backdrop_show_objects(info,BDSF_RECALC);
	        }

	        // Otherwise, launch function
	        else
	        {
		        // Launch the function
		        function_launch(
			        FUNCTION_RUN_FUNCTION_EXTERNAL,
			        (action==DESKTOP_POPUP_COPY)?def_function_copy:def_function_move,
			        0,
			        FUNCF_ICONS|FUNCF_RESCAN_DESKTOP|FUNCF_DRAG_DROP|FUNCF_COPY_NO_MOVE,
			        source,0,
			        0,environment->env->desktop_location,
			        0,GUI->desktop_dir_lock,
			        0,
			        0,
			        (lamsg)?(Buttons *)lamsg:(Buttons *)CopyAppMessage(msg,global_memory_pool));

		        if (!lamsg)
			    ReplyAppMessage(msg);

			reply = 0;
	        }
	}

	// Clear busy pointer
	ClearPointer(info->window);

	return reply;
}


// Handle drop on an object
BOOL desktop_drop_on_object(BackdropInfo *info,GalileoListerAppMessage *lamsg,struct AppMessage *amsg,BackdropObject *drop_obj,UWORD qual)
{
	Lister *source = 0;
	struct WBArg *arglist;
	struct GLAData *argdata = 0;
	char *name;
	WORD numargs;
	short ret=1;

	// Allocate buffer
	if (!(name=AllocVec(1024,0))) return 1;

	if (lamsg)
	{
	    source = lamsg->glam_Lister;
	    numargs = lamsg->glam_NumArgs;
	    arglist = lamsg->glam_ArgList;
	    argdata = lamsg->glam_ArgData;
	}
	else
	if (amsg)
	{
	    numargs = amsg->am_NumArgs;
	    arglist = amsg->am_ArgList;
	}
	else
	    return 1;

	// Was it an AppIcon?
	if (drop_obj->bdo_type==BDO_APP_ICON)
	{
	    // Flash error
	    DisplayBeep(info->window->WScreen);

	    return 1;
	}

	// Was it a group?
	else
	if (drop_obj->bdo_type==BDO_GROUP)
	{
		WORD arg;
		GroupData *group;
		BOOL ok=0;
		BPTR parent_lock = 0;

		// Find group if it's open
		lock_listlock(&GUI->group_list,0);
		if (!(group=backdrop_find_group(drop_obj)))
			unlock_listlock(&GUI->group_list);

		// Go through arguments
		for (arg=0;arg<numargs;arg++)
		{
			// Valid file?
			if ((!argdata && arglist[arg].wa_Name && *arglist[arg].wa_Name) ||
			    (argdata && !(argdata[arg].glad_Flags&(AAEF_DIR|AAEF_DEV))))
			{
			        if (arglist[arg].wa_Lock)
				    parent_lock = arglist[arg].wa_Lock;
			        else
				if (lamsg)
				    parent_lock = lamsg->glam_Lock;

				// Send add message to group if it's open
				if (group)
				{
					char *copy;

					// Copy name
					if (copy=AllocVec(strlen(arglist[arg].wa_Name)+1,0))
					{
						strcpy(copy,arglist[arg].wa_Name);
						IPC_Command(group->ipc,GROUP_ADD_ICON,0,(APTR)DupLock(parent_lock),copy,0);
						ok=1;
					}
				}

				// Otherwise add object to group
				else
				{
					backdrop_group_add_object(drop_obj->bdo_name,0,arglist[arg].wa_Name,parent_lock,-1,-1);
					ok=1;
				}
			}
		}

		// Show error if no ok
		if (!ok) DisplayBeep(GUI->screen_pointer);

		// Unlock process list if it's locked
		if (group) unlock_listlock(&GUI->group_list);
	}

	// Or a project?
	else
	if (drop_obj->bdo_icon->do_Type==WBPROJECT)
	{
		// Is it an Galileo command?
		if (command_filetype)
		{
			BPTR lock;

			// Set failure initially
			ret=0;

			// Get lock on directory
			if (lock=backdrop_icon_lock(drop_obj))
			{
				// See if it's a command
				if (filetype_match_type(drop_obj->bdo_name, lock, command_filetype))
				{
					// Run command with args
					backdrop_object_open(
						info,
						drop_obj,
						0,
						0,
						lamsg,
						amsg);
					ret=2;
				}

				// Unlock
				UnLock(lock);
			}
		}

		// Beep if not a command
		if (!ret) DisplayBeep(GUI->screen_pointer);
	}

	// Or a tool?
	else
	if (drop_obj->bdo_icon->do_Type==WBTOOL)
	{
		// Run program with args
		backdrop_object_open(
			info,
			drop_obj,
			0,
			0,
			lamsg,
			amsg);
	}


	// Or a disk/directory?
	else
	if (drop_obj->bdo_icon->do_Type==WBDISK ||
		drop_obj->bdo_icon->do_Type==WBDRAWER ||
		drop_obj->bdo_icon->do_Type==WBGARBAGE)
	{
	    BPTR dest_lock;

	    // Get destination path
	    if (dest_lock=backdrop_icon_lock(drop_obj))
	    {
		    short action;

		    // Is object a left-out?
		    if (drop_obj->bdo_type==BDO_LEFT_OUT)
		    {
			BPTR org_dir, tmp_lock;

			org_dir = CurrentDir(dest_lock);
			if (tmp_lock = Lock(drop_obj->bdo_name,ACCESS_READ))
			{
			    dest_lock = tmp_lock;
			    strcpy(info->buffer,drop_obj->bdo_name);
			    UnLock(CurrentDir(org_dir));
			}
			else
			{
			    CurrentDir(org_dir);
			    FreeVec(name);
			    DisplayBeep(GUI->screen_pointer);
			    return 1;
			}
		    }
		    else
		    if (drop_obj->bdo_type==BDO_DISK)
		    {
			char *tmp;
			tmp = stpcpy(info->buffer,drop_obj->bdo_name);
			*tmp++ = ':';
			*tmp = 0;
		    }

		    // Get filetype action
		    if (qual&IEQUALIFIER_CONTROL) action=FTTYPE_CTRL_DRAGDROP;
		    else
		    if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) action=FTTYPE_ALT_DRAGDROP;
		    else
		    action=FTTYPE_DRAG_DROP;

		    // Do filetype action on files
		    function_launch(
			    FUNCTION_FILETYPE,
			    0,
			    action,
			    FUNCF_DRAG_DROP|FUNCF_ICONS|FUNCF_DROPON_LOCK,
			    source,0,
			    0,info->buffer,
			    0,dest_lock,
			    0, //arg_array,
			    0,
			    (lamsg)?(Buttons *)lamsg:(Buttons *)CopyAppMessage(amsg,global_memory_pool));
		    ret = 2;
	    }
	}
	else ret=0;

	if (amsg)
	    ReplyAppMessage(amsg);


	FreeVec(name);
	return ret;
}
