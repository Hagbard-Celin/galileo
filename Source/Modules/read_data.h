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

#define CATCOMP_NUMBERS
#include "read.strings"

#define PEN_COUNT		8

enum
{
	MENU_BASE,
	MENU_NEXT,
	MENU_SEARCH,
	MENU_REPEAT,
	MENU_PRINT,
	MENU_QUIT,
	MENU_TAB_1,
	MENU_TAB_2,
	MENU_TAB_4,
	MENU_TAB_8,
	MENU_MODE_NORMAL,
	MENU_MODE_ANSI,
	MENU_MODE_HEX,
	MENU_SAVE,

	GAD_SEARCH_LAYOUT,
	GAD_SEARCH_TEXT,
	GAD_SEARCH_CASE,
	GAD_SEARCH_WILD,
	GAD_SEARCH_ONLYWORD,
	GAD_SEARCH_REVERSE,
	GAD_SEARCH_OKAY,
	GAD_SEARCH_CANCEL,

	MENU_USE_SCREEN,
	MENU_SCREEN_MODE,
	MENU_SELECT_FONT,

	MENU_SAVE_AS,
	MENU_TO_EDITOR,
	MENU_SELECT_EDITOR_NORMAL,
	MENU_SELECT_EDITOR_ANSI,
	MENU_SELECT_EDITOR_HEX,
};
