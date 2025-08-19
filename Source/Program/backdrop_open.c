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
#include "lister_protos.h"
#include "dirlist_protos.h"
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "backdrop_protos.h"
#include "buffers.h"
#include "menu_data.h"
#include "clock_task.h"
#include "lsprintf_protos.h"

// Open a backdrop object
void backdrop_object_open(
	BackdropInfo *info,
	BackdropObject *object,
	UWORD qual,
	ULONG flags,
	GalileoListerAppMessage *lamsg,
	struct AppMessage *amsg)
{
	struct WBArg *arglist = 0;
	struct GLAData *argdata = 0;
	Lister *lister;
	IPCData *ipc=0;
	BPTR old,parent_lock = 0;
	struct DateStamp *voldate = 0, date = {0};
	WORD numargs = 0;

	// Valid object?
	if (!object || !object->bdo_icon) return;

	// Is object a group?
	if (object->bdo_type==BDO_GROUP)
	{
		// Open group
		backdrop_open_group(info,object,1);
		return;
	}

	// Bad disk?
	if (object->bdo_type==BDO_BAD_DISK)
	{
		// Error
		DisplayBeep(info->window->WScreen);
		return;
	}

	// Is object an appicon?
	if (object->bdo_type==BDO_APP_ICON)
	{
		struct AppMessage *msg;
		struct MsgPort *port;

		// Is icon busy?
		if (object->bdo_flags&BDOF_BUSY)
		{
			DisplayBeep(info->window->WScreen);
			return;
		}

		// Allocate AppMessage
		if (!(msg=AllocAppMessage(global_memory_pool,GUI->appmsg_port,0))) return;

		// Message type
		msg->am_Type=MTYPE_APPICON;

		// Get AppInfo
		port=WB_AppWindowData(
			(struct AppWindow *)object->bdo_misc_data,
			&msg->am_ID,
			&msg->am_UserData);

		// Send the message
		PutMsg(port,(struct Message *)msg);
		return;
	}

	// If we're not trying to start a tool, see if there is one selected
	if (object->bdo_icon->do_Type!=WBTOOL && info->first_sel_tool && info->first_sel_tool!=object)
	{
		// Check that it's valid
		if (find_backdrop_object(info,info->first_sel_tool))
		{
			// Use this tool to open the projects
			object=info->first_sel_tool;
		}
	}

	// Get parent lock
	if (object->bdo_parent_lock)
	    parent_lock = object->bdo_parent_lock;
	else
	if (object->bdo_flags&BDOF_DESKTOP_FOLDER)
	    parent_lock = GUI->desktop_dir_lock;
	else
	if (info->lister && info->lister->cur_buffer && info->lister->cur_buffer->buf_Lock)
	{
	    parent_lock = info->lister->cur_buffer->buf_Lock;
	    voldate = &info->lister->cur_buffer->buf_VolumeDate;
	}
	else
	if (object->bdo_icon->do_Type==WBDISK)
	{
	    parent_lock = Lock(object->bdo_device_name,ACCESS_READ);
	    voldate = &object->bdo_date;
	}

	if (!parent_lock)
	    return;

	if (lamsg)
	{
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
	if (flags&BOOF_NOARGS)
	    numargs = -1;

	// Change current directory
	old=CurrentDir(parent_lock);

	// Directory to read?
	if (object->bdo_icon->do_Type==WBDISK ||
		object->bdo_icon->do_Type==WBDRAWER ||
		object->bdo_icon->do_Type==WBGARBAGE)
	{
		char *path = 0;
		BPTR new_lock = 0;

		if (object->bdo_icon->do_Type==WBDISK)
		{
		    path = object->bdo_device_name;
		    new_lock = DupLock(parent_lock);
		}

		// Drawer to read?
		else
		if (object->bdo_icon->do_Type==WBDRAWER || object->bdo_icon->do_Type==WBGARBAGE)
		{
		    path = object->bdo_name;
		    new_lock = Lock(path,ACCESS_READ);
		}

		// Can't open
		if (!path || !new_lock)
		    DisplayBeep(info->window->WScreen);
		else
		{
		    // If shift isn't down, look for already open
		    if (!(qual&IEQUAL_ANYSHIFT))
		    {
			    // Lock lister list
			    lock_listlock(&GUI->lister_list,FALSE);

			    // Go through listers
			    for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
				    ipc->node.mln_Succ;
				    ipc=(IPCData *)ipc->node.mln_Succ)
			    {
				    // Get lister
				    lister=IPCDATA(ipc);

				    // Is this lister what we're after?
				    if ((SameLock(lister->cur_buffer->buf_Lock,new_lock) == LOCK_SAME) &&
					    !(lister->flags&LISTERF_BUSY))
				    {
					    // Activate this window
					    IPC_Command(ipc,IPC_ACTIVATE,0,(APTR)1,0,0);
					    //if (object->bdo_icon->do_Type==WBDISK)
						UnLock(new_lock);
					    break;
				    }
			    }

			    // Unlock lister list
			    unlock_listlock(&GUI->lister_list);

			    // Get one?
			    if (!ipc->node.mln_Succ) ipc=0;
		    }

		    // Open new lister?
		    if (!ipc)
		    {
			    // Unlock path lock
			    //UnLock(new_lock);

			    if (!voldate)
			    {
				if (VolIdFromLock(new_lock, &date, NULL))
				    voldate = &date;
			    }

			    // Read into existing lister in icon action mode if control is down
			    if (info->lister && info->lister->flags&LISTERF_ICON_ACTION && qual&IEQUALIFIER_CONTROL)
			    {
				    // Read into existing lister
				    // FIXME: Use lock FIXED??
				    read_directory(info->lister,path,new_lock,GETDIRF_CANMOVEEMPTY|GETDIRF_CANCHECKBUFS);
			    }

			    // Open lister for this path
			    else
			    if (lister=lister_open_new(path, voldate, new_lock, object, info->window, info->lister))
			    {
				    // Initialise lister
				    IPC_Command(
					    lister->ipc,
					    LISTER_INIT,
					    (flags&BOOF_ACTIVATE)?LISTERF_ACTIVATE:0,
					    GUI->screen_pointer,
					    0,0);

				    // Close parent?
				    if (qual&IEQUALIFIER_LALT)
				    {
					    // Got a lister?
					    if (info->lister)
						    IPC_Command(info->lister->ipc,IPC_QUIT,0,0,0,0);
				    }
			    }

		    }
		}
	}

	// Something to run?
	else
	if (object->bdo_icon->do_Type==WBTOOL ||
		object->bdo_icon->do_Type==WBPROJECT)
	{
		char *default_tool=0;
		BOOL execute=0,ok=1;
		struct DiskObject *got_icon=0,*icon;

		// Get icon
		if (object->bdo_icon->do_Type==WBPROJECT &&
			(got_icon=GetDiskObject(object->bdo_name))) icon=got_icon;
		else icon=object->bdo_icon;

		// Is object a project?
		if (icon->do_Type==WBPROJECT)
		{
			// Valid tool?
			if (icon->do_DefaultTool && *icon->do_DefaultTool)
			{
				// Get default tool
				default_tool=icon->do_DefaultTool;

				// Trap more?
				if (file_trap_more(object->bdo_name,default_tool)) ok=0;

				// And ignore execute for fake icons
				else
				if (!got_icon &&
					strcmpi(FilePart(default_tool),"execute")==0) default_tool=0;
			}

			// No default tool?
			if (!default_tool)
			{
				struct FileInfoBlock __aligned fib;
				BPTR test;

				// Lock and examine file
				if (test=Lock(object->bdo_name,ACCESS_READ))
				{
					// Examine object
					Examine(test,&fib);
					UnLock(test);

					// Is S bit set?
					if (fib.fib_Protection&FIBF_SCRIPT)
					{
						// Execute it
						default_tool=object->bdo_name;
						execute=1;
					}
				}

				// If S bit isn't set, submit to filetypes
				if (!execute)
				{
					short action;

					Buttons *bmsg = 0;

					if (lamsg)
					{
					    bmsg = (Buttons *)lamsg;
					}
					else
					if (amsg)
					{
					    bmsg = (Buttons *)CopyAppMessage(amsg,global_memory_pool);
					}

					// Get filetype action
					if (qual&IEQUALIFIER_CONTROL) action=FTTYPE_CTRL_DOUBLECLICK;
					else
					if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) action=FTTYPE_ALT_DOUBLECLICK;
					else
					action=FTTYPE_DOUBLE_CLICK;

					// Do filetype action on file
					function_launch(
						FUNCTION_FILETYPE,
						0,
						action,
						(bmsg)?FUNCF_WBARG_PASSTRUGH:0,
						info->lister,0,
						object->bdo_name,0, //path,0,
						parent_lock,0,
						0, //BuildArgArray(object->bdo_name,0),
						0,
						bmsg);
				}
			}
		}
							
		// If it's a tool, see if icon exists at all
		else
		if (icon->do_Type==WBTOOL)
		{
			char buf[108];
			BPTR test;

			// Build icon name
			StrCombine(buf,object->bdo_name,".info",sizeof(buf));

			// Does icon exist?
			if (test=Lock(buf,ACCESS_READ))
			{
				// Yep, it's ok
				UnLock(test);
			}

			// No icon
			else
			{
				// Launch proc to run as AmigaDOS
				misc_startup("galileo_run",MENU_EXECUTE,info->window,(APTR)object->bdo_name,0);
				ok=0;
			}
		}

		// Ok to run?
		if (ok && (icon->do_Type==WBTOOL || default_tool))
		{
			char *launchprog;

			// Program to actually launch
			launchprog=(icon->do_Type==WBTOOL)?object->bdo_name:(char *)FilePart(default_tool);

			// Set screen title
			lsprintf(info->buffer,GetString(&locale,MSG_LAUNCHING_PROGRAM),launchprog);
			title_error(info->buffer,0);

			if (execute)
			{
			    Att_List *command_list;

			    // Allocate command list
			    if (command_list=Att_NewList(LISTF_POOL))
			    {
				    short len;
				    char *command;
				    struct Node *node;

				    // Arguments supplied?
				    if (numargs>0)
				    {
					    WORD arg;

					    // Go through arguments
					    for (arg=0;arg<numargs;arg++)
					    {
						    STRPTR path;
						    BPTR arg_lock = 0;

						    if (arglist[arg].wa_Lock)
							arg_lock = arglist[arg].wa_Lock;
						    else
						    if (lamsg && lamsg->glam_Lock)
						        arg_lock = lamsg->glam_Lock;

						    if (path = PathFromLock(NULL, arg_lock, PFLF_USE_DEVICENAME|PFLF_SLASH_APPEND, arglist[arg].wa_Name))
						    {
							Att_NewNode(command_list,path,0,0);
							FreeMemH(path);
						    }
					    }
				    }

				    // Selected objects as arguments?
				    else
				    if (numargs==0)
				    {
					    // Get arguments
					    backdrop_run_build_args(command_list,info,object);
				    }

				    // Get total length of arguments
				    for (node=command_list->list.lh_Head,len=0;node->ln_Succ;node=node->ln_Succ)
					    len+=strlen(node->ln_Name)+3;

				    // Add length of name
				    len+=strlen(object->bdo_name)+3;

				    // Execute
				    len+=8;

				    // Allocate buffer
				    if (command=AllocVec(len,MEMF_CLEAR))
				    {
					    char *ptr;
					    char output[140];
					    BPTR out;

					    // Build command
					    lsprintf(command,"execute \"%s\"",object->bdo_name);
					    //else lsprintf(command,"\"%s\"",object->bdo_name);

					    // Get pointer to end of string
					    ptr=command+strlen(command);

					    // Add arguments
					    for (node=command_list->list.lh_Head;node->ln_Succ;node=node->ln_Succ)
					    {
						    // Add space and name in quotes
						    *ptr++=' ';
						    *ptr++='\"';
						    strcpy(ptr,node->ln_Name);

						    // Bump pointer
						    ptr+=strlen(ptr);
						    *ptr++='\"';
						    *ptr=0;
					    }

					    // Open output
					    lsprintf(output,"%s%s/AUTO/CLOSE/WAIT/SCREEN %s",
						    environment->env->output_device,
						    environment->env->output_window,
						    get_our_pubscreen());
					    if (!(out=Open(output,MODE_OLDFILE)))
						    out=Open("nil:",MODE_OLDFILE);

					    // Run command
					    CLI_Launch(command,(struct Screen *)-1,0,out,0,LAUNCHF_USE_STACK,environment->env->default_stack, NULL);


					    // Free command buffer
					    FreeVec(command);
				    }

				    // Free arg list
				    Att_RemList(command_list,0);
			    }
			}
			// Workbench launch
			else
			{
				struct WBArg *args;
				ULONG args_size, stack_size;
				BOOL validlister;
				WORD argsnum = 1;

				if (info->lister && info->lister->cur_buffer && info->lister->cur_buffer->buf_Lock)
				    validlister = TRUE;
				else
				    validlister = FALSE;

				if (default_tool)
				    argsnum++;

				if (numargs > 0)
				    argsnum += numargs;
				else
				if (numargs == 0)
				{
				    BackdropObject *sel_object;

				    // Lock backdrop list
				    lock_listlock(&info->objects,0);

				    // Get number of selected icons
				    for (sel_object=(BackdropObject *)info->objects.list.lh_TailPred;
					    sel_object->bdo_node.ln_Pred;
					    sel_object=(BackdropObject *)sel_object->bdo_node.ln_Pred)
				    {
					    // Is object selected (and not the one we're running) with a valid path?
					    if (sel_object->bdo_state && sel_object!=object && (sel_object->bdo_path || validlister || sel_object->bdo_type==BDO_DISK))
					    {
						// Is icon for a disk?
						if (sel_object->bdo_type==BDO_DISK ||  sel_object->bdo_type==BDO_LEFT_OUT)
						    argsnum++;
					    }
				    }

				    // Unlock backdrop list
				    unlock_listlock(&info->objects);
				}


				args_size = sizeof(struct WBArg) * argsnum + 1;

				if (args = AllocVec(args_size,MEMF_CLEAR|MEMF_PUBLIC))
				{
				    struct LaunchWBArg wbargs;
				    STRPTR argnull = ((char *)args) + args_size;
				    WORD arg = 0;

				    if (default_tool)
					arg++;

				    if (args[arg].wa_Name = AllocVec(strlen(object->bdo_name)+1,MEMF_PUBLIC))
				    {
					BPTR arg_lock = 0;

				        // Copy name
					strcpy(args[arg].wa_Name,object->bdo_name);

				        // Get parent lock
				        if (object->bdo_parent_lock)
					    arg_lock = object->bdo_parent_lock;
				        else
				        if (object->bdo_flags&BDOF_DESKTOP_FOLDER)
					    arg_lock = GUI->desktop_dir_lock;
				        else
				        if (info->lister->cur_buffer->buf_Lock)
					    arg_lock = info->lister->cur_buffer->buf_Lock;

				        // And duplicate it
					args[arg].wa_Lock = DupLock(arg_lock);

					arg++;
				    }
				    else
				    {
				        ok = 0;
				    }

				    // Do we have arguments from drag and drop?
				    if (numargs > 0)
				    {
					while (arg < argsnum)
					{
					    BPTR org_dir;

					    if ((!(arglist[arg-1].wa_Name) || !(*arglist[arg-1].wa_Name)) ||
						(argdata && argdata[arg-1].glad_Flags&(AAEF_DIR|AAEF_DEV)))
					    {
						args[arg].wa_Name = argnull;

					    }
					    else
					    {
						if (args[arg].wa_Name = AllocVec(strlen(arglist[arg-1].wa_Name) + 1, MEMF_PUBLIC))
						{
						    strcpy(args[arg].wa_Name,arglist[arg-1].wa_Name);
						}
						else
						{
						    ok = 0;
						    break;
						}
					    }

					    if (ok)
					    {
						BPTR arg_lock = 0;

						if (arglist[arg-1].wa_Lock)
						    arg_lock = arglist[arg-1].wa_Lock;
						else
						if (lamsg && lamsg->glam_Lock)
						    arg_lock = lamsg->glam_Lock;

						if (argdata && argdata[arg-1].glad_Flags&AAEF_DIR)
						{
						    org_dir = CurrentDir(arg_lock);

						    args[arg].wa_Lock = Lock(arglist[arg-1].wa_Name, ACCESS_READ);
						    CurrentDir(org_dir);
						}
						else
						    args[arg].wa_Lock = DupLock(arg_lock);
					    }

					    arg++;
					}
				    }
				    else
				    if (numargs == 0)
				    {
				        BackdropObject *sel_object;

				        // Lock backdrop list
				        lock_listlock(&info->objects,0);

				        // Get number of selected icons
				        for (sel_object=(BackdropObject *)info->objects.list.lh_TailPred;
					        sel_object->bdo_node.ln_Pred;
					        sel_object=(BackdropObject *)sel_object->bdo_node.ln_Pred)
				        {
						BOOL sel_ok = 0;

					        // Is object selected (and not the one we're running) with a valid path?
						if (sel_object->bdo_state && sel_object!=object && (sel_object->bdo_path || validlister || sel_object->bdo_type==BDO_DISK))
					        {
						    BPTR org_dir, arg_lock = 0;
						    WORD type = 0;

						    if (sel_object->bdo_flags&BDOF_ASSIGN)
						    {
							args[arg].wa_Name = argnull;
							if (sel_object->bdo_flags&BDOF_ASSIGN_VOL)
							{
							    arg_lock = sel_object->bdo_parent_lock;
							}
							else
							{
							    arg_lock = sel_object->bdo_parent_lock;
							    type = 1;
							}
							sel_ok = 1;
						    }
						    // Is icon for a disk?
						    else
						    if (sel_object->bdo_type==BDO_DISK)
						    {
							args[arg].wa_Name = argnull;
							type = 2;
							sel_ok = 1;
						    }
						    // Is object a real one?
						    else
						    if (sel_object->bdo_type==BDO_LEFT_OUT)
						    {
						        // Get parent lock
						        if (sel_object->bdo_parent_lock)
							    arg_lock = sel_object->bdo_parent_lock;
						        else
						        if (sel_object->bdo_flags&BDOF_DESKTOP_FOLDER)
							    arg_lock = GUI->desktop_dir_lock;
						        else
						        if (info->lister->cur_buffer->buf_Lock)
							    arg_lock = info->lister->cur_buffer->buf_Lock;

							if (sel_object->bdo_icon->do_Type==WBDRAWER || sel_object->bdo_icon->do_Type==WBGARBAGE)
							{
							    args[arg].wa_Name = argnull;
							    type = 1;
							}
							sel_ok = 1;
						    }

						    if (!type && !(args[arg].wa_Name))
						    {
							if (args[arg].wa_Name = AllocVec(strlen(sel_object->bdo_name) + 1, MEMF_PUBLIC))
							{
							    strcpy(args[arg].wa_Name,sel_object->bdo_name);
							    sel_ok = 1;
							}
							else
							{
							    ok = 0;
							    break;
							}


						    }

						    if (arg_lock)
						    {
							if (!type)
							    args[arg].wa_Lock = DupLock(arg_lock);
							else
							{
							    org_dir = CurrentDir(arg_lock);
							    args[arg].wa_Lock = Lock((type == 2)?sel_object->bdo_device_name:sel_object->bdo_name, ACCESS_READ);
							    CurrentDir(org_dir);
							}
						    }

						    if (sel_ok)
							arg++;
					        }
				        }

				        // Unlock backdrop list
				        unlock_listlock(&info->objects);
				    }

				    if (object->bdo_icon->do_StackSize > environment->env->default_stack)
					stack_size = object->bdo_icon->do_StackSize;
				    else
					stack_size = environment->env->default_stack;

				    // Fill in data for launch-packet.
				    wbargs.lwba_args = args;
				    wbargs.lwba_numargs = argsnum;
				    wbargs.lwba_stack = stack_size;
				    wbargs.lwba_default_tool = default_tool;
				    if (object->bdo_icon->do_ToolWindow)
					wbargs.lwba_toolwindow = object->bdo_icon->do_ToolWindow;
				    else
					wbargs.lwba_toolwindow = 0;

				    if (!ok || !(WB_LaunchArg(&wbargs,GUI->screen_pointer,0,0,0,0)))
				    {
					FreeLaunchWBArgs(&args,argsnum);
				    }
				}
				//WB_LaunchNew(command,GUI->screen_pointer,0,environment->env->default_stack,default_tool);
			}

		}

		// Free icon
		if (got_icon) FreeDiskObject(got_icon);
	}

	// Restore directory
	CurrentDir(old);
	if (object->bdo_icon->do_Type==WBDISK)
	    UnLock(parent_lock);
}


// Build arguments from icons
void backdrop_run_build_args(Att_List *list,BackdropInfo *info,BackdropObject *exclude)
{
	BackdropObject *object;
	BOOL validlister;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	if (info->lister && info->lister->cur_buffer && info->lister->cur_buffer->buf_Lock)
	    validlister = TRUE;
	else
	    validlister = FALSE;

	// Go through backdrop list backwards (to maintain compatibility with Workbench icon ordering)
	for (object=(BackdropObject *)info->objects.list.lh_TailPred;
		object->bdo_node.ln_Pred;
		object=(BackdropObject *)object->bdo_node.ln_Pred)
	{
		// Is object selected (and not the one we're running) with a valid path?
		if (object->bdo_state && object!=exclude && (object->bdo_path || validlister || object->bdo_type==BDO_DISK))
		{
			BOOL ok=0;
			char *name = 0, *alloc_name = 0;

			// Is icon for a disk?
			if (object->bdo_type==BDO_DISK)
			{
			    name = object->bdo_device_name;
			    ok=1;
			}
			// Is object a real one?
			else
			if (object->bdo_type==BDO_LEFT_OUT)
			{
				// In a lister, it's ok
				if (validlister)
				{
				    name = object->bdo_name;
				    ok = 1;
				}
				else
				if (object->bdo_flags&BDOF_DESKTOP_FOLDER)
				{
				    if (alloc_name = AllocVec(strlen(environment->env->desktop_location) + strlen(object->bdo_name) + 2,MEMF_ANY))
				    {
					STRPTR tmp;

					tmp = stpcpy(alloc_name,environment->env->desktop_location);
					*tmp = '/';
					tmp++;
					strcpy(tmp,object->bdo_name);
					name = alloc_name;
					ok = 1;
				    }
				}
			}

			// Ok to use?
			if (ok)
			{
				// Add to list
				Att_NewNode(list,name,0,0);

				if (alloc_name)
				    FreeVec(alloc_name);
			}
		}
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}

/*
// Add to selection list
void backdrop_selection_list(BackdropInfo *info,BackdropObject *object)
{
	// Clear list?
	if (!object) Att_RemList(GUI->icon_sel_list,REMLIST_SAVELIST);

	// Add to list
	else
	{
		char name[256];

		// Disk?
		if (object->bdo_type==BDO_DISK)
			strcpy(name,object->bdo_device_name);

		// Build name
		else
		{
			strcpy(name,object->bdo_path);
			AddPart(name,object->bdo_name,256);
		}

		// Add to list
		Att_NewNode(GUI->icon_sel_list,name,(ULONG)info,ADDNODE_EXCLUSIVE);
	}
}
*/
