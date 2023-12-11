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

#include "galileofmlib.h"

PopUpItem *__asm L_PopUpNewItem(register __a0 PopUpHandle *,register __d0 ULONG,register __d1 ULONG,register __d2 ULONG);

// New PopUpHandle
PopUpHandle *__asm __saveds L_PopUpNewHandle(register __d0 ULONG userdata,register __a0 REF_CALLBACK (callback),register __a1 struct GalileoLocale *locale)
{
	PopUpHandle *handle;

	// Allocate handle and create pool
	if (!(handle=AllocVec(sizeof(PopUpHandle),MEMF_CLEAR)) ||
		!(handle->ph_Memory=L_NewMemHandle(1024,768,MEMF_CLEAR)))
	{
		// Failed
		FreeVec(handle);
		return 0;
	}

	// Initialise menu
	NewList((struct List *)&handle->ph_Menu);
	handle->ph_Menu.locale=locale;
	handle->ph_Menu.flags=POPUPMF_HELP;
	handle->ph_Menu.userdata=userdata;
	NewList((struct List *)&handle->ph_SubStack);

	// Call-back for refresh?
	if (callback)
	{
		handle->ph_Menu.callback=callback;
		handle->ph_Menu.flags|=POPUPMF_REFRESH;
	}
	return handle;
}


// Free PopUpHandle
void __asm __saveds L_PopUpFreeHandle(register __a0 PopUpHandle *handle)
{
	// Valid handle?
	if (handle)
	{
		// Free memory
		L_FreeMemHandle(handle->ph_Memory);
		FreeVec(handle);
	}
}


// Add item to a menu
PopUpItem *__asm __saveds L_PopUpNewItem(
	register __a0 PopUpHandle *handle,
	register __d0 ULONG string,
	register __d1 ULONG id,
	register __d2 ULONG flags)
{
	PopUpItem *item;

	// Allocate item
	if (item=L_AllocMemH(handle->ph_Memory,sizeof(PopUpItem)))
	{
		// Fill out item
		item->item_name=(char *)string;
		item->id=id;
		item->flags=flags&(~POPUPF_LOCALE);

		// Not bar label?
		if (string!=(ULONG)POPUP_BARLABEL)
		{
			// Locale ID supplied?
			if (!(flags&POPUPF_STRING))
			{
				// Get string
				item->item_name=L_GetString(handle->ph_Menu.locale,string);
			}
			else item->flags&=~POPUPF_STRING;
		}

		// Add to menu
		if (handle->ph_List)
			AddTail(handle->ph_List,(struct Node *)item);
		else
			AddTail((struct List *)&handle->ph_Menu,(struct Node *)item);
	}

	return item;
}


// Add separator
void __asm __saveds L_PopUpSeparator(register __a0 PopUpHandle *handle)
{
	// Add separator
	L_PopUpNewItem(handle,(ULONG)POPUP_BARLABEL,0,0);

	// Set flag
	handle->ph_Flags|=POPHF_SEP;
}


// Make sub-menu
BOOL __asm __saveds L_PopUpItemSub(register __a0 PopUpHandle *menu,register __a1 PopUpItem *item)
{
	SubStack *stack;

	// Create list for submenus, and stack entry
	if (!(item->data=L_AllocMemH(menu->ph_Memory,sizeof(struct MinList))) ||
		!(stack=L_AllocMemH(menu->ph_Memory,sizeof(SubStack))))
	{
		L_FreeMemH(item->data);
		item->data=0;
		return 0;
	}

	// Initialise list
	NewList((struct List *)item->data);

	// Add list pointer to stack
	stack->ss_List=menu->ph_List;
	AddHead((struct List *)&menu->ph_SubStack,(struct Node *)stack);

	// Get list pointer
	menu->ph_List=(struct List *)item->data;
	return 1;
}


// End sub-menu
void __asm __saveds L_PopUpEndSub(register __a0 PopUpHandle *menu)
{
	SubStack *stack;

	// Clear list pointer
	menu->ph_List=0;

	// Substack?
	if (!(IsListEmpty((struct List *)&menu->ph_SubStack)))
	{
		// Pop it off
		stack=(SubStack *)menu->ph_SubStack.mlh_Head;
		RemHead((struct List *)&menu->ph_SubStack);

		// Restore list pointer
		menu->ph_List=stack->ss_List;

		// Free stack entry
		L_FreeMemH(stack);
	}
}


// Set flags of an item (by ID only)
ULONG __asm __saveds L_PopUpSetFlags(register __a0 PopUpMenu *menu,register __d0 USHORT id,register __d1 ULONG value,register __d2 ULONG mask)
{
	PopUpItem *item;
	ULONG old=0;

	// Find item
	if (item=L_GetPopUpItem(menu,id))
	{
		// Save old flags
		old=item->flags;

		// Set new flags
		item->flags&=~mask;
		item->flags|=value;
	}

	return old;
}
