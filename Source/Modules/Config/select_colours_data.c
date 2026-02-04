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

#include "config_lib.h"
#include "select_colours.h"
#include "enums.h"

#define CATCOMP_NUMBERS
#include "config.strings"

// Palette box window dimensions
ConfigWindow
	_palette_box_window={
		{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,16,5},
		{0,0,76,28}},

	_palette_box_cust_window={
		{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,20,7},
		{0,0,74,64}};


// Tag lists
struct TagItem
	_palette_box_tags[]={
		{GTCustom_LayoutRel,GAD_PALETTE_LAYOUT},
		{TAG_END,0}},

	_palette_slider_tags[]={
		{GA_RelVerify,TRUE},
		{GA_Immediate,TRUE},
		{TAG_MORE,(ULONG)_palette_box_tags}};


// Palette box objects
ObjectDef
	_palette_box_objects[]={

		// Layout object
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{4,4,-4,-4},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_PALETTE_LAYOUT,
			0},

		// Foreground
		{OD_GADGET,
			PALETTE_KIND,
			{0,1,8,SIZE_MAXIMUM},
			{4,8,24,-8},
			MSG_PALETTE_FOREGROUND,
			PLACETEXT_ABOVE,
			GAD_PALETTE_FOREGROUND,
			_palette_box_tags},

		// Background
		{OD_GADGET,
			PALETTE_KIND,
			{8,1,8,SIZE_MAXIMUM},
			{36,8,24,-8},
			MSG_PALETTE_BACKGROUND,
			PLACETEXT_ABOVE,
			GAD_PALETTE_BACKGROUND,
			_palette_box_tags},

		{OD_END}},

	_palette_box_cust_objects[]={

		// Layout object
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{2,2,-2,-10},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE,
			GAD_PALETTE_LAYOUT,
			0},

		// Foreground
		{OD_GADGET,
			PALETTE_KIND,
			{0,1,10,5},
			{4,4,24,8},
			MSG_PALETTE_FOREGROUND,
			PLACETEXT_ABOVE,
			GAD_PALETTE_FOREGROUND,
			_palette_box_tags},

		// Background
		{OD_GADGET,
			PALETTE_KIND,
			{10,1,10,5},
			{36,4,24,8},
			MSG_PALETTE_BACKGROUND,
			PLACETEXT_ABOVE,
			GAD_PALETTE_BACKGROUND,
			_palette_box_tags},

		// Red slider foreground
		{OD_GADGET,
			SLIDER_KIND,
			{0,6,10,0},
			{4,14,24,10},
			0,
			0,
			GAD_PALETTE_RED_FORE,
			_palette_slider_tags},

		// Green slider foreground
		{OD_GADGET,
			SLIDER_KIND,
			{0,6,10,0},
			{4,25,24,10},
			0,
			0,
			GAD_PALETTE_GREEN_FORE,
			_palette_slider_tags},

		// Blue slider foreground
		{OD_GADGET,
			SLIDER_KIND,
			{0,6,10,0},
			{4,36,24,10},
			0,
			0,
			GAD_PALETTE_BLUE_FORE,
			_palette_slider_tags},

		// Red slider background
		{OD_GADGET,
			SLIDER_KIND,
			{10,6,10,0},
			{36,14,24,10},
			0,
			0,
			GAD_PALETTE_RED_BACK,
			_palette_slider_tags},

		// Green slider background
		{OD_GADGET,
			SLIDER_KIND,
			{10,6,10,0},
			{36,25,24,10},
			0,
			0,
			GAD_PALETTE_GREEN_BACK,
			_palette_slider_tags},

		// Blue slider background
		{OD_GADGET,
			SLIDER_KIND,
			{10,6,10,0},
			{36,36,24,10},
			0,
			0,
			GAD_PALETTE_BLUE_BACK,
			_palette_slider_tags},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,POS_PROPORTION+48,1},
			{2,-2,0,6},
			MSG_USE,
			BUTTONFLAG_OKAY_BUTTON,
			GAD_PALETTE_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,POS_PROPORTION+48,1},
			{-2,-2,0,6},
			MSG_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON,
			GAD_PALETTE_CANCEL,
			0},

		{OD_END}};
