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

#ifndef _GALILEO_FUNCTION_FLAGS_H
#define _GALILEO_FUNCTION_FLAGS_H


#define FUNCF_OUTPUT_WINDOW	(1<<0)	// Output window
#define FUNCF_OUTPUT_FILE	(1<<1)	// Output to file
#define FUNCF_WORKBENCH_OUTPUT	(1<<2)	// Open window on Workbench
#define FUNCF_RUN_ASYNC		(1<<4)	// Run asynchronously
#define FUNCF_CD_SOURCE		(1<<5)	// CD source
#define FUNCF_CD_DESTINATION	(1<<6)	// CD destination
#define FUNCF_DO_ALL_FILES	(1<<7)	// Repeat for all files
#define FUNCF_RECURSE_DIRS	(1<<8)	// Recursive directories
#define FUNCF_RELOAD_FILES	(1<<9)	// Reload each file when done
#define FUNCF_NO_QUOTES		(1<<11)	// Don't quote filenames
#define FUNCF_RESCAN_SOURCE	(1<<12)	// Rescan source directory
#define FUNCF_RESCAN_DEST	(1<<13)	// Rescan destination directory
#define FUNCF_WAIT_CLOSE	(1<<16)	// Wait for click before closing window

#define FUNCF_ORIGINAL_FLAGS	(FUNCF_OUTPUT_WINDOW|\
				 FUNCF_OUTPUT_FILE|\
				 FUNCF_WORKBENCH_OUTPUT|\
				 FUNCF_RUN_ASYNC|\
				 FUNCF_CD_SOURCE|\
				 FUNCF_CD_DESTINATION|\
				 FUNCF_DO_ALL_FILES|\
				 FUNCF_RECURSE_DIRS|\
				 FUNCF_RELOAD_FILES|\
				 FUNCF_NO_QUOTES|\
				 FUNCF_RESCAN_SOURCE|\
				 FUNCF_RESCAN_DEST|\
				 FUNCF_WAIT_CLOSE)

#define FUNCF2_HOTKEY_FUNC	(1<<0)	// Function is a hotkey function
#define FUNCF2_FILETYPE_FUNC	(1<<1)	// Function is a filetype function
#define FUNCF2_VALID_IX		(1<<2)	// Valid IX information (qual mask/same)
#define FUNCF2_LABEL_FUNC	(1<<3)	// Function has a label associated with it


// These flags are used internally by Galileo and not stored
#define FUNCF2_ORIGINAL		(1<<30)	// Call original function
#define FUNCF2_FREE_FUNCTION	(1<<31)	// Free after use

#endif
