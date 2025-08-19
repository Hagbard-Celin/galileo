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

#ifndef _GALILEOFM_LISTER_IPC_H
#define _GALILEOFM_LISTER_IPC_H

enum
{
	LISTER_OPEN,			// Open lister display					0
	LISTER_CLOSE,			// Close lister display
	LISTER_MAKE_SOURCE,		// Make this lister the source
	LISTER_ACTIVATE,		// Change activation status
	LISTER_BUSY,			// Set or clear busy state
	LISTER_MAKE_DEST,		// Make this lister the destination			5
	LISTER_OFF,			// Turn this lister off

	LISTER_CHECK_SPECIAL_BUFFER,	// Check if currently showing a special buffer
	LISTER_SHOW_SPECIAL_BUFFER,	// Show special buffer
	LISTER_BUFFER_FIND,		// Searches for a named buffer

	LISTER_BUFFER_FIND_EMPTY,	// Finds an empty buffer				10
	LISTER_BUFFER_LIST,		// List available buffers
	LISTER_GET_PATH,		// Get current path
	LISTER_REFRESH_PATH,		// Refresh path field
	LISTER_REFRESH_WINDOW,		// Refresh lister
	LISTER_REFRESH_TITLE,		// Refresh lister title					15
	LISTER_REFRESH_SLIDERS,		// Refresh lister sliders
	LISTER_ADD_FILE,		// Add file to list
	LISTER_REMOVE_FILE,		// Remove file from list
	LISTER_RELOAD_FILE,		// Reload a file in the list

	LISTER_MAKE_RESELECT,		// Make reselection list				20
	LISTER_DO_RESELECT,		// Reselect from a list
	LISTER_DO_PARENT_ROOT,		// Do parent/root read
	LISTER_FIND_FIRST_SEL,		// Scroll to first selected entry
	LISTER_FIND_ENTRY,		// Scroll to show an entry
	LISTER_STORE_POS,		// Store lister position				25
	LISTER_RESTORE_POS,		// Restore old position in lister
	LISTER_SHOW_INFO,		// Show selection information
	LISTER_REMOVE_SIZES,		// Remove directory sizes
	LISTER_SET_LOCKER,		// Set locker port

	LISTER_INIT,			// Initialise and (maybe) open lister			30
	LISTER_SELECT_GLOBAL_STATE,	// Select global state
	LISTER_SELECT_GLOBAL_TOGGLE,	// Toggle global state
	LISTER_STATUS,			// Lister status text
	LISTER_PROGRESS_ON,		// Progress indicator on
	LISTER_PROGRESS_OFF,		// Progress indicator off				35
	LISTER_PROGRESS_UPDATE,		// Progress indicator update
	LISTER_UNLOCK,			// Unlock this lister
	LISTER_SELECT_WILD,		// Do wildcard selection
	LISTER_DEVICE_LIST,		// Display device list in this lister

	LISTER_PARENT,			// Read parent						40
	LISTER_ROOT,			// Read root
	LISTER_UPDATE_STAMP,		// Update buffer stamp
	LISTER_COPY_BUFFER,		// Copy a buffer from somewhere
	LISTER_RESCAN,			// Rescan contents
	LISTER_RESCAN_CHECK,		// Rescan contents if date changed			45
	LISTER_SPLIT,			// Split display
	LISTER_WAIT,			// Wait for lister to finish
	LISTER_FILETYPE_SNIFF,		// Sniffed out a filetype
	LISTER_REFRESH_NAME,		// Refresh lister name
	LISTER_GET_ICONS,		// Read icons						50

	LISTER_REMOVE_ENTRY,		// Remove an entry
	LISTER_GET_HANDLE,		// Get lister's handle
	LISTER_REFRESH_FREE,		// Refresh free space

	LISTER_BACKFILL_CHANGE,		// Backfill pattern has changed

	LISTER_ICONIFY,			// Iconify lister					55
	LISTER_UNICONIFY,		// Uniconify lister

	LISTER_MODE,			// Change mode
	LISTER_TOOLBAR,			// New toolbar

	LISTER_WAIT_BUSY,		// Wait for lister to go busy

	LISTER_SHOW_BUFFER,		// Show a buffer in a lister                            60

	LISTER_FILE_PROGRESS_TOTAL,	// Set file progress total
	LISTER_FILE_PROGRESS_UPDATE,	// Set file progress update
	LISTER_GET_ICON,		// Get specific icon

	LISTER_DO_FUNCTION,		// Do a function
	LISTER_CHECK_REFRESH,		// Check for refresh

	LISTER_UPDATE_DEVICE,		// Change a device name

	LISTER_HIGHLIGHT,		// Highlight an entry (perhaps)
	LISTER_SHOW_ICON,		// Show a specific icon
	LISTER_FILE_PROGRESS_INFO,	// Set progress info string
	LISTER_PROGRESS_TOTAL,		// Set total progress
	LISTER_FILE_PROGRESS_SET,	// Set total file stuff
	LISTER_PROGRESS_TITLE,		// Set progress title
	LISTER_PROGRESS_COUNT,		// Update bar count

	LISTER_SET_SNIFF,		// Set sniff flag
	LISTER_CONFIGURE,		// Change display format
	LISTER_SELSHOW,			// Select/show entry
	LISTER_FIND_CACHED_BUFFER,	// Find a buffer in the cache
	LISTER_SET_GAUGE,		// Set gauge state
	LISTER_BUFFER_FIND_LOCK,	// Searches for a buffer by lock
	LISTER_BUFFER_FIND_EMPTY_LOCK,	// Finds an empty buffer
	LISTER_SET_PATH,
};

#endif
