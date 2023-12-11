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
#include "key_finder.h"

ConfigWindow
	keyfinder_window={
		{POS_CENTER,POS_CENTER,60,10},
		{0,0,0,0}};

struct TagItem
	_keyfinder_key_taglist[]={
		{GTST_MaxChars,80},
		{TAG_END}},

	_keyfinder_func_taglist[]={
		{DLV_ReadOnly,TRUE},
		{DLV_ShowSeparators,TRUE},
		{TAG_END}};

ObjectDef
	keyfinder_objects[]={

		// Key field
		{OD_GADGET,
			HOTKEY_KIND,
			{8,0,SIZE_MAX_LESS-1,1},
			{4,4,-4,4},
			MSG_KEYFINDER_KEY,
			PLACETEXT_LEFT,
			GAD_KEYFINDER_KEY,
			_keyfinder_key_taglist},

		// Found in display
		{OD_GADGET,
			TEXT_KIND,
			{8,1,SIZE_MAX_LESS-1,1},
			{4,14,-4,6},
			MSG_KEYFINDER_FOUND,
			PLACETEXT_LEFT,
			GAD_KEYFINDER_FOUND,
			0},

		// Function display
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{8,2,SIZE_MAX_LESS-1,SIZE_MAXIMUM},
			{4,22,-4,-4},
			0,
			0,
			GAD_KEYFINDER_FUNCTION,
			_keyfinder_func_taglist},

		{OD_END}};
