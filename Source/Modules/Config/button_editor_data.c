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

#include <libraries/gadtools.h>
#include <gfm/configwin.h>
#include <gfm/gadtools.h>
#include <gfm/gadget_tags.h>
#include <gfm/windowdata.h>
#include "config_lib.h"
#include "select_colours.h"
#include "button_editor.h"
#include "enums.h"

#define CATCOMP_NUMBERS
#include "config.strings"

// Button editor window dimensions
ConfigWindow
	_button_editor_window={
		{POS_MOUSE_CENTER,0,50,9},
		{0,0,52,93}};


// Call-back hook to get task pointer
static void __asm button_editor_task_callback(
	register __a1 struct TagItem *tag,
	register __a2 struct Window *window)
{
	// Get task pointer
	tag->ti_Tag=GTCustom_ChangeSigTask;
	tag->ti_Data=(ULONG)FindTask(0);
}


// Call-back hook to get signal bit
static void __asm button_editor_bit_callback(
	register __a1 struct TagItem *tag,
	register __a2 struct Window *window)
{
	ButtonEdData *data;

	// Get data pointer
	data=(ButtonEdData *)CFGDATA(window);

	// Get signal bit
	tag->ti_Tag=GTCustom_ChangeSigBit;
	tag->ti_Data=(ULONG)data->change_bit;
}


// Taglists
struct TagItem

	_button_editor_relative[]={
		{GTCustom_LayoutRel,GAD_BUTTONED_LAYOUT_AREA},
		{TAG_END}},

	_button_editor_function_layout[]={
		{GTCustom_LayoutRel,GAD_BUTTONED_LAYOUT},
		{TAG_END}},

	_button_editor_palette_layout[]={
		{GTCustom_LayoutRel,GAD_BUTTONED_PALETTE},
		{TAG_END}},

	_button_editor_functions[]={
		{GLV_ShowSelected,TRUE},
		{GLV_DragNotify,1},
		{GLV_ShowChecks,2},
		{TAG_MORE,(ULONG)_button_editor_function_layout}},

	_button_editor_name[]={
		{GTST_MaxChars,255},
		{GTCustom_ThinBorders,TRUE},
		{TAG_MORE,(ULONG)_button_editor_function_layout}},

	_button_editor_label[]={
		{GTST_MaxChars,255},
		{GTCustom_CallBack,(ULONG)button_editor_task_callback},
		{GTCustom_CallBack,(ULONG)button_editor_bit_callback},
		{GTCustom_ThinBorders,TRUE},
		{TAG_MORE,(ULONG)_button_editor_function_layout}},

	_button_editor_image[]={
		{GTCustom_Control,GAD_BUTTONED_LABEL},
		{GFB_DefPath,(ULONG)"PROGDIR:images/"},
		{TAG_MORE,(ULONG)_button_editor_function_layout}};


// Button editor objects
ObjectDef
	_button_editor_objects[]={

		// Layout box
		{OD_AREA,
			0,
			{0,0,SIZE_MAXIMUM,SIZE_MAX_LESS-1},
			{3,3,-3,-12},
			0,
			AREAFLAG_RECESSED|AREAFLAG_ERASE|AREAFLAG_THIN,
			GAD_BUTTONED_LAYOUT_AREA,
			0},

		// Function area
		{OD_AREA,
			TEXTPEN,
			{0,1,SIZE_MAXIMUM,4},
			{6,-2,-6,20},
			MSG_BUTTONED_FUNCTIONS,
			AREAFLAG_TITLE,
			GAD_BUTTONED_LAYOUT,
			_button_editor_relative},

		// Function list
		{OD_GADGET,
			MY_LISTVIEW_KIND,
			{0,0,16,SIZE_MAX_LESS-1},
			{4,0,24,-8},
			0,
			LISTVIEWFLAG_CURSOR_KEYS,
			GAD_BUTTONED_FUNCTIONS,
			_button_editor_functions},

		// Add
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,8,1},
			{4,-4,12,4},
			MSG_BUTTONED_ADD,
			BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_ADD,
			_button_editor_function_layout},

		// Del
		{OD_GADGET,
			BUTTON_KIND,
			{8,POS_RIGHT_JUSTIFY,8,1},
			{16,-4,12,4},
			MSG_BUTTONED_DEL,
			BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_DEL,
			_button_editor_function_layout},

		// Name
		{OD_GADGET,
			STRING_KIND,
			{24,0,SIZE_MAXIMUM,1},
			{32,0,-4,6},
			MSG_BUTTONED_NAME,
			PLACETEXT_LEFT,
			GAD_BUTTONED_NAME,
			_button_editor_name},

		// Edit function
		{OD_GADGET,
			BUTTON_KIND,
			{24,2,SIZE_MAXIMUM,1},
			{32,16,-4,6},
			MSG_BUTTONED_EDIT_FUNCTION,
			BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_EDIT_FUNCTION,
			_button_editor_function_layout},

		// Palette area
		{OD_AREA,
			TEXTPEN,
			{0,6,20,SIZE_MAXIMUM},
			{6,22,54,-6},
			MSG_BUTTONED_PALETTE,
			AREAFLAG_TITLE,
			GAD_BUTTONED_PALETTE,
			_button_editor_relative},

		// Foreground
		{OD_GADGET,
			PALETTE_KIND,
			{0,1,10,SIZE_MAXIMUM},
			{6,2,16,-4},
			MSG_PALETTE_FOREGROUND,
			PLACETEXT_ABOVE|BUTTONFLAG_THIN_BORDERS,
			GAD_PALETTE_FOREGROUND,
			_button_editor_palette_layout},

		// Background
		{OD_GADGET,
			PALETTE_KIND,
			{10,1,10,SIZE_MAXIMUM},
			{28,2,16,-4},
			MSG_PALETTE_BACKGROUND,
			PLACETEXT_ABOVE|BUTTONFLAG_THIN_BORDERS,
			GAD_PALETTE_BACKGROUND,
			_button_editor_palette_layout},

		// Button area
		{OD_AREA,
			TEXTPEN,
			{20,6,SIZE_MAXIMUM,SIZE_MAXIMUM},
			{64,22,-6,-6},
			MSG_BUTTONED_BUTTON,
			AREAFLAG_TITLE|AREAFLAG_ERASE,
			GAD_BUTTONED_BUTTON,
			_button_editor_relative},

		// Use
		{OD_GADGET,
			BUTTON_KIND,
			{0,POS_RIGHT_JUSTIFY,14,1},
			{3,-3,8,6},
			MSG_USE,
			BUTTONFLAG_OKAY_BUTTON|BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_USE,
			0},

		// Cancel
		{OD_GADGET,
			BUTTON_KIND,
			{POS_RIGHT_JUSTIFY,POS_RIGHT_JUSTIFY,14,1},
			{-3,-3,8,6},
			MSG_CANCEL,
			BUTTONFLAG_CANCEL_BUTTON|BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_CANCEL,
			0},

		{OD_END}},

	_button_editor_image_objects[]={

		// Image requester
		{OD_GADGET,
			FILE_BUTTON_KIND,
			{24,1,0,1},
			{32,8,28,6},
			MSG_BUTTONED_IMAGE,
			BUTTONFLAG_THIN_BORDERS,
			GAD_BUTTONED_IMAGE_REQ,
			_button_editor_image},

		// Image path
		{OD_GADGET,
			STRING_KIND,
			{24,1,SIZE_MAXIMUM,1},
			{60,8,-4,6},
			0,
			0,
			GAD_BUTTONED_LABEL,
			_button_editor_name},

		{OD_END}},

	_button_editor_label_objects[]={

		// Label
		{OD_GADGET,
			STRING_KIND,
			{24,1,SIZE_MAXIMUM,1},
			{32,8,-4,6},
			MSG_BUTTONED_LABEL,
			PLACETEXT_LEFT,
			GAD_BUTTONED_LABEL,
			_button_editor_label},

		{OD_END}};
