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

#ifndef GALILEOFM_CONFIG_LISTER_H
#define GALILEOFM_CONFIG_LISTER_H

#define DIRFLAGS_DISABLE_CACHING    (1<<0)	// Disable caching
#define DIRFLAGS_REREAD_CHANGED	    (1<<1)	// Re-read changed buffers
#define DIRFLAGS_EXPANDPATHS	    (1<<7)	// Expand pathnames

// lister options
#define LISTEROPTF_DEVICES	    (1<<0)	// Device list in new lister
#define LISTEROPTF_2XCLICK	    (1<<1)	// Double-click editing
#define LISTEROPTF_SIMPLE	    (1<<2)	// Simple-refresh windows
#define LISTEROPTF_TITLES	    (1<<3)	// Want field titles
#define LISTEROPTF_SNAPSHOT	    (1<<4)	// Always use snapshot position
#define LISTEROPTF_POPUP	    (1<<5)	// PopUp in name mode
#define LISTEROPTF_EDIT_LEFT	    (1<<6)	// Editing with left button
#define LISTEROPTF_EDIT_MID	    (1<<7)	// Editing with middle button
#define LISTEROPTF_EDIT_BOTH	    (LISTEROPTF_EDIT_LEFT|LISTEROPTF_EDIT_MID)
#define LISTEROPTF_FUEL_GAUGE	    (1<<8)	// Free space gauge
#define LISTEROPTF_SUBDROP	    (1<<9)	// Drop into sub-directories
#define LISTEROPTF_NOACTIVESELECT   (1<<10)	// No file select on window activation
#define LISTEROPTF_VOSTY_ZOOM	    (1<<11)	// Ben Vost zoom mode

#endif
