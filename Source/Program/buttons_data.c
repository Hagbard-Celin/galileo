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
#include "menu_data.h"

// Menu for toolbar button banks
MenuData
	button_toolbar_menu[]={
		{NM_TITLE,0,MSG_PROJECT,0},
		{NM_ITEM,MENU_TOOLBAR_BUTTONS_NEW,MSG_NEW_LISTER_MENU,MENUFLAG_COMM_SEQ},
		{NM_ITEM,MENU_OPEN_BUTTONS_LOCAL,MSG_OPEN,MENUFLAG_COMM_SEQ},
		{NM_ITEM,0,(ULONG)NM_BARLABEL,0},
		{NM_ITEM,MENU_SAVE_BUTTONS,MSG_SAVE_MENU,MENUFLAG_COMM_SEQ},
		{NM_ITEM,MENU_SAVEAS_BUTTONS,MSG_SAVEAS_MENU,MENUFLAG_COMM_SEQ},
		{NM_ITEM,0,(ULONG)NM_BARLABEL,0},
		{NM_ITEM,MENU_CLOSE_BUTTONS,MSG_QUIT,MENUFLAG_COMM_SEQ},
		{NM_TITLE,0,MSG_EDIT,0},
		{NM_ITEM,MENU_TOOLBAR_RESET_DEFAULTS,MSG_DEFAULTS,MENUFLAG_COMM_SEQ|MENUFLAG_USE_SEQ|MENUFLAG_MAKE_SEQ('D')},
		{NM_ITEM,MENU_TOOLBAR_LAST_SAVED,MSG_LAST_SAVED,MENUFLAG_COMM_SEQ},
		{NM_ITEM,MENU_TOOLBAR_RESTORE,MSG_RESTORE,MENUFLAG_COMM_SEQ},
		{NM_END}};
