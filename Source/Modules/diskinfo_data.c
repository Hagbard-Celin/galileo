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

#include "diskinfo.h"

ConfigWindow
	diskinfo_win={
		{POS_CENTER,POS_CENTER,48,19},
		{0,0,0,74}};

ModuleInfo
	module_info={
		56,
		"diskinfo.gfmmodule",
		"diskinfo.catalog",
		MODULEF_CATALOG_VERSION,
		1,
		{0,"DiskInfo",MSG_DISKINFO_DESC,FUNCF_NEED_SOURCE|FUNCF_NO_ARGS,0}};


struct TagItem
	diskinfo_layout[]={
		{GTCustom_LayoutRel,DISKINFO_LAYOUT},
		{TAG_DONE}},

	button_tags[]={
		{GTCustom_ThinBorders,TRUE},
		{TAG_END}},

	name_tags[]={
		{GTST_MaxChars,31},
		{GTCustom_PathFilter,1},
		{TAG_MORE,(ULONG)diskinfo_layout}},

	key_tags[]={
		{GTCustom_LayoutPos,GAD_NAME},
		{TAG_MORE,(ULONG)diskinfo_layout}},

	handler_tags[]={
		{GTCustom_Borderless,TRUE},
		{GTCustom_Justify,JUSTIFY_LEFT},
		{TAG_MORE,(ULONG)key_tags}},

	space_tags[]={
		{GTCustom_Borderless,TRUE},
		{GTCustom_Justify,JUSTIFY_RIGHT},
		{TAG_MORE,(ULONG)key_tags}},

	errors_tags[]={
		{GTCustom_Borderless,TRUE},
		{GTCustom_Justify,JUSTIFY_RIGHT},
		{TAG_MORE,(ULONG)diskinfo_layout}},

	used_mb_tags[]={
		{GTCustom_LayoutPos,GAD_USED},
		{TAG_MORE,(ULONG)errors_tags}},

	free_mb_tags[]={
		{GTCustom_LayoutPos,GAD_FREE},
		{TAG_MORE,(ULONG)errors_tags}},

	capacity_mb_tags[]={
		{GTCustom_LayoutPos,GAD_CAPACITY},
		{TAG_MORE,(ULONG)errors_tags}};

// Disk info objects
ObjectDef
	diskinfo_objects[]={

		// Layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE|AREAFLAG_THIN,
			DISKINFO_LAYOUT,
			0},

		// Name
		{OD_GADGET,
			STRING_KIND,
			{9,0,20,1},
			{4,4,4,6},
			MSG_NAME,
			PLACETEXT_LEFT,
			GAD_NAME,
			name_tags},

		// Type
		{OD_GADGET,
			TEXT_KIND,
			{0,0,SIZE_MAXIMUM,1},
			{0,4,-2,4},
			MSG_TYPE,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_TYPE,
			handler_tags},

		// Handler
		{OD_GADGET,
			TEXT_KIND,
			{0,1,SIZE_MAXIMUM,1},
			{0,10,-2,4},
			MSG_HANDLER,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_HANDLER,
			handler_tags},

		// State
		{OD_GADGET,
			TEXT_KIND,
			{0,2,SIZE_MAXIMUM,1},
			{0,16,-2,4},
			MSG_STATE,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_STATE,
			handler_tags},

		// Used key
		{OD_AREA,
			0,
			{1,4,POS_SQUARE,1},
			{4,18,0,4},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED|POSFLAG_ADJUST_POS_Y,
			GAD_USED_KEY,
			key_tags},

		// Used space
		{OD_GADGET,
			TEXT_KIND,
			{0,4,25,1},
			{0,20,0,0},
			MSG_USED,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_USED,
			space_tags},

		// Used space in meg
		{OD_GADGET,
			TEXT_KIND,
			{4,0,8,1},
			{0,0,-2,0},
			0,
			POSFLAG_ADJUST_POS_X|POSFLAG_ALIGN_POS_Y,
			GAD_USED_MB,
			used_mb_tags},

		// Free key
		{OD_AREA,
			0,
			{1,5,POS_SQUARE,1},
			{4,26,0,4},
			0,
			AREAFLAG_ERASE|AREAFLAG_RECESSED|POSFLAG_ADJUST_POS_Y,
			GAD_FREE_KEY,
			key_tags},

		// Free space
		{OD_GADGET,
			TEXT_KIND,
			{0,5,25,1},
			{0,28,0,0},
			MSG_FREE,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_FREE,
			space_tags},

		// Free space in meg
		{OD_GADGET,
			TEXT_KIND,
			{4,0,8,1},
			{0,0,-2,0},
			0,
			POSFLAG_ADJUST_POS_X|POSFLAG_ALIGN_POS_Y,
			GAD_FREE_MB,
			free_mb_tags},

		// Line across the window
		{OD_AREA,
			0,
			{1,6,SIZE_MAX_LESS-1,0},
			{4,38,-4,2},
			0,
			POSFLAG_ADJUST_POS_Y|AREAFLAG_LINE,
			0,
			key_tags},

		// Capacity
		{OD_GADGET,
			TEXT_KIND,
			{0,6,25,1},
			{0,48,0,0},
			MSG_CAPACITY,
			POSFLAG_ADJUST_POS_Y|POSFLAG_ALIGN_POS_X,
			GAD_CAPACITY,
			space_tags},

		// Capacity in meg
		{OD_GADGET,
			TEXT_KIND,
			{4,0,8,1},
			{0,0,-2,0},
			0,
			POSFLAG_ADJUST_POS_X|POSFLAG_ALIGN_POS_Y,
			GAD_CAPACITY_MB,
			capacity_mb_tags},

		// Graph area
		{OD_AREA,
			0,
			{POS_CENTER,POS_RIGHT_JUSTIFY-2,24,5},
			{0,-6,20,10},
			0,
			0,
			GAD_GRAPH,
			diskinfo_layout},

		// Errors display
		{OD_GADGET,
			TEXT_KIND,
			{POS_RIGHT_JUSTIFY-2,POS_RIGHT_JUSTIFY,14,1},
			{-4,-6,0,0},
			0,
			0,
			GAD_ERRORS,
			errors_tags},

		// Ok
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_OK,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_OK,
			button_tags},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_CANCEL,
			button_tags},

		{OD_END}};


short
	diskinfo_colours[4][3]={
		{220,64,64},{64,64,220},
		{146,43,43},{43,43,146}};

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
		0,0};
