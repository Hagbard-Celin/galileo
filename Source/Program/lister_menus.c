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

#define set_item(item,true) if(true){item->Flags|=CHECKED;}else{item->Flags&=~CHECKED;}
#define off_item(item,true) if(true){item->Flags&=~ITEMENABLED;}else{item->Flags|=ITEMENABLED;}

// Fix menu ghosting for a lister
void lister_fix_menus(Lister *lister,BOOL sel_only)
{
	struct MenuItem *item;
	struct Menu *menu;
	BOOL busy=0,icon=0,action=0,sel=0;
	short a;

	// Get menu pointer
	if (!lister_valid_window(lister) || !(menu=lister->window->MenuStrip))
		return;

	// Clear menu strip
	ClearMenuStrip(lister->window);

	// Get flags
	if (lister->flags&LISTERF_LOCK) busy=1;
	if (lister->flags&LISTERF_VIEW_ICONS) icon=1;
	if (lister->flags&LISTERF_ICON_ACTION) action=1;

	// In icon mode?
	if (icon)
	{
		BackdropObject *icon;

		// Lock icon list
		lock_listlock(&lister->backdrop_info->objects,FALSE);

		// Go through list
		for (icon=(BackdropObject *)lister->backdrop_info->objects.list.lh_Head;
			icon->bdo_node.ln_Succ;
			icon=(BackdropObject *)icon->bdo_node.ln_Succ)
		{
			// Something selected?
			if (icon->bdo_state)
			{
				// Set flag and break
				sel=1;
				break;
			}
		}

		// Unlock icon list
		unlock_listlock(&lister->backdrop_info->objects);
	}

/*
	// Text mode
	else
	{
		// Anything selected?
		if (lister->cur_buffer->buf_SelectedFiles[0]>0 ||
			lister->cur_buffer->buf_SelectedDirs[0]>0) sel=1;
	}
*/

	// Not just doing things dependant on selected icon?
	if (!sel_only)
	{
		// Show as name (set)
		if (item=find_menu_item(menu,MENU_LISTER_VIEW_NAME))
			set_item(item,!icon);

		// Show as icon (set)
		if (item=find_menu_item(menu,MENU_LISTER_VIEW_ICON))
			set_item(item,icon && !action);

		// Icon action (set)
		if (item=find_menu_item(menu,MENU_LISTER_ICON_ACTION))
			set_item(item,icon && action);

		// Show all
		if (item=find_menu_item(menu,MENU_LISTER_SHOW_ALL))
			set_item(item,icon && lister->flags&LISTERF_SHOW_ALL);

		// Close (disabled when busy)
		if (item=find_menu_item(menu,MENU_LISTER_CLOSE))
			off_item(item,busy);

		// Lister states (disabled when busy, or in icon - not action - mode)
		for (a=MENU_LISTER_SOURCE;a<=MENU_LISTER_OFF;a++)
		{
			// Find item
			if (item=find_menu_item(menu,a))
				off_item(item,busy || (icon && !action));
		}

		// Edit (disabled when busy or in icon mode)
		if (item=find_menu_item(menu,MENU_EDIT_LISTER))
			off_item(item,busy || icon);

		// New drawer (disabled when busy)
		if (item=find_menu_item(menu,MENU_ICON_MAKEDIR))
			off_item(item,busy);

		// Snapshot (disabled when busy)
		if (item=find_menu_item(menu,MENU_ICON_SNAPSHOT))
			off_item(item,busy);

		// Select all (disabled when busy)
		if (item=find_menu_item(menu,MENU_ICON_SELECT_ALL))
			off_item(item,busy);

		// Clean up (disabled when busy or not in icon mode)
		if (item=find_menu_item(menu,MENU_ICON_CLEANUP))
			off_item(item,busy || !icon);

		// Open parent (disabled when there's no parent)
		if (item=find_menu_item(menu,MENU_OPEN_PARENT))
		{
			BOOL dis=0;

			// Does path end in a colon?
			if (lister->cur_buffer->buf_Path[strlen(lister->cur_buffer->buf_Path)-1]==':')
				dis=1;

			// Do disable
			off_item(item,dis);
		}
	}

	// Open icon (disabled when busy or nothing selected)
	if (item=find_menu_item(menu,MENU_ICON_OPEN))
		off_item(item,busy || !sel);

	// Information (disabled when busy or nothing selected)
	if (item=find_menu_item(menu,MENU_ICON_INFO))
		off_item(item,busy || !sel);

	// Snapshot icon (disabled when nothing selected)
	if (item=find_menu_item(menu,MENU_ICON_SNAPSHOT_ICON))
		off_item(item,!sel);

	// Unsnapshot (disabled when busy or nothing selected)
	if (item=find_menu_item(menu,MENU_ICON_UNSNAPSHOT))
		off_item(item,busy || !sel);

	// Leave out (disabled when busy or nothing selected in icon mode)
	if (item=find_menu_item(menu,MENU_ICON_LEAVE_OUT))
		off_item(item,busy || (!sel && icon));

	// Rename (disabled when busy or nothing selected in icon mode)
	if (item=find_menu_item(menu,MENU_ICON_RENAME))
		off_item(item,busy || (!sel && icon));

	// Delete (disabled when busy or nothing selected in icon mode)
	if (item=find_menu_item(menu,MENU_ICON_DELETE))
		off_item(item,busy || (!sel && icon));

	// Reset menu strip
	ResetMenuStrip(lister->window,menu);
}


UWORD lister_listerpopup(Lister *lister,UWORD code)
{
	short a,num=0;
	UWORD res;
	PopUpHandle *menu;
	PopUpItem *item;
	PopUpExt *ext;

	// Allocate menu handle
	if (!(menu=PopUpNewHandle((ULONG)lister,lister->backdrop_info->callback,&locale)))
		return (UWORD)-1;

	// Build default lister popup
	for (a=0;lister_popup_data[a];a+=2)
	{
		if (lister_popup_data[a]==(UWORD)-1)
			PopUpSeparator(menu);
		else
		if (item=PopUpNewItem(menu,lister_popup_data[a],lister_popup_data[a+1],0))
		{
			// Kludge for 'lock position'
			if (item->id==MENU_LISTER_LOCK_POS)
				item->flags|=POPUPF_CHECKIT|((lister->flags&LISTERF_LOCK_POS)?POPUPF_CHECKED:0);
		}
	}

	// Clear 'separator' flag
	menu->ph_Flags&=~POPHF_SEP;

	// Lock extension list
	lock_listlock(&GUI->popupext_list,FALSE);

	// Go through list
	for (ext=(PopUpExt *)GUI->popupext_list.list.lh_Head;
		ext->pe_Node.ln_Succ;
		ext=(PopUpExt *)ext->pe_Node.ln_Succ)
	{
		// Lister?
		if (ext->pe_Type==POPUP_LISTER2)
		{
			// Check custom handler
			if (!(ext->pe_Flags&POPUPEXTF_HANDLER) ||
				strcmp(ext->pe_Command,lister->cur_buffer->buf_CustomHandler)==0)
			{
				// Add a separator if needed
				if (!(menu->ph_Flags&POPHF_SEP))
					PopUpSeparator(menu);

				// Allocate item
				if (item=PopUpNewItem(
					menu,
					(ULONG)ext->pe_Menu,
					MENU_EXTENSION+num,
					POPUPF_STRING))
				{
					// Set data pointer
					item->data=ext;
					++num;
				}
			}
		}
	}

	// Do popup menu
	item=0;
	if ((res=DoPopUpMenu(lister->window,&menu->ph_Menu,&item,code))!=(UWORD)-1 &&
		res>=MENU_EXTENSION)
	{
		// Got valid pointer?
		if (item && item->data)
		{
			PopUpExt *ext=(PopUpExt *)item->data;

			// Custom handler in lister?
			if (strcmp(lister->cur_buffer->buf_CustomHandler,ext->pe_Command)==0)
			{
				// Send message
				rexx_handler_msg(
					0,
					lister->cur_buffer,
					RXMF_WARN,
					HA_String,0,ext->pe_Menu,
					HA_Value,1,lister,
					TAG_END);
			}

			// Normal extension
			else
			{
				// Call popup function
				popup_run_func(item->data,0,0,lister);
			}

			// Clear result code
			res=(UWORD)-1;
		}
	}

	// Unlock extension list
	unlock_listlock(&GUI->popupext_list);

	// Free menu data
	PopUpFreeHandle(menu);
	return res;
}
