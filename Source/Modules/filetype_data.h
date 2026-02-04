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

#define CATCOMP_NUMBERS
#include "filetype.strings"

#define FILENAME_MAXLEN	(30)
#define FILETYPE_MAXLEN	(31)
#define ID_MAXLEN	(7)
#define PRI_MAXLEN	(4)

enum
{
    GAD_FIND_LAYOUT,
    GAD_FIND_LISTVIEW,
    GAD_FIND_TEXT1,
    GAD_FIND_TEXT2,
    GAD_FIND_TEXT3,
    GAD_FIND_TEXT4,
    GAD_FIND_TEXT5,
    GAD_FIND_TEXT6,
    GAD_FIND_TEXT7,
    GAD_FIND_TEXT8,
    GAD_FIND_USE,
    GAD_FIND_INSTALL,
    GAD_FIND_CREATE,
    GAD_FIND_EDIT,
    GAD_FIND_CANCEL,

    GAD_CREATE_LAYOUT,
    GAD_CREATE_LISTVIEW,
    GAD_CREATE_ADD,
    GAD_CREATE_DELETE,
    GAD_CREATE_CLEAR,
    GAD_CREATE_FILETYPE,
    GAD_CREATE_NAME,
    GAD_CREATE_NAME_FIELD,
    GAD_CREATE_IFF,
    GAD_CREATE_IFF_FIELD,
    GAD_CREATE_GROUP,
    GAD_CREATE_GROUP_FIELD,
    GAD_CREATE_ID,
    GAD_CREATE_ID_FIELD,
    GAD_CREATE_BYTES,
    GAD_CREATE_BYTES_FIELD,
    GAD_CREATE_CYCLE,
    GAD_CREATE_EDIT,
    GAD_CREATE_SAVE,
    GAD_CREATE_CANCEL,
};
