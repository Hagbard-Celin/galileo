/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
CopyRight 2024,2025 Hagbard Celine

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
#include "icon.strings"

enum
{
	GAD_BASE,

	GAD_ICON_AREA,
	GAD_ICON_IMAGE,
	GAD_ICON_FILE_INFO,
	GAD_INFO_ICON_TYPE,
	GAD_INFO_FILE_DIMS,
	GAD_INFO_FILE_COLORS,
	GAD_INFO_FILE_MORE,
	GAD_INFO_FILE_SIZE,
	GAD_ICON_SAVE,
	GAD_ICON_NEXT,
	GAD_ICON_CANCEL,
	GAD_BOTTOM_LINE,

	GAD_ICON_BLOCKS,
	GAD_ICON_BYTES,
	GAD_ICON_STACK,
	GAD_ICON_PROTECTION_AREA,
	GAD_ICON_PROTECTION_LAYOUT,
	GAD_ICON_PROTECTION_HS,
	GAD_ICON_PROTECTION_PA,
	GAD_ICON_PROTECTION_RW,
	GAD_ICON_PROTECTION_ED,
	GAD_ICON_TYPE,
	GAD_ICON_LAST_CHANGED,
	GAD_ICON_COMMENT,
	GAD_TOOLTYPES_TOOL_AREA,
	GAD_TOOLTYPES_PROJECT_AREA,
	GAD_ICON_TOOLTYPES,
	GAD_ICON_TOOLTYPES_EDIT,
	GAD_ICON_TOOLTYPES_NEW,
	GAD_ICON_TOOLTYPES_DELETE,
	GAD_ICON_TOOLTYPES_SORT,
	GAD_ICON_DEFAULT_TOOL_GLASS,
	GAD_ICON_DEFAULT_TOOL,
	GAD_RIGHT_LAYOUT_AREA,
	GAD_PCENT_FREE_LAYOUT_AREA,
	GAD_PCENT_USED_LAYOUT_AREA,
	GAD_PCENT_FREE,
	GAD_PCENT_USED,
	GAD_SIZE_LAYOUT_AREA,
	GAD_LOWER_LAYOUT_AREA,
	GAD_ICON_STATUS,
	GAD_DEVICE,
	GAD_STATE,
	GAD_ERRORS,
	GAD_FILESYSTEM,
	GAD_HANDLER,
	GAD_VOL_NAME,
	GAD_DEV_NAME,
	GAD_USED_KEY_AREA,
	GAD_USED_KEY,
	GAD_FREE_KEY_AREA,
	GAD_FREE_KEY,
	GAD_PCENT_USED_KEY_AREA,
	GAD_PCENT_USED_KEY,
	GAD_PCENT_FREE_KEY_AREA,
	GAD_PCENT_FREE_KEY,
	GAD_USED,
	GAD_USED_MB,
	GAD_FREE,
	GAD_FREE_MB,
	GAD_CAPACITY,
	GAD_CAPACITY_MB,
	GAD_GRAPH,
	GAD_BLOCKSIZE,

	MENU_BORDERLESS,
	MENU_NO_LABEL,
	MENU_STRIP_NEWICONS,

	GAD_ICON_NAME1,
	GAD_ICON_NAME2,
	GAD_ICON_NAME3,

	GAD_VIRTUAL_PROTBITLIST,

	MENU_STRIP_OLD,
	MENU_NEWICONS,
	MENU_OLDICONS,

	MENU_ICON_EDIT,
	MENU_ICON_PICK_EDITOR,
	MENU_DISK,
	MENU_DRAWER,
	MENU_TOOL,
	MENU_PROJECT,
	MENU_GARBAGE,

	MENU_COPY_OLDICON,
	MENU_COPY_NEWICON,
	MENU_COPY_BOTHICONS,
	MENU_COPY_TOOLTYPES,
};
