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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au

*/

#include "galileofm.h"
#include "search.h"

// Search window
ConfigWindow
	search_window={
		{POS_CENTER,POS_CENTER,48,6},
		{0,0,0,52}};

UWORD
	search_labels[]={
		MSG_SEARCH_PROMPT,
		MSG_SEARCH_LEAVE_SELECTED,
		MSG_SEARCH_OUTPUT,
		0};

// Tags
struct TagItem
	search_layout[]={
		{GTCustom_LayoutRel,GAD_SEARCH_LAYOUT},
		{TAG_DONE}},

	search_text_tags[]={
		{GTST_MaxChars,80},
		{TAG_MORE,(ULONG)search_layout}},

	search_result_tags[]={
		{GTCustom_LocaleLabels,(ULONG)search_labels},
		{TAG_MORE,(ULONG)search_layout}};


// Search objects
ObjectDef
	search_objects[]={

		// Search layout area
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{3,3,-3,-12},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_SEARCH_LAYOUT,
			0},

		// Search string
		{OD_GADGET,
			STRING_KIND,
			{0,1,SIZE_MAXIMUM,1},
			{4,10,-4,4},
			MSG_ENTER_SEARCH_STRING,
			PLACETEXT_ABOVE,
			GAD_SEARCH_TEXT,
			search_text_tags},

		// Case insensitive
		{OD_GADGET,
			CHECKBOX_KIND,
			{0,2,0,1},
			{4,16,26,4},
			MSG_SEARCH_NO_CASE,
			PLACETEXT_RIGHT,
			GAD_SEARCH_CASE,
			search_layout},

		// Wildcards
		{OD_GADGET,
			CHECKBOX_KIND,
			{0,3,0,1},
			{4,22,26,4},
			MSG_SEARCH_WILD,
			PLACETEXT_RIGHT,
			GAD_SEARCH_WILD,
			search_layout},

		// Whole words
		{OD_GADGET,
			CHECKBOX_KIND,
			{0,4,0,1},
			{4,28,26,4},
			MSG_SEARCH_ONLYWORD,
			PLACETEXT_RIGHT,
			GAD_SEARCH_ONLYWORD,
			search_layout},

		// Result
		{OD_GADGET,
			MX_KIND,
			{22,2,1,1},
			{30,16,7,1},
			0,
			PLACETEXT_RIGHT,
			GAD_SEARCH_RESULT,
			search_result_tags},

		// Okay
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,12,1},
			{3,-3,4,6},
			MSG_OK_BUTTON,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_SEARCH_OKAY,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,12,1},
			{-3,-3,4,6},
			MSG_CANCEL_BUTTON,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_SEARCH_CANCEL,
			0},

		{OD_END}};
