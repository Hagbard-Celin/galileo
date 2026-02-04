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

#ifndef _GALILEOFM_CFG_FLAGS_H
#define _GALILEOFM_CFG_FLAGS_H

#define ENVRF_AUTO_TILE	    (1<<0)	// Do auto-tile on startup

#define CFGSEF_CHANGED	    (1<<15)

/****************************************************************************
	         Configuration flags
 ****************************************************************************/

// CFG_SETS	date_flags
#define		DATE_SUBST	(1<<0)
#define		DATE_12HOUR	(1<<1)
#define		DATE_1000SEP	(1<<2)

// CFG_SETS	icon_flags
#define		ICONFLAG_DOUNTOICONS	(1<<1)
#define		ICONFLAG_AUTOSELECT	(1<<2)
#define		ICONFLAG_TRAP_MORE	(1<<3)

// CFG_SETS	replace_method
enum {
	    REPLACE_ALWAYS,
	    REPLACE_NEVER,
	    REPLACE_OLDER,
	    REPLACE_ASK,
};

// CFG_SETS	replace_flags
#define		REPLACEF_CHECK_VERSION	    (1<<0)
#define		REPLACEF_VERBOSE_REPLACE    (1<<1)

// CFG_SETS	update_flags
#define		UPDATEF_SCROLL	    (1<<1)
#define		UPDATEF_NOTIFY	    (1<<3)

// CFG_SETS	hide_method
enum
{
	    HIDE_CLOCK,
	    HIDE_NONE,
	    HIDE_ICON,
	    HIDE_MENU
};

// CFG_SETS	general_flags
#define		GENERALF_MOUSE_HOOK		(1<<20)
#define		GENERALF_VERBOSE_REPLACE	(1<<21)
#define		GENERALF_QUICK_QUIT		(1<<22)
#define		GENERALF_EXTENDED_KEY		(1<<23)
#define		GENERALF_FILETYPE_SNIFFER	(1<<24)
#define		GENERALF_VALID_POPKEY		(1<<25)
#define		GENERALF_ENABLE_MUFS		(1<<26)
#define		GENERALF_EXCLUSIVE_STARTUP_SND	(1<<27)

#endif
