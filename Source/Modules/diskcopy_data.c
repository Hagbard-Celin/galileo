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

#include "diskcopy.h"

ModuleInfo
	module_info={
		1,
		"diskcopy.gfmmodule",
		"diskcopy.catalog",
		0,
		1,
		{0,"DiskCopy",MSG_DISKCOPY_DESC,FUNCF_NO_ARGS,0}};

ConfigWindow
	diskcopy_window={
		{POS_CENTER,POS_CENTER,45,9},
		{0,0,44,36}};

struct TagItem

	diskcopy_layout_tags[]={
		{GTCustom_LayoutRel,GAD_DISKCOPY_LAYOUT},
		{TAG_DONE}},

	diskcopy_source_tags[]={
		{DLV_ShowSelected,0},
		{DLV_Check,1},
		{TAG_MORE,(ULONG)&diskcopy_layout_tags}},

	diskcopy_dest_tags[]={
		{DLV_MultiSelect,1},
		{TAG_MORE,(ULONG)&diskcopy_layout_tags}};


ObjectDef
	diskcopy_objects[]={

		// Background area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_DISKCOPY_LAYOUT,
			0},

		// Source lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,1,11,6},
			{4,4,24,4},
			MSG_DISKCOPY_FROM,
			0,
			GAD_DISKCOPY_SOURCE,
			diskcopy_source_tags},

		// Destination lister
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{POS_RIGHT_JUSTIFY,1,11,6},
			{-4,4,24,4},
			MSG_DISKCOPY_TO,
			0,
			GAD_DISKCOPY_DEST,
			diskcopy_dest_tags},

		// Verify
		{OD_GADGET,
			CHECKBOX_KIND,
			{13,3,0,1},
			{32,4,26,4},
			MSG_DISKCOPY_VERIFY,
			PLACETEXT_RIGHT,
			GAD_DISKCOPY_VERIFY,
			diskcopy_layout_tags},

		// Bump name
		{OD_GADGET,
			CHECKBOX_KIND,
			{13,4,0,1},
			{32,9,26,4},
			MSG_DISKCOPY_BUMP,
			PLACETEXT_RIGHT,
			GAD_DISKCOPY_BUMP,
			diskcopy_layout_tags},

		// Status area
		{OD_AREA,
			TEXTPEN,
			{0,7,SIZE_MAXIMUM,1},
			{4,12,-4,6},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE|TEXTFLAG_CENTER,
			GAD_DISKCOPY_STATUS,
			diskcopy_layout_tags},

		// Diskcopy
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_DISKCOPY_DISKCOPY,
			0,
			GAD_DISKCOPY_DISKCOPY,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_DISKCOPY_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_DISKCOPY_CANCEL,
			0},

		{OD_END}};
