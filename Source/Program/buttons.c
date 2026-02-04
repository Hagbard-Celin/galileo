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
#include <gfm/semaphores_protos.h>
#include <gfm/button_protos.h>
#include <gfm/config_display.h>
#include "misc_protos.h"
#include "backdrop_protos.h"
#include "buttons_protos.h"

// Create a new button bank
Buttons *buttons_new(
	char *name,
	char *path,
	BPTR parent_lock,
	Cfg_ButtonBank *bank,
	struct IBox *box,
	short type,
	ULONG flags)
{
	Buttons *buttons;
	IPCData *ipc;
	APTR memhandle;
#ifdef _DEBUG
	char procname[25];
#endif

	// Memory handle
	if (!(memhandle=NewMemHandle(0,0,MEMF_CLEAR)))
	    return 0;

	// Allocate new buttons structure
	if (!(buttons=AllocMemH(memhandle,sizeof(Buttons))))
	{
		FreeMemHandle(memhandle);
		if (bank) CloseButtonBank(bank);
		return 0;
	}

	buttons->memory = memhandle;

	// Initialise icon position
	buttons->icon_pos_x=NO_ICON_POSITION;
	buttons->icon_pos_y=NO_ICON_POSITION;

	// Need to ask for file?
	if (name==(char *)-1) buttons->flags=BUTTONF_LOAD;

	// Bank supplied?
	else
	if (bank) buttons->bank=bank;

	// Filename supplied?
	else
	if (name)
	{
	    ULONG len;
	    char *tmp;

	    if (parent_lock)
		buttons->buttons_parent_lock = DupLock(parent_lock);

	    len = strlen(name) + 1;

	    if (path)
		len += strlen(path);

	    buttons->buttons_file = AllocMemH(memhandle, len);

	    if (path)
		tmp = stpcpy(buttons->buttons_file, path);
	    else
		tmp = buttons->buttons_file;

	    strcpy(tmp,name);
	}
	// Create new bank
	else
	if (!(buttons->bank=NewButtonBank(1,type)))
	{
		FreeMemHandle(memhandle);
		return 0;
	}

	// Carry some flags over
	buttons->flags|=flags&(BUTTONF_ICONIFIED|BUTTONF_TOOLBAR|BUTTONF_UNDERMOUSE|BUTTONF_FAIL);
		
	// Store position
	if (box)
	{
		buttons->pos=*box;
		buttons->flags|=BUTTONF_COORDS_SUPPLIED;
	}

	// Set first-time flag
	buttons->flags|=BUTTONF_FIRST_TIME;

#ifdef _DEBUG
	sprintf(procname, "galileo_buttons_%08lx", buttons);
#endif

	// Start buttons process
	if (!(IPC_Launch(
		&GUI->buttons_list,
		&ipc,
#ifdef _DEBUG
		procname,
#else
		"galileo_buttons",
#endif
		(ULONG)buttons_code,
		STACK_DEFAULT,
		(ULONG)buttons)))
	{
		// Free stuff if process never got off the ground
		if (!ipc)
		{
		    if (buttons->buttons_parent_lock)
			UnLock(buttons->buttons_parent_lock);
		    CloseButtonBank(buttons->bank);
		    FreeMemHandle(memhandle);
		}
		return 0;
	}

	return buttons;
}


// Update buttons data
void buttons_update(Buttons *buttons)
{
	if (buttons && buttons->bank)
	{
		// If window is open, get current position
		if (buttons->window)
		{
			buttons->pos.Left=buttons->window->LeftEdge;
			buttons->pos.Top=buttons->window->TopEdge;
			buttons->pos.Width=buttons->window->GZZWidth;
			buttons->pos.Height=buttons->window->GZZHeight;
		}

		// Update icon position
		buttons_update_icon(buttons);
	}
}


// Get button font
void buttons_get_font(Buttons *buttons)
{
	struct TextAttr attr;
	char name[80];

	// Close existing font
	if (buttons->font) CloseFont(buttons->font);

	// Lock bank
	GetSemaphore(&buttons->bank->lock,SEMF_SHARED,0);

	// Fill out attr
	strcpy(name,buttons->bank->window.font_name);
	attr.ta_Name=name;
	attr.ta_YSize=buttons->bank->window.font_size;
	attr.ta_Style=0;
	attr.ta_Flags=0;

	// Unlock bank
	FreeSemaphore(&buttons->bank->lock);

	// Get font
	buttons->font=OpenDiskFont(&attr);

	// Set font
	if (buttons->window && buttons->font)
		SetFont(buttons->window->RPort,buttons->font);
}


// Update icon position
void buttons_update_icon(Buttons *buttons)
{
	BackdropObject *object;

	// Got an icon?
	if (!buttons->appicon) return;

	// Lock main icon list
	lock_listlock(&GUI->backdrop->objects,0);

	// Find object
	if (object=backdrop_find_appicon(GUI->backdrop,(AppEntry *)buttons->appicon))
	{
		short x,y;
		ULONG iflags;

		// Get position
		x=object->bdo_pos.Left;
		y=object->bdo_pos.Top;

		// Adjust for borders?
		if (!((iflags=GetIconFlags(object->bdo_icon))&ICONF_BORDER_OFF) &&
			(!(environment->env->desktop_flags&DESKTOPF_NO_BORDERS) || (iflags&ICONF_BORDER_ON)))
		{
			// Shift back by border size
			x-=ICON_BORDER_X;
			y-=ICON_BORDER_Y_TOP;
		}

		// Save position
		buttons->icon_pos_x=x;
		buttons->icon_pos_y=y;
	}

	// Unlock main icon list
	unlock_listlock(&GUI->backdrop->objects);
}
