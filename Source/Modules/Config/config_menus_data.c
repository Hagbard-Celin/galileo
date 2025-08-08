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

#include "config_lib.h"

ConfigWindow
	config_menu_window={
		{POS_CENTER,POS_CENTER,54,14},
		{0,0,32,44}},

	config_menukeys_window={
		{POS_CENTER,POS_CENTER,44,13},
		{0,0,24,38}};

struct TagItem
	config_menu_layout_tags[]={
		{GTCustom_LayoutRel,GAD_MENUS_LAYOUT},
		{TAG_DONE}},

	config_menu_list_tags[]={
		{GLV_ShowSelected,0},
		{GLV_DragNotify,2},
		{GLV_ShowSeparators,1},
		{TAG_MORE,(ULONG)config_menu_layout_tags}},

	config_menukeys_list_tags[]={
		{GLV_ShowSelected,0},
		{GLV_ShowSeparators,1},
		{TAG_MORE,(ULONG)config_menu_layout_tags}},

	config_menu_name_tags[]={
		{GTST_MaxChars,64},
		{GTCustom_NoSelectNext,TRUE},
		{TAG_MORE,(ULONG)config_menu_layout_tags}},

	config_menu_key_tags[]={
		{GTST_MaxChars,1},
		{GTCustom_NoSelectNext,TRUE},
		{GTCustom_UpperCase,TRUE},
		{TAG_MORE,(ULONG)config_menu_layout_tags}};


ObjectDef
	config_menu_objects[]={

		// Layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_MENUS_LAYOUT,
			0},

		// Menu lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,1,18,9},
			{4,4,0,4},
			MSG_MENUS_MENU_TITLE,
			0,
			GAD_MENUS_MENU,
			config_menu_list_tags},

		// Item lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{18,1,18,9},
			{12,4,0,4},
			MSG_MENUS_ITEM_TITLE,
			0,
			GAD_MENUS_ITEM,
			config_menu_list_tags},

		// Sub-item lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{36,1,18,9},
			{20,4,0,4},
			MSG_MENUS_SUB_TITLE,
			0,
			GAD_MENUS_SUB,
			config_menu_list_tags},

		// Menu name
		{OD_GADGET,
			STRING_KIND,
			{0,10,18,1},
			{4,8,0,6},
			0,
			0,
			GAD_MENUS_MENU_NAME,
			config_menu_name_tags},

		// Item name
		{OD_GADGET,
			STRING_KIND,
			{18,10,18,1},
			{12,8,0,6},
			0,
			0,
			GAD_MENUS_ITEM_NAME,
			config_menu_name_tags},

		// Sub-item name
		{OD_GADGET,
			STRING_KIND,
			{36,10,18,1},
			{20,8,0,6},
			0,
			0,
			GAD_MENUS_SUB_NAME,
			config_menu_name_tags},

		// Add menu
		{OD_GADGET,
			BUTTON_KIND,
			{0,11,6,1},
			{4,14,0,6},
			MSG_MENUS_ADD,
			0,
			GAD_MENUS_MENU_ADD,
			config_menu_layout_tags},

		// Copy menu
		{OD_GADGET,
			BUTTON_KIND,
			{6,11,6,1},
			{4,14,0,6},
			MSG_MENUS_COPY,
			0,
			GAD_MENUS_MENU_COPY,
			config_menu_layout_tags},

		// Del menu
		{OD_GADGET,
			BUTTON_KIND,
			{12,11,6,1},
			{4,14,0,6},
			MSG_MENUS_DEL,
			0,
			GAD_MENUS_MENU_DEL,
			config_menu_layout_tags},

		// Edit menu
		{OD_GADGET,
			BUTTON_KIND,
			{0,12,6,1},
			{4,20,0,6},
			MSG_MENUS_EDIT,
			0,
			GAD_MENUS_MENU_EDIT,
			config_menu_layout_tags},

		// Move up menu
		{OD_GADGET,
			BUTTON_KIND,
			{6,12,6,1},
			{4,20,0,6},
			MSG_MENUS_UP,
			0,
			GAD_MENUS_MENU_UP,
			config_menu_layout_tags},

		// Move down menu
		{OD_GADGET,
			BUTTON_KIND,
			{12,12,6,1},
			{4,20,0,6},
			MSG_MENUS_DOWN,
			0,
			GAD_MENUS_MENU_DOWN,
			config_menu_layout_tags},

		// Add item
		{OD_GADGET,
			BUTTON_KIND,
			{18,11,6,1},
			{12,14,0,6},
			MSG_MENUS_ADD,
			0,
			GAD_MENUS_ITEM_ADD,
			config_menu_layout_tags},

		// Copy menu
		{OD_GADGET,
			BUTTON_KIND,
			{24,11,6,1},
			{12,14,0,6},
			MSG_MENUS_COPY,
			0,
			GAD_MENUS_ITEM_COPY,
			config_menu_layout_tags},

		// Del menu
		{OD_GADGET,
			BUTTON_KIND,
			{30,11,6,1},
			{12,14,0,6},
			MSG_MENUS_DEL,
			0,
			GAD_MENUS_ITEM_DEL,
			config_menu_layout_tags},

		// Edit menu
		{OD_GADGET,
			BUTTON_KIND,
			{18,12,6,1},
			{12,20,0,6},
			MSG_MENUS_EDIT,
			0,
			GAD_MENUS_ITEM_EDIT,
			config_menu_layout_tags},

		// Move up menu
		{OD_GADGET,
			BUTTON_KIND,
			{24,12,6,1},
			{12,20,0,6},
			MSG_MENUS_UP,
			0,
			GAD_MENUS_ITEM_UP,
			config_menu_layout_tags},

		// Move down menu
		{OD_GADGET,
			BUTTON_KIND,
			{30,12,6,1},
			{12,20,0,6},
			MSG_MENUS_DOWN,
			0,
			GAD_MENUS_ITEM_DOWN,
			config_menu_layout_tags},

		// Add sub
		{OD_GADGET,
			BUTTON_KIND,
			{36,11,6,1},
			{20,14,0,6},
			MSG_MENUS_ADD,
			0,
			GAD_MENUS_SUB_ADD,
			config_menu_layout_tags},

		// Copy sub
		{OD_GADGET,
			BUTTON_KIND,
			{42,11,6,1},
			{20,14,0,6},
			MSG_MENUS_COPY,
			0,
			GAD_MENUS_SUB_COPY,
			config_menu_layout_tags},

		// Del sub
		{OD_GADGET,
			BUTTON_KIND,
			{48,11,6,1},
			{20,14,0,6},
			MSG_MENUS_DEL,
			0,
			GAD_MENUS_SUB_DEL,
			config_menu_layout_tags},

		// Edit menu
		{OD_GADGET,
			BUTTON_KIND,
			{36,12,6,1},
			{20,20,0,6},
			MSG_MENUS_EDIT,
			0,
			GAD_MENUS_SUB_EDIT,
			config_menu_layout_tags},

		// Move up menu
		{OD_GADGET,
			BUTTON_KIND,
			{42,12,6,1},
			{20,20,0,6},
			MSG_MENUS_UP,
			0,
			GAD_MENUS_SUB_UP,
			config_menu_layout_tags},

		// Move down menu
		{OD_GADGET,
			BUTTON_KIND,
			{48,12,6,1},
			{20,20,0,6},
			MSG_MENUS_DOWN,
			0,
			GAD_MENUS_SUB_DOWN,
			config_menu_layout_tags},

		// Save
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_SAVE,
			0,
			GAD_MENUS_SAVE,
			0},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{POS_CENTER,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_USE,
			0,
			GAD_MENUS_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_LISTER_BUTTONS_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_MENUS_CANCEL,
			0},

		{OD_END}};

ObjectDef
	config_menukeys_objects[]={

		// Layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_MENUS_LAYOUT,
			0},

		// Menu lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,1,22,9},
			{4,4,0,4},
			MSG_MENUS_MENU_TITLE,
			0,
			GAD_MENUS_MENU,
			config_menukeys_list_tags},

		// Item lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{22,1,22,9},
			{12,4,0,4},
			MSG_MENUS_ITEM_TITLE,
			0,
			GAD_MENUS_ITEM,
			config_menukeys_list_tags},

		// Menu key
		{OD_GADGET,
			STRING_KIND,
			{22,11,5,1},
			{12,8,0,4},
			MSG_MENUS_MENU_KEY,
			0,
			GAD_MENUS_MENU_KEY,
			config_menu_key_tags},

		// Save
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,10,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_SAVE,
			0,
			GAD_MENUS_SAVE,
			0},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{POS_CENTER,POS_RIGHT_JUSTIFY,10,1},
			{2,-2,12,6},
			MSG_LISTER_BUTTONS_USE,
			0,
			GAD_MENUS_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,10,1},
			{-2,-2,12,6},
			MSG_LISTER_BUTTONS_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_MENUS_CANCEL,
			0},

		{OD_END}};

long
	config_menus_title[]={
		MSG_MENUS_TITLE,MSG_USER_MENU_DEF_TITLE,
		MSG_MENUS_LISTER_TITLE,MSG_LISTER_MENU_DEF_TITLE,
		MSG_MENUKEYS_TITLE,MSG_MENUKEYS_DEF_TITLE,
		MSG_MENUS_START_TITLE,MSG_START_DEF_TITLE};

char
	*config_menus_default[]={
		"PROGDIR:buttons/user menu_default",
		"PROGDIR:buttons/lister menu_default",
		"PROGDIR:system/menu keys_default",
		"PROGDIR:buttons/start menu_default"};
