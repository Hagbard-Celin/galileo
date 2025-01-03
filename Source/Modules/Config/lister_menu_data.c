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

#include "config_lib.h"

ConfigWindow
	hotkeys_window={
		{POS_CENTER,POS_CENTER,64,12},
		{0,0,32,70}},

	scripts_window={
		{POS_CENTER,POS_CENTER,44,8},
		{0,0,28,70}};

struct TagItem
	_lister_menu_layout[]={
		{GTCustom_CopyTags,1},
		{GTCustom_LayoutRel,GAD_LISTER_MENU_LAYOUT},
		{TAG_DONE}},

	_lister_menu_items[]={
		{GLV_ShowSelected,0},
		{GLV_DragNotify,2},
		{TAG_MORE,(ULONG)&_lister_menu_layout}},

	script_list_items[]={
		{GLV_ShowSelected,0},
		{TAG_MORE,(ULONG)&_lister_menu_layout}},

	_lister_menu_name[]={
		{GTST_MaxChars,64},
		{TAG_MORE,(ULONG)&_lister_menu_layout}};

ObjectDef
	hotkeys_objects[]={

		// Layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_LISTER_MENU_LAYOUT,
			0},

		// Add
		{OD_GADGET,
			BUTTON_KIND,
			{0,1,10,1},
			{4,4,12,6},
			MSG_LISTER_MENU_ADD,
			0,
			GAD_LISTER_MENU_ADD,
			_lister_menu_layout},

		// Insert
		{OD_GADGET,
			BUTTON_KIND,
			{0,2,10,1},
			{4,12,12,6},
			MSG_LISTER_MENU_INSERT,
			0,
			GAD_LISTER_MENU_INSERT,
			_lister_menu_layout},

		// Duplicate
		{OD_GADGET,
			BUTTON_KIND,
			{0,3,10,1},
			{4,20,12,6},
			MSG_LISTER_MENU_DUPLICATE,
			0,
			GAD_LISTER_MENU_DUPLICATE,
			_lister_menu_layout},

		// Delete
		{OD_GADGET,
			BUTTON_KIND,
			{0,4,10,1},
			{4,28,12,6},
			MSG_LISTER_MENU_REMOVE,
			0,
			GAD_LISTER_MENU_REMOVE,
			_lister_menu_layout},

		// Edit
		{OD_GADGET,
			BUTTON_KIND,
			{0,5,10,1},
			{4,36,12,6},
			MSG_LISTER_MENU_EDIT,
			0,
			GAD_LISTER_MENU_EDIT,
			_lister_menu_layout},

		// Menu lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{10,1,SIZE_MAXIMUM,SIZE_MAX_LESS-2},
			{20,4,-4,-14},
			MSG_LISTER_MENU_ITEMS,
			LISTVIEWFLAG_CURSOR_KEYS,
			GAD_LISTER_MENU_ITEMS,
			_lister_menu_items},

		// Item name
		{OD_GADGET,
			STRING_KIND,	
			{10,POS_RIGHT_JUSTIFY-1,SIZE_MAXIMUM,1},
			{20,-10,-4,4},
			MSG_LISTER_MENU_NAME,
			0,
			GAD_LISTER_MENU_NAME,
			_lister_menu_name},

		// Global
		{OD_GADGET,
			CHECKBOX_KIND,
			{10,POS_RIGHT_JUSTIFY,0,1},
			{20,-4,26,4},
			MSG_HOTKEYS_GLOBAL,
			PLACETEXT_RIGHT,
			GAD_HOTKEYS_GLOBAL,
			_lister_menu_layout},

		// Save
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_SAVE,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_LISTER_MENU_SAVE,
			0},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{POS_CENTER,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_USE,
			0,
			GAD_LISTER_MENU_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_LISTER_BUTTONS_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_LISTER_MENU_CANCEL,
			0},

		{OD_END}},

	scripts_objects[]={

		// Layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_LISTER_MENU_LAYOUT,
			0},

		// Save
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_SAVE,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_LISTER_MENU_SAVE,
			0},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{POS_CENTER,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_USE,
			0,
			GAD_LISTER_MENU_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_LISTER_BUTTONS_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_LISTER_MENU_CANCEL,
			0},

		// Delete
		{OD_GADGET,
			BUTTON_KIND,
			{0,1,10,1},
			{4,4,12,6},
			MSG_LISTER_MENU_REMOVE,
			0,
			GAD_LISTER_MENU_REMOVE,
			_lister_menu_layout},

		// Edit
		{OD_GADGET,
			BUTTON_KIND,
			{0,2,10,1},
			{4,12,12,6},
			MSG_LISTER_MENU_EDIT,
			0,
			GAD_LISTER_MENU_EDIT,
			_lister_menu_layout},

		// Menu lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{10,1,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{20,4,-4,-4},
			MSG_SCRIPTS,
			LISTVIEWFLAG_CURSOR_KEYS,
			GAD_LISTER_MENU_ITEMS,
			script_list_items},

		{OD_END}};


MenuData
	_lister_menu_menus[]={
		{NM_TITLE,0,MSG_PROJECT,0},
		{NM_ITEM,MENU_LISTER_MENU_NEW,MSG_LISTER_BUTTONS_MENU_NEW,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('N')},
		{NM_ITEM,MENU_LISTER_MENU_OPEN,MSG_LISTER_BUTTONS_MENU_OPEN,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('O')},
		{NM_ITEM,0,(ULONG)NM_BARLABEL,0},
		{NM_ITEM,MENU_LISTER_MENU_SAVE,MSG_LISTER_BUTTONS_MENU_SAVE,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('S')},
		{NM_ITEM,MENU_LISTER_MENU_SAVEAS,MSG_LISTER_BUTTONS_MENU_SAVEAS,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('A')},
		{NM_ITEM,0,(ULONG)NM_BARLABEL,0},
		{NM_ITEM,GAD_LISTER_MENU_CANCEL,MSG_LISTER_BUTTONS_MENU_QUIT,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('Q')},
		{NM_TITLE,0,MSG_BUTTONS_EDIT,0},
		{NM_ITEM,MENU_LISTER_RESET_DEFAULTS,MSG_RESET_DEFAULTS,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('D')},
		{NM_ITEM,MENU_LISTER_LAST_SAVED,MSG_LAST_SAVED,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('L')},
		{NM_ITEM,MENU_LISTER_RESTORE,MSG_RESTORE,MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('R')},
		{NM_END}};
