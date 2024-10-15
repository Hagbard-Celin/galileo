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

#include "recyclebin.h"

ModuleInfo
	module_info={
		1,
		"recyclebin.gfmmodule",
		"recyclebin.catalog",
		MODULEF_CALL_STARTUP,
		1,
		{CMD_DELETE,"DeleteFile",0,0,0}};


ConfigWindow
	recyclebin_window={
		{POS_CENTER,POS_CENTER,48,10},
		{0,0,74,26}};


struct TagItem

	recyclebin_layout_tags[]={
		{GTCustom_LayoutRel,GAD_RECYCLEBIN_LAYOUT},
		{TAG_END}},

	recyclebin_storage_tags[]={
		{GLV_ShowSelected,1},
		{GLV_DragNotify,1},
		{TAG_MORE,(ULONG)recyclebin_layout_tags}};		


ObjectDef
	recyclebin_objects[]={

		// Background area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{2,2,-2,-2},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_RECYCLEBIN_LAYOUT,
			0},

		// Storage lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,1,24,8},
			{4,4,24,4},
			MSG_RECYCLEBIN_STORAGE,
			0,
			GAD_RECYCLEBIN_STORAGE,
			recyclebin_storage_tags},

		// Add
		{OD_GADGET,
			BUTTON_KIND,
			{0,9,12,1},
			{4,8,12,6},
			MSG_RECYCLEBIN_ADD,
			0,
			GAD_RECYCLEBIN_ADD,
			recyclebin_layout_tags},

		// Delete
		{OD_GADGET,
			BUTTON_KIND,
			{12,9,12,1},
			{16,8,12,6},
			MSG_RECYCLEBIN_DELETE,
			0,
			GAD_RECYCLEBIN_DELETE,
			recyclebin_layout_tags},

		// Statistics title
		{OD_TEXT,
			TEXTPEN,
			{24,0,0,1},
			{38,3,0,0},
			MSG_RECYCLEBIN_STATS,
			0,
			0,
			recyclebin_layout_tags},

		// Statistics
		{OD_AREA,
			0,
			{24,1,24,8},
			{38,4,24,4},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_RECYCLEBIN_STATS,
			recyclebin_layout_tags},

		// Empty
		{OD_GADGET,
			BUTTON_KIND,
			{24,9,24,1},
			{38,8,24,6},
			MSG_RECYCLEBIN_EMPTY,
			0,
			GAD_RECYCLEBIN_EMPTY,
			recyclebin_layout_tags},

		{OD_END}};
