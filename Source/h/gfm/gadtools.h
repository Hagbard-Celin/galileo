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

#ifndef _GALILEOFM_GADTOOLS_H
#define _GALILEOFM_GADTOOLS_H

#define SIZE_MAXIMUM		-1
#define SIZE_MAX_LESS		-101

#define OD_END		0		// End of a list
#define OD_GADGET	1		// A gadget
#define OD_TEXT		2		// Some text
#define OD_AREA		3		// A rectangular area
#define OD_IMAGE	4		// An image
#define OD_SKIP		-1		// Skip this entry

#define JUSTIFY_LEFT		0
#define JUSTIFY_RIGHT		1
#define JUSTIFY_CENTER		2

#define TEXTFLAG_RIGHT_JUSTIFY		(1<<1)		// Right-justify text
#define TEXTFLAG_CENTER			(1<<2)		// Center text
#define TEXTFLAG_TEXT_STRING		(1<<17)		// Text is a string, not a Locale ID
#define TEXTFLAG_NO_USCORE		(1<<18)		// No underscore in text
#define TEXTFLAG_ADJUST_TEXT		(1<<23)		// Adjust for text

#define BUTTONFLAG_IMMEDIATE		(1<<8)
#define BUTTONFLAG_OKAY_BUTTON		(1<<18)		// Button is an "ok" button
#define BUTTONFLAG_CANCEL_BUTTON	(1<<19)		// Button is a "cancel" button
#define BUTTONFLAG_TOGGLE_SELECT	(1<<20)		// Button is toggle-select
#define LISTVIEWFLAG_CURSOR_KEYS	(1<<21)		// Lister responds to cursor
#define BUTTONFLAG_THIN_BORDERS		(1<<22)		// Button has thin borders
#define FILEBUTFLAG_SAVE		(1<<21)		// Save mode

#define POSFLAG_ADJUST_POS_X		(1<<24)		// Position adjustor
#define POSFLAG_ADJUST_POS_Y		(1<<25)		// Position adjustor
#define POSFLAG_ALIGN_POS_X		(1<<26)		// Align
#define POSFLAG_ALIGN_POS_Y		(1<<27)		// Align
#define POSFLAG_CENTER_POS_X	 	(1<<28)	        // Center
#define POSFLAG_CENTER_POS_Y	 	(1<<29)	        // Center

// Custom tags
#define GTCustom_LocaleLabels	TAG_USER + 0	// Points to list of Locale IDs
#define GTCustom_Image		TAG_USER + 1	// Image for gadget
#define GTCustom_CallBack	TAG_USER + 2	// Tag ID and data filled in by callback
#define GTCustom_LayoutRel	TAG_USER + 3	// Layout relative to this object ID
#define GTCustom_Control	TAG_USER + 4	// Controls another gadget
#define GTCustom_TextAttr	TAG_USER + 6	// TextAttr to use
#define GTCustom_MinMax		TAG_USER + 24	// Minimum and maximum bounds
#define GTCustom_BorderButton	TAG_USER + 26	// Border button
#define GTCustom_ThinBorders	TAG_USER + 27	// Gadget has thin borders
#define GTCustom_TextInside	TAG_USER + 28	// Gadget has text inside it
#define GTCustom_LocaleKey	TAG_USER + 29	// Key from locale string
#define GTCustom_Flags		TAG_USER + 30	// Flags
#define GTCustom_NoSelectNext	TAG_USER + 31	// Don't select next field
#define GTCustom_PathFilter	TAG_USER + 32	// Filter path characters
#define GTCustom_History	TAG_USER + 33	// History
#define GTCustom_CopyTags	TAG_USER + 34	// Copy tags
#define GTCustom_FontPens	TAG_USER + 35	// Place to store pens and style
#define GTCustom_FontPenCount	TAG_USER + 36	// Number of pens for font requester
#define GTCustom_FontPenTable	TAG_USER + 37	// Table of pens for font requester
#define GTCustom_Bold		TAG_USER + 38	// Bold pen
#define GTCustom_Secure		TAG_USER + 39	// Secure string field
#define GTCustom_Integer	TAG_USER + 40	// Integer gadget
#define GTCustom_TextPlacement	TAG_USER + 41	// Position of text
#define GTCustom_NoGhost	TAG_USER + 42	// Disable without ghosting
#define GTCustom_LayoutFlags	TAG_USER + 43	// Layout flags
#define GTCustom_Style		TAG_USER + 44	// Pen styles
#define GTCustom_FrameFlags	TAG_USER + 45	// Frame flags
#define GTCustom_ChangeSigTask	TAG_USER + 46	// Task to signal on change
#define GTCustom_ChangeSigBit	TAG_USER + 47	// Signal bit to use
#define GTCustom_LayoutPos	TAG_USER + 49	// Use with the POSFLAGs
#define GTCustom_Borderless	TAG_USER + 50	// Borderless
#define GTCustom_Justify	TAG_USER + 51	// Justification
#define GTCustom_UpperCase	TAG_USER + 53	// Uppercase
#define GTCustom_Recessed	TAG_USER + 54	// Recessed border
#define GTCustom_FixedWidthOnly	TAG_USER + 55	// Only show fixed-width fonts
#define GTCustom_CompoundGadget TAG_USER + 60

#define POS_CENTER		-1
#define POS_RIGHT_JUSTIFY	-2
#define POS_MOUSE_CENTER	-3
#define POS_MOUSE_REL		-4
#define POS_PROPORTION		1024
#define POS_SQUARE		1124
#define POS_REL_RIGHT		(1<<14)

// Listview tags
#define GLV_ScrollUp		TAG_USER + 7		// Scroll list up
#define GLV_ScrollDown		TAG_USER + 8		// Scroll list down
#define GLV_SelectPrevious	TAG_USER + 11		// Select previous item
#define GLV_SelectNext		TAG_USER + 12		// Select next item
#define GLV_Labels		GTLV_Labels		// Labels
#define GLV_Top			GTLV_Top		// Top item
#define GLV_MakeVisible		GTLV_MakeVisible	// Make visible
#define GLV_Selected		GTLV_Selected		// Selected
#define GLV_ScrollWidth		GTLV_ScrollWidth	// Scroller width
#define GLV_ShowSelected	GTLV_ShowSelected	// Show selected
#define GLV_Check		TAG_USER + 10		// Check selection
#define GLV_Highlight		TAG_USER + 14		// Highlight selection
#define GLV_MultiSelect		TAG_USER + 9		// Multi-selection
#define GLV_ReadOnly		GTLV_ReadOnly		// Read only
#define GLV_Lines		TAG_USER + 13		// Visible lines (get only)
#define GLV_ShowChecks		TAG_USER + 15		// Show checkmarks
#define GLV_Flags		TAG_USER + 16		// Layout flags
#define GLV_NoScroller		TAG_USER + 17		// No scroller necessary
#define GLV_TopJustify		TAG_USER + 18		// Top-justify items
#define GLV_RightJustify	TAG_USER + 19		// Right-justify items
#define GLV_DragNotify		TAG_USER + 20		// Notify of drags
#define GLV_GetLine		TAG_USER + 25		// Get line from coordinate
#define GLV_DrawLine		TAG_USER + 26		// Draw a line from the listview
#define GLV_Object		TAG_USER + 27		// Pointer to object
#define GLV_DoubleClick		TAG_USER + 28		// Indicates double-click
#define GLV_ShowFilenames	TAG_USER + 48		// Show filenames only
#define GLV_ShowSeparators	TAG_USER + 52		// Show --- as separator
#define GLV_PageUp		TAG_USER + 53
#define GLV_PageDown		TAG_USER + 54
#define GLV_Home		TAG_USER + 55
#define GLV_End			TAG_USER + 56
#define GLV_ScrollLeft		TAG_USER + 57
#define GLV_ThinBorder		TAG_USER + 58		// Thin borders
#define GLV_NoBorder		TAG_USER + 59
#define GLV_CompoundGadget	TAG_USER + 60
#define GLV_CompoundObject  	TAG_USER + 61
#define GLV_CompoundCx		TAG_USER + 62

#define AREAFLAG_RAISED			(1<<8)		// Raised rectangle
#define AREAFLAG_RECESSED		(1<<9)		// Recessed rectangle
#define AREAFLAG_THIN			(1<<10)		// Thin borders
#define AREAFLAG_ICON			(1<<11)		// Icon drop box
#define AREAFLAG_ERASE			(1<<12)		// Erase interior
#define AREAFLAG_LINE			(1<<13)		// Line (separator)
#define AREAFLAG_GRAPH			(1<<14)		// Graph display
#define AREAFLAG_OPTIM			(1<<15)		// Optimised refreshing
#define AREAFLAG_TITLE			(1<<16)		// Box with a title
#define AREAFLAG_NOFILL			(1<<18)		// No fill
#define AREAFLAG_FILL_COLOUR		(1<<19)		// Always fill with colour
#define AREAFLAG_GROOVE         	(1<<20)     	// Recessed line (separator)

#define MY_LISTVIEW_KIND	127		// Custom listview gadget
#define FILE_BUTTON_KIND	126		// File button gadget
#define DIR_BUTTON_KIND		125		// Directory button gadget
#define FONT_BUTTON_KIND	124		// Font button gadget
#define FIELD_KIND		123		// Text field (no editing)
#define FRAME_KIND		122		// Frame
#define HOTKEY_KIND		121		// Hotkey field
#define POPUP_BUTTON_KIND	120		// File button gadget (no popup)

typedef struct _MenuData {
    UBYTE   type;			    // Menu type
    ULONG   id;				    // Menu ID
    ULONG   name;			    // Menu name
    ULONG   flags;			    // Menu flags
} MenuData;

#define MENUFLAG_TEXT_STRING	(1<<16)	    // Menu name is a real string
#define MENUFLAG_COMM_SEQ	(1<<17)	    // Give menu a command sequence
#define MENUFLAG_AUTO_MUTEX	(1<<18)	    // Automatic mutual exclusion
#define MENUFLAG_USE_SEQ	(1<<19)	    // Use command sequence supplied

#define MENUFLAG_MAKE_SEQ(c)	((ULONG)(c)<<24)
#define MENUFLAG_GET_SEQ(fl)	((char)((fl)>>24))

#define NM_NEXT			10
#define NM_BAR_LABEL	(ULONG)NM_BARLABEL

#endif

