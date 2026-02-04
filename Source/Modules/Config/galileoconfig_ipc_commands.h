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

#ifndef _GALILEOCONFIG_IPC_COMMANDS_H
#define _GALILEOCONFIG_IPC_COMMANDS_H


// Messages for the button editor
enum
{
	BUTTONEDIT_BASE=0x8000,

	BUTTONEDIT_HELLO,		// Editor says hello
	BUTTONEDIT_GOODBYE,		// Editor says goodbye
	BUTTONEDIT_QUIT,		// Editor is told to quit
	BUTTONEDIT_NEW_BANK,		// New bank pointer
	BUTTONEDIT_REFRESH,		// Refresh bank
	BUTTONEDIT_GET_SELECTION,	// Get current row/column selection
	BUTTONEDIT_SET_SELECTION,	// Set current row/column selection
	BUTTONEDIT_GET_BUTTON,		// Get a button
	BUTTONEDIT_FLASH,		// Control select flash
	BUTTONEDIT_REDRAW_BUTTON,	// Redraw a button
	BUTTONEDIT_PROCESS_KEY,		// Asked to process a key
	BUTTONEDIT_EDIT_BUTTON,		// Edit a button
	BUTTONEDIT_RETURN,		// Returning an edited button
	BUTTONEDIT_GET_SCREEN_DATA,	// Get screen data
	BUTTONEDIT_NEW_COLOURS,		// Colours changed
	BUTTONEDIT_EDIT_ME,		// Edit this bank now
	BUTTONEDIT_CLIP_BUTTON,		// Copy a button to the clipboard
	BUTTONEDIT_SELECT_BUTTON,	// Select a button
	BUTTONEDIT_GIMME_BANK,		// Get current bank pointer
	BUTTONEDIT_GET_BUTTON_POINT,	// Get a button from a point
	BUTTONEDIT_BUTTON_TO_BANK,	// Send a button to a bank
	BUTTONEDIT_PISS_OFF,		// Stop editing this bank
	BUTTONEDIT_REOPEN,		// Re-open window
	BUTTONEDIT_RE_EDIT_ME,		// Re-edit a bank
	BUTTONEDIT_CHANGE_BANK,		// Change bank pointer
	BUTTONEDIT_REDRAW,		// Redraw bank

	BUTTONEDIT_MENU_NEW,
	BUTTONEDIT_MENU_OPEN,
	BUTTONEDIT_MENU_SAVE,
	BUTTONEDIT_MENU_SAVEAS,
	BUTTONEDIT_MENU_CLOSE,
	BUTTONEDIT_MENU_DEFAULTS,
	BUTTONEDIT_MENU_LASTSAVED,
	BUTTONEDIT_MENU_RESTORE,

	FUNCTIONEDIT_BASE=0x9000,
	FUNCTIONEDIT_RETURN,		// Function editor returning a function
	FUNCTIONEDIT_COPY_LINE,		// Copy a function

	FILETYPEEDIT_BASE=0x9100,
	FILETYPEEDIT_RETURN,		// Returning an edited filetype
	CLASSEDIT_RETURN,		// Return from the class editor

	CFG_APPMESSAGE_PASS=0x9200,	// Passed app message
	CFG_NEW_ID,			// New identity
};

#define WINDOW_BUTTON_CONFIG	1000
#define WINDOW_FUNCTION_EDITOR	1001

#endif
