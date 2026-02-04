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

#ifndef _GALILEOFM_MODULE_FUNC_FLAGS_H
#define _GALILEOFM_MODULE_FUNC_FLAGS_H

// Function flags
#define FUNCF_NEED_SOURCE		(1<<0)	// Needs a source directory
#define FUNCF_NEED_DEST			(1<<1)	// Needs a destination directory
#define FUNCF_NEED_FILES		(1<<2)	// Needs some files to work with
#define FUNCF_NEED_DIRS			(1<<3)	// Needs some files to work with
#define FUNCF_NEED_ENTRIES	(FUNCF_NEED_FILES|FUNCF_NEED_DIRS)
#define FUNCF_CAN_DO_ICONS		(1<<6)	// Function can do icons
#define FUNCF_SINGLE_SOURCE		(1<<8)	// Only a single source needed
#define FUNCF_SINGLE_DEST		(1<<9)	// Only a single destination needed
#define FUNCF_NO_LOOP_SOURCE	(1<<10)	// Don't loop through sources
#define FUNCF_WANT_DEST			(1<<11)	// Want destinations, don't need them
#define FUNCF_WANT_SOURCE		(1<<12)	// Want source, don't need it
#define FUNCF_CREATE_SOURCE		(1<<13)	// Can create our own source

#define FUNCF_WANT_ENTRIES		(1<<19)	// Want entries
#define FUNCF_NO_ARGS			(1<<20)	// Don't pass arguments
#define FUNCF_NO_BUSY			(1<<21)	// Don't send lister busy
#define FUNCF_SYNC				(1<<24)	// Function wants to be synchronous

#define ENTRYF_RECURSE_DIRS		(1<<0)

#define FUNCID_STARTUP			0xffffffff

#endif
