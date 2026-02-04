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

#include <exec/types.h>
#include <libraries/gadtools.h>
#include <gfm/gadtools.h>
#include <gfm/configwin.h>
#include "module_func_flags.h"
#include "moduleinfo.h"
#include "play_data.h"

ModuleInfo
	module_info={
		1,
		"play.gfmmodule",
		"play.catalog",
		0,
		1,
		{0,"Play",MSG_PLAY_DESC,FUNCF_NO_ARGS,0}};

ConfigWindow
	play_window={
		{POS_CENTER,POS_CENTER,40,5},
		{0,0,8,18}};


struct TagItem
	play_layout[]={
		{GTCustom_LayoutRel,GAD_PLAY_LAYOUT},
		{TAG_END}};


ObjectDef
	play_objects[]={

		// Information area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_PLAY_LAYOUT,
			0},

		// Filename title
		{OD_TEXT,
			TEXTPEN,
			{8,0,0,1},
			{2,2,0,0},
			MSG_FILENAME,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			play_layout},

		// Filename
		{OD_AREA,
			TEXTPEN,
			{9,0,SIZE_MAXIMUM,1},
			{2,2,-4,0},
			0,
			AREAFLAG_ERASE,
			GAD_PLAY_FILENAME,
			play_layout},

		// Length title
		{OD_TEXT,
			TEXTPEN,
			{8,1,0,1},
			{2,2,0,0},
			MSG_LENGTH,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			play_layout},

		// Length
		{OD_AREA,
			TEXTPEN,
			{9,1,SIZE_MAXIMUM,1},
			{2,2,-4,0},
			0,
			AREAFLAG_ERASE,
			GAD_PLAY_LENGTH,
			play_layout},

		// Type title
		{OD_TEXT,
			TEXTPEN,
			{8,2,0,1},
			{2,2,0,0},
			MSG_TYPE,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			play_layout},

		// Type
		{OD_AREA,
			TEXTPEN,
			{9,2,SIZE_MAXIMUM,1},
			{2,2,-4,0},
			0,
			AREAFLAG_ERASE,
			GAD_PLAY_TYPE,
			play_layout},

		// Status title
		{OD_TEXT,
			TEXTPEN,
			{8,3,0,0},
			{2,2,0,0},
			MSG_STATUS,
			TEXTFLAG_RIGHT_JUSTIFY,
			0,
			play_layout},

		// Status
		{OD_AREA,
			TEXTPEN,
			{9,3,SIZE_MAXIMUM,1},
			{2,2,-4,0},
			0,
			AREAFLAG_ERASE,
			GAD_PLAY_STATUS,
			play_layout},

		// Next button
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{2,-2,12,6},
			MSG_NEXT,
			0,
			GAD_PLAY_NEXT,
			0},

		// Abort
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-2,-2,12,6},
			MSG_ABORT,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_PLAY_ABORT,
			0},

		{OD_END}};
