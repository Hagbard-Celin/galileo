/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023,2024 Hagbard Celine

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

#include <dos/dos.h>
#include <exec/types.h>
#include <libraries/gadtools.h>
#include <workbench/workbench.h>
#include <gfm/gadtools.h>
#include <gfm/glistview.h>
#include <gfm/configwin.h>
#include <gfm/filesystem_id.h>
#include "module_func_flags.h"
#include "moduleinfo.h"
#include "icon_data.h"

ModuleInfo
	module_info={
		1,
		"icon.gfmmodule",
		"icon.catalog",
		0,
		1,
		{0,"IconInfo",MSG_ICONINFO_DESC,FUNCF_NO_ARGS,0}};

ConfigWindow
	icon_disk_window={
		{POS_CENTER,POS_CENTER,58,19},
		{0,0,29,46}},

	icon_tool_window={
		{POS_CENTER,POS_CENTER,68,18},
		{0,0,12,58}},

	icon_project_window={
		{POS_CENTER,POS_CENTER,68,19},
		{0,0,12,65}},

	*icon_windows[]={
		&icon_disk_window,
		&icon_tool_window,
		&icon_tool_window,
		&icon_project_window,
		&icon_tool_window,};


struct TagItem
	icon_layout[]={
		{GTCustom_CopyTags,1},
		{TAG_END}},

	pos_icon_layout[]={
		{GTCustom_LayoutPos,GAD_ICON_AREA},
		{TAG_MORE,(ULONG)icon_layout}},

	pos_icon_disk_layout[]={
		{GTCustom_LayoutPos,GAD_ICON_AREA},
		{TAG_END}},

	right_area_layout[]={
		{GTCustom_LayoutRel,GAD_RIGHT_LAYOUT_AREA},
		{TAG_MORE,(ULONG)icon_layout}},

	right_area_disk_layout[]={
		{GTCustom_LayoutRel,GAD_RIGHT_LAYOUT_AREA},
		{TAG_END}},

	line_layout[]={
		{GTCustom_LayoutPos,GAD_ICON_SAVE},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_area_layout[]={
		{GTCustom_LayoutRel,GAD_ICON_AREA},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_file_layout[]={
		{GTCustom_LayoutRel,GAD_ICON_FILE_INFO},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_stack_tags[]={
		{GTIN_MaxChars,10},
		{TAG_MORE,(ULONG)pos_icon_layout}},

	icon_protection_tags[]={
		{GLV_MultiSelect,1},
		{GLV_NoScroller,2},
		{GLV_CompoundGadget,GAD_VIRTUAL_PROTBITLIST},
		{GTCustom_LayoutRel,GAD_ICON_PROTECTION_LAYOUT},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_protection_layout[]={
		{GTCustom_LayoutRel,GAD_ICON_PROTECTION_AREA},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_protection_tags_hs[]={
		{GLV_NoBorder,GLVNBF_RIGHT},
		{TAG_MORE,(ULONG)icon_protection_tags}},

	icon_protection_tags_parw[]={
		{GLV_NoBorder,GLVNBF_LEFT|GLVNBF_RIGHT},
		{TAG_MORE,(ULONG)icon_protection_tags}},

	icon_protection_tags_ed[]={
		{GLV_NoBorder,GLVNBF_LEFT},
		{TAG_MORE,(ULONG)icon_protection_tags}},

	details_layout[]={
		{GTCustom_LayoutRel,GAD_LOWER_LAYOUT_AREA},
		{TAG_END}},

	name_layout[]={
		{GTST_MaxChars,30},
		{GTCustom_PathFilter,1},
		{GTCustom_NoGhost,1},
		{TAG_MORE,(ULONG)details_layout}},

	space_layout[]={
		{GTCustom_LayoutRel,GAD_SIZE_LAYOUT_AREA},
		{TAG_END}},

	key_free_layout[]={
		{GTCustom_LayoutRel,GAD_FREE_KEY_AREA},
		{TAG_END}},

	key_layout[]={
		{GTCustom_LayoutRel,GAD_USED_KEY_AREA},
		{TAG_END}},

	used_pcent_key_layout[]={
		{GTCustom_LayoutRel,GAD_PCENT_USED_KEY_AREA},
		{TAG_END}},

	used_pcent_layout[]={
		{GTCustom_LayoutRel,GAD_PCENT_USED_LAYOUT_AREA},
		{TAG_END}},

	free_pcent_key_layout[]={
		{GTCustom_LayoutRel,GAD_PCENT_FREE_KEY_AREA},
		{TAG_END}},

	free_pcent_layout[]={
		{GTCustom_LayoutRel,GAD_PCENT_FREE_LAYOUT_AREA},
		{TAG_END}},

	icon_comment_tags[]={
		{GTST_MaxChars,79},
		{TAG_MORE,(ULONG)pos_icon_layout}},

	icon_tooltypes_tags[]={
		{GTCustom_LayoutPos,GAD_BOTTOM_LINE},
		{GLV_ShowSelected,0},
		{GLV_Flags,PLACETEXT_LEFT},
		{GLV_DragNotify,2},
		{GTST_MaxChars,255},
		{GTCustom_NoSelectNext,TRUE},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_tooltypes_layout_tags[]={
		{GTCustom_LayoutPos,GAD_ICON_COMMENT},
		{TAG_MORE,(ULONG)icon_tooltypes_tags}},

	icon_default_tool_tags[]={
		{GTCustom_LayoutPos,GAD_ICON_COMMENT},
		{GTST_MaxChars,255},
		{TAG_MORE,(ULONG)icon_layout}},

	icon_disk_default_tool_tags[]={
		{GTST_MaxChars,255},
		{TAG_MORE,(ULONG)details_layout}};


ObjectDef

	// Common to all icons
	icon_info_objects[]={

		// Icon frame area
		{OD_AREA,
			0,
			{0,0,22,12},
			{3,3,10,5},
			0,
			AREAFLAG_RECESSED|AREAFLAG_NOFILL,
			GAD_ICON_AREA,
			0},

		// Icon display area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,8},
			{0,0,0,3},
			0,
			/*AREAFLAG_RAISED|AREAFLAG_ERASE|AREAFLAG_FILL_COLOUR,*/
			/*AREAFLAG_RAISED|*/AREAFLAG_FILL_COLOUR|AREAFLAG_OPTIM/*|AREAFLAG_ICON*/,
			GAD_ICON_IMAGE,
			icon_area_layout},

		// Icon file info area
		{OD_AREA,
			0,
			{0,POS_RIGHT_JUSTIFY,SIZE_MAXIMUM,3},
			{0,0,0,8},
			0,
			AREAFLAG_RAISED|AREAFLAG_NOFILL,
			GAD_ICON_FILE_INFO,
			icon_area_layout},

		// Icon type field
		{OD_TEXT,
			TEXTPEN,
			{0,0,SIZE_MAXIMUM,1},
			{0,2,0,0},
			0,
			TEXTFLAG_CENTER|AREAFLAG_ERASE,
			GAD_INFO_ICON_TYPE,
			icon_file_layout},

		// Info dimensions
		{OD_TEXT,
			TEXTPEN,
			{7,1,0,1},
			{4,3,0,0},
			MSG_IMG_SIZE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			icon_file_layout},

		// Info dimensions field
		{OD_TEXT,
			TEXTPEN,
			{8,1,SIZE_MAXIMUM,1},
			{4,3,-2,0},
			0,
			AREAFLAG_ERASE,
			GAD_INFO_FILE_DIMS,
			icon_file_layout},

		// Icon colors
		{OD_TEXT,
			TEXTPEN,
			{7,2,0,1},
			{4,4,0,0},
			MSG_COLORS,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			icon_file_layout},

		// Number of icon colors
		{OD_TEXT,
			TEXTPEN,
			{8,2,4,1},
			{4,4,0,0},
			0,
			AREAFLAG_ERASE,
			GAD_INFO_FILE_COLORS,
			icon_file_layout},

		// More..
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,7,1},
			{0,0,1,2},
			MSG_MORE,
			BUTTONFLAG_THIN_BORDERS,
			GAD_INFO_FILE_MORE,
			icon_file_layout},

		// Save
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{8,-4,12,6},
			MSG_ICON_SAVE,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_ICON_SAVE,
			icon_layout},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-8,-4,12,6},
			MSG_ICON_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_ICON_CANCEL,
			icon_layout},

		// Line across the window
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,0},
			{0,-6,0,2},
			0,
			POSFLAG_ALIGN_POS_Y|AREAFLAG_GROOVE,
			GAD_BOTTOM_LINE,
			line_layout},

		{OD_END}},


	// Next gadget
	icon_info_next_object[]={

		{OD_GADGET,
			BUTTON_KIND,
			{POS_CENTER,POS_RIGHT_JUSTIFY,12,1},
			{0,-4,12,6},
			MSG_ICON_NEXT,
			0,
			GAD_ICON_NEXT,
			icon_layout},

		{OD_END}},


	// Tool/Project common objects
	icon_toolproj_right[]={

		// Right layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,12},
			{0,0,0,11},
			0,
			POSFLAG_ADJUST_POS_X|AREAFLAG_NOFILL,
			GAD_RIGHT_LAYOUT_AREA,
			pos_icon_layout},

		{OD_END}},


	// Blocks/Bytes/Stack objects
	icon_file_info_objects[]={


		// Bytes
		{OD_TEXT,
			TEXTPEN,
			{8,5,0,1},
			{4,17,0,0},
			MSG_SIZE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			right_area_layout},

		// Bytes field
		{OD_TEXT,
			TEXTPEN,
			{9,5,0,1},
			{4,17,0,0},
			0,
			0,
			GAD_ICON_BYTES,
			right_area_layout},

		// Blocks
		{OD_TEXT,
			TEXTPEN,
			{8,6,0,1},
			{4,20,0,0},
			MSG_BLOCKS,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			right_area_layout},

		// Blocks field
		{OD_TEXT,
			TEXTPEN,
			{9,6,0,1},
			{4,20,0,0},
			0,
			0,
			GAD_ICON_BLOCKS,
			right_area_layout},

		{OD_END}},


	// Tool/Project common objects
	icon_toolproj_objects[]={

		// Filename
		{OD_TEXT,
			TEXTPEN,
			{8,0,0,1},
			/*{0,POS_RIGHT_JUSTIFY,0,1},*/
			{4,6,0,0},
			/*{0,0,0,0},*/
			MSG_NAME,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			right_area_layout},

		// Filename output line 1
		{OD_TEXT,
			TEXTPEN,
			{9,0,0,1},
			{4,6,0,0},
			0,
			0,
			GAD_ICON_NAME1,
			right_area_layout},

		// Filename output line 2
		{OD_TEXT,
			TEXTPEN,
			{9,1,0,1},
			{4,7,0,0},
			0,
			0,
			GAD_ICON_NAME2,
			right_area_layout},

		// Filename output line 3
		{OD_TEXT,
			TEXTPEN,
			{9,2,0,1},
			{4,8,0,0},
			0,
			0,
			GAD_ICON_NAME3,
			right_area_layout},

		// Type
		{OD_TEXT,
			TEXTPEN,
			{8,3,0,1},
			/*{0,POS_RIGHT_JUSTIFY,0,1},*/
			{4,11,0,0},
			/*{0,0,0,0},*/
			MSG_TYPE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			right_area_layout},

		// Type field
		{OD_TEXT,
			TEXTPEN,
			{9,3,0,1},
			{4,11,0,0},
			0,
			0,
			GAD_ICON_TYPE,
			right_area_layout},

		// Last changed
		{OD_TEXT,
			TEXTPEN,
			{8,4,0,1},
			/*{0,POS_RIGHT_JUSTIFY,0,1},*/
			{4,14,0,0},
			/*{0,0,0,0},*/
			MSG_LAST_CHANGED,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			right_area_layout},

		// Last changed field
		{OD_TEXT,
			TEXTPEN,
			{9,4,0,1},
			{4,14,0,0},
			0,
			0,
			GAD_ICON_LAST_CHANGED,
			right_area_layout},

		// Protection bits lister area
		{OD_AREA,
			0,
			{0,7,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{0,20,0,0},
			0,
			AREAFLAG_NOFILL,
			GAD_ICON_PROTECTION_AREA,
			right_area_layout},

		// Protection bits lister layout
		{OD_AREA,
			0,
			{POS_CENTER,POS_CENTER,32,2},
			{0,0,82,6},
			0,
			AREAFLAG_NOFILL,
			GAD_ICON_PROTECTION_LAYOUT,
			icon_protection_layout},

		// Protection bits lister HS
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,0,6,2},
			{0,0,21,6},
			0,
			POSFLAG_ADJUST_POS_X,
			GAD_ICON_PROTECTION_HS,
			icon_protection_tags_hs},

		// Protection bits lister PA
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{POS_REL_RIGHT,0,8,2},
			{0,0,19,6},
			0,
			0,
			GAD_ICON_PROTECTION_PA,
			icon_protection_tags_parw},

		// Protection bits lister RW
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{POS_REL_RIGHT,0,8,2},
			{0,0,19,6},
			0,
			0,
			GAD_ICON_PROTECTION_RW,
			icon_protection_tags_parw},

		// Protection bits lister ED
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{POS_REL_RIGHT,0,10,2},
			{0,0,23,6},
			0,
			0,
			GAD_ICON_PROTECTION_ED,
			icon_protection_tags_ed},

		// Comment
		{OD_GADGET,
			STRING_KIND,
			{9,0,SIZE_MAXIMUM,1},
			{4,2,-4,6},
			MSG_COMMENT,
			POSFLAG_ADJUST_POS_Y,
			GAD_ICON_COMMENT,
			icon_comment_tags},

		// Tooltypes edit field
		{OD_GADGET,
			STRING_KIND,
			{20,POS_RIGHT_JUSTIFY-1,SIZE_MAXIMUM,1},
			{4,-19,-4,6},
			0,
			0,
			GAD_ICON_TOOLTYPES_EDIT,
			icon_tooltypes_tags},

		// Sort button
		{OD_GADGET,
			BUTTON_KIND,
			{9,POS_RIGHT_JUSTIFY-3,10,1},
			//{9,0,10,1},
			/*{4,-15,0,6},*/
			{4,-31,0,6},
			MSG_SORT,
			0,
			GAD_ICON_TOOLTYPES_SORT,
			icon_layout},

		// Delete button
		{OD_GADGET,
			BUTTON_KIND,
			{9,POS_RIGHT_JUSTIFY-2,10,1},
			//{9,0,10,1},
			/*{4,-8,0,6},*/
			{4,-25,0,6},
			MSG_DELETE,
			0,
			GAD_ICON_TOOLTYPES_DELETE,
			icon_layout},

		// New button
		{OD_GADGET,
			BUTTON_KIND,
			{9,POS_RIGHT_JUSTIFY-1,10,1},
			//{9,0,10,1},
			/*{4,-1,0,6},*/
			{4,-19,0,6},
			MSG_NEW,
			0,
			GAD_ICON_TOOLTYPES_NEW,
			icon_layout},

		{OD_END}},


	// Tool only objects
	icon_tooldir_objects[]={

		// Tooltypes
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{20,0,SIZE_MAXIMUM,SIZE_MAX_LESS-2},
			{4,1,-4,-26},
			MSG_TOOLTYPES,
			POSFLAG_ADJUST_POS_Y,
			GAD_ICON_TOOLTYPES,
			icon_tooltypes_layout_tags},

		{OD_END}},


	// Tool only objects
	icon_tool_objects[]={

		// Stack
		{OD_GADGET,
			INTEGER_KIND,
			{0,4,6,1},
			{4,11,12,6},
			MSG_STACK,
			POSFLAG_ALIGN_POS_X|POSFLAG_ADJUST_POS_Y|PLACETEXT_ABOVE,
			GAD_ICON_STACK,
			icon_stack_tags},

		{OD_END}},


	// Project gadgets
	icon_project_objects[]={

		// Default tool glass
		{OD_GADGET,
			FILE_BUTTON_KIND,
			/*{20,POS_RIGHT_JUSTIFY-7,0,1},*/
			{20,0,0,1},
			/*{4,-14,28,4},*/
			{4,1,28,6},
			MSG_DEFAULT_TOOL,
			POSFLAG_ADJUST_POS_Y,
			GAD_ICON_DEFAULT_TOOL_GLASS,
			icon_default_tool_tags},

		// Default tool
		{OD_GADGET,
			STRING_KIND,
			/*{20,POS_RIGHT_JUSTIFY-7,SIZE_MAXIMUM,1},*/
			{20,0,SIZE_MAXIMUM,1},
			/*{32,-14,-4,4},*/
			{32,1,-4,6},
			0,
			POSFLAG_ADJUST_POS_Y,
			GAD_ICON_DEFAULT_TOOL,
			icon_default_tool_tags},

		// Stack
		{OD_GADGET,
			INTEGER_KIND,
			{0,5,6,1},
			{4,18,12,6},
			MSG_STACK,
			POSFLAG_ALIGN_POS_X|POSFLAG_ADJUST_POS_Y|PLACETEXT_ABOVE,
			GAD_ICON_STACK,
			icon_stack_tags},

		// Tooltypes
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{20,1,SIZE_MAXIMUM,SIZE_MAX_LESS-2},
			{4,8,-4,-26},
			MSG_TOOLTYPES,
			POSFLAG_ADJUST_POS_Y,
			GAD_ICON_TOOLTYPES,
			icon_tooltypes_layout_tags},

		{OD_END}},


	// Disk objects
	icon_disk_objects[]={

		// Right frame area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,12},
			{2,0,-3,5},
			0,
			POSFLAG_ADJUST_POS_X|POSFLAG_ALIGN_POS_Y|AREAFLAG_RECESSED|AREAFLAG_NOFILL,
			GAD_RIGHT_LAYOUT_AREA,
			pos_icon_disk_layout},

		// Size frame area
		{OD_AREA,
			0,
			{0,8,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{0,3,0,0},
			0,
			AREAFLAG_RAISED|AREAFLAG_NOFILL,
			GAD_SIZE_LAYOUT_AREA,
			right_area_disk_layout},

		// Graph area
		{OD_AREA,
			0,
			{9,0,25,8},
			{2,1,0,0},
			0,
			AREAFLAG_NOFILL,//AREAFLAG_RAISED,
			GAD_GRAPH,
			right_area_disk_layout},

		// Used key layout area
		{OD_AREA,
			0,
			{0,0,1,1},
			{1,1,1,1},
			0,
			AREAFLAG_NOFILL,//AREAFLAG_ERASE,
			GAD_USED_KEY_AREA,
			space_layout},

		// Used key
		{OD_AREA,
			0,
			{POS_CENTER,POS_CENTER,1,POS_SQUARE},
			{0,0,-1,0},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED,
			GAD_USED_KEY,
			key_layout},

		// Used space
		{OD_TEXT,
			TEXTPEN,
			{6,0,0,1},
			{4,2,0,0},
			MSG_USED,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			space_layout},

		// Used space field
		{OD_TEXT,
			TEXTPEN,
			{7,0,21,1},
			{0,2,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_USED,
			space_layout},

		// Used space in meg
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY,0,8,1},
			{-3,2,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_USED_MB,
			space_layout},

		// Free key layout area
		{OD_AREA,
			0,
			{0,1,1,1},
			{1,2,1,1},
			0,
			AREAFLAG_NOFILL,//|AREAFLAG_ERASE,
			GAD_FREE_KEY_AREA,
			space_layout},

		// Free key
		{OD_AREA,
			0,
			{POS_CENTER,POS_CENTER,1,POS_SQUARE},
			{0,0,-1,0},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED,
			GAD_FREE_KEY,
			key_free_layout},

		// Free space
		{OD_TEXT,
			TEXTPEN,
			{6,1,0,1},
			{4,3,0,0},
			MSG_FREE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			space_layout},

		// Free space field
		{OD_TEXT,
			TEXTPEN,
			{7,1,21,1},
			{0,3,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_FREE,
			space_layout},

		// Free space in meg
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY,1,8,1},
			{-3,3,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_FREE_MB,
			space_layout},

		// Capacity
		{OD_TEXT,
			TEXTPEN,
			{6,2,0,1},
			{4,4,0,0},
			MSG_CAPACITY,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			space_layout},

		// Capacity field
		{OD_TEXT,
			TEXTPEN,
			{7,2,21,1},
			{0,5,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_CAPACITY,
			space_layout},

		// Capacity in meg
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY,2,8,1},
			{-3,5,0,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_CAPACITY_MB,
			space_layout},

		// Lower frame area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,7},
			{4,0,-4,28},
			0,
			POSFLAG_ADJUST_POS_Y|AREAFLAG_NOFILL,
			GAD_LOWER_LAYOUT_AREA,
			pos_icon_layout},

		// Name
		{OD_GADGET,
			STRING_KIND,
			{12,0,31,1},
			{-1,2,4,6},
			MSG_VOLNAME,
			PLACETEXT_LEFT,
			GAD_VOL_NAME,
			name_layout},

		// Devname
		{OD_TEXT,
			TEXTPEN,
			{POS_REL_RIGHT,0,0,1},
			{2,5,0,0},
			0,
			0,
			GAD_DEV_NAME,
			details_layout},

		// Created
		{OD_TEXT,
			TEXTPEN,
			{11,1,0,1},
			{-1,10,0,0},
			MSG_CREATED,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// Created field
		{OD_TEXT,
			TEXTPEN,
			{12,1,19,1},
			{-1,10,0,0},
			0,
			0,
			GAD_ICON_LAST_CHANGED,
			details_layout},

		// State
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY-11,1,0,1},
			{-2,10,0,0},
			MSG_STATE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// State field
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY,1,10,1},
			{-2,10,0,0},
			0,
			0,
			GAD_STATE,
			details_layout},

		// Device
		{OD_TEXT,
			TEXTPEN,
			{11,2,0,1},
			{-1,12,0,0},
			MSG_DEVICE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},


		// Device field
		{OD_TEXT,
			TEXTPEN,
			{12,2,30,1},
			{-1,12,0,0},
			0,
			0,
			GAD_DEVICE,
			details_layout},

		// Errors
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY-11,2,0,1},
			{-2,12,0,0},
			MSG_ERRORS,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// Errors field
		{OD_TEXT,
			TEXTPEN,
			{POS_RIGHT_JUSTIFY,2,10,1},
			{-2,12,0,0},
			0,
			0,
			GAD_ERRORS,
			details_layout},

		// Filesystem
		{OD_TEXT,
			TEXTPEN,
			{11,3,0,1},
			{-1,14,0,0},
			MSG_FILESYSTEM,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// Filesystem field
		{OD_TEXT,
			TEXTPEN,
			{12,3,SIZE_MAX_LESS-1,1},
			{-1,14,0,0},
			0,
			0,
			GAD_FILESYSTEM,
			details_layout},

		// Handler
		{OD_TEXT,
			TEXTPEN,
			{11,4,0,1},
			{-1,16,0,0},
			MSG_HANDLER,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// Handler field
		{OD_TEXT,
			TEXTPEN,
			{12,4,SIZE_MAX_LESS-1,1},
			{-1,16,0,0},
			0,
			0,
			GAD_HANDLER,
			details_layout},

		// Blocksize
		{OD_TEXT,
			TEXTPEN,
			{11,5,0,1},
			{-1,18,0,0},
			MSG_BLOCKSIZE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			details_layout},

		// Blocksize field
		{OD_TEXT,
			TEXTPEN,
			{12,5,21,1},
			{-1,18,0,0},
			0,
			0,
			GAD_BLOCKSIZE,
			details_layout},

		{OD_END}},

	// Percent Free
	icon_pcent_free_objects[]={

		// Persent free frame area
		{OD_AREA,
			0,
			{1,2,8,1},
			{2,-1,0,5},
			0,
			AREAFLAG_RAISED|AREAFLAG_NOFILL,
			GAD_PCENT_FREE_LAYOUT_AREA,
			right_area_layout},

		// Free percent key layout area
		{OD_AREA,
			0,
			{0,0,1,1},
			{1,2,0,0},
			0,
			AREAFLAG_NOFILL,
			GAD_PCENT_FREE_KEY_AREA,
			free_pcent_layout},

		// Free percent key
		{OD_AREA,
			0,
			{POS_CENTER,POS_CENTER,1,POS_SQUARE},
			{0,0,-2,0},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED,
			GAD_PCENT_FREE_KEY,
			free_pcent_key_layout},

		// Percent free field
		{OD_TEXT,
			TEXTPEN,
			{1,0,6,1},
			{1,2,2,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_PCENT_FREE,
			free_pcent_layout},

		{OD_END}},

	// Percent used
	icon_pcent_used_objects[]={

		// Persent used frame area
		{OD_AREA,
			0,
			{1,5,8,1},
			{2,-1,0,5},
			0,
			AREAFLAG_RAISED|AREAFLAG_NOFILL,
			GAD_PCENT_USED_LAYOUT_AREA,
			right_area_layout},

		// Used percent key layout area
		{OD_AREA,
			0,
			{0,0,1,1},
			{1,2,0,0},
			0,
			AREAFLAG_NOFILL,
			GAD_PCENT_USED_KEY_AREA,
			used_pcent_layout},

		// Used percent key
		{OD_AREA,
			0,
			{POS_CENTER,POS_CENTER,1,POS_SQUARE},
			{0,0,-2,0},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED,
			GAD_PCENT_USED_KEY,
			used_pcent_key_layout},

		// Percent used field
		{OD_TEXT,
			TEXTPEN,
			{1,0,6,1},
			{1,2,2,0},
			0,
			TEXTFLAG_RIGHT_JUSTIFY,
			GAD_PCENT_USED,
			used_pcent_layout},

		{OD_END}};


short
	diskinfo_colours[4][3]={
		{220,64,64},{64,64,220},
		{146,43,43},{43,43,146}};


short
	protect_lookup[]={
		FIBF_HOLD,
		FIBF_SCRIPT,
		FIBF_PURE,
		FIBF_ARCHIVE,
		FIBF_READ,
		FIBF_WRITE,
		FIBF_EXECUTE,
		FIBF_DELETE};

ULONG
	disktype_lookup[]={
		ID_DOS_DISK,MSG_DOSTYPE_0,
		ID_FFS_DISK,MSG_DOSTYPE_1,
		ID_INTER_DOS_DISK,MSG_DOSTYPE_2,
		ID_INTER_FFS_DISK,MSG_DOSTYPE_3,
		ID_FASTDIR_DOS_DISK,MSG_DOSTYPE_4,
		ID_FASTDIR_FFS_DISK,MSG_DOSTYPE_5,
		ID_LONG_DOS_DISK,MSG_DOSTYPE_6,
		ID_LONG_FFS_DISK,MSG_DOSTYPE_7,
		ID_MSDOS_DISK,MSG_DOSTYPE_MSDOS,
		ID_AFS_PRO,MSG_DOSTYPE_AFSP,
		ID_AFS_USER,MSG_DOSTYPE_AFSU,
		ID_AFS_MULTI,MSG_DOSTYPE_AFSM,
		ID_PFS_FLOPPY,MSG_DOSTYPE_PFSF,
		ID_PFS_HARD,MSG_DOSTYPE_PFSH,
		ID_SFS0_HARD,MSG_DOSTYPE_SFS0,
		ID_SFS2_HARD,MSG_DOSTYPE_SFS2,
		ID_ENVOY_DISK,MSG_DOSTYPE_ENVOY,
		ID_CDHFS_DISK,MSG_DOSTYPE_CDHFS,
		ID_CDISO_DISK,MSG_DOSTYPE_CDISO,
		ID_MUFS_INTER_FFS_DISK,MSG_DOSTYPE_MUFS_INTER_FFS,
		ID_MUFS_DOS_DISK,MSG_DOSTYPE_MUFS_DOS,
		ID_MUFS_FFS_DISK,MSG_DOSTYPE_MUFS_FFS,
		ID_MUFS_INTER_OFS,MSG_DOSTYPE_MUFS_INTER_DOS,
		ID_MUFS_INTER2_FFS_DISK,MSG_DOSTYPE_MUFS_INTER2_FFS,
		ID_MUFS_FASTDIR_DOS_DISK,MSG_DOSTYPE_MUFS_FASTDIR_DOS,
		ID_MUFS_FASTDIR_FFS_DISK,MSG_DOSTYPE_MUFS_FASTDIR_FFS,
		ID_PFS2_DISK,MSG_DOSTYPE_PFS2,
		ID_PFS3_DISK,MSG_DOSTYPE_PFS3,
		ID_PDS2_DISK,MSG_DOSTYPE_PDS2,
		ID_PDS3_DISK,MSG_DOSTYPE_PDS3,
		ID_MUPFS_DISK,MSG_DOSTYPE_MUPFS,
		ID_AMICDFS_DISK,MSG_DOSTYPE_AMICDFS,
		ID_ASIMCDFS_DISK,MSG_DOSTYPE_ASIMCDFS,
		ID_CDDA_DISK,MSG_DOSTYPE_CDDA,
		ID_FAT95_DISK,MSG_DOSTYPE_FAT95,
		ID_BFFS_DISK,MSG_DOSTYPE_BFFS,
		0,MSG_UNKNOWN};


short
	icon_type_labels[]={
		WBDISK,MSG_ICON_DISK,
		WBDRAWER,MSG_ICON_DRAWER,
		WBTOOL,MSG_ICON_TOOL,
		WBPROJECT,MSG_ICON_PROJECT,
		WBGARBAGE,MSG_ICON_GARBAGE,
		WBDEVICE,MSG_ICON_DEVICE,
		WBKICK,MSG_ICON_KICK,
		0};

UWORD __aligned
	oldicon_imagedata[1]={0xffff};

struct Image
	oldicon_image={
		0,0,1,1,1,oldicon_imagedata,1,0,0};
