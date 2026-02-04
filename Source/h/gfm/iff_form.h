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

#ifndef GALILEOFM_IFF_FORM_H
#define GALILEOFM_IFF_FORM_H


/****************************************************************************
	      IFF Configuration Storage
 ****************************************************************************/

#define ID_GILO	    MAKE_ID('G','I','L','O')	// Galileo FORM
#define ID_BTBK	    MAKE_ID('B','T','B','K')	// Button bank to open
#define ID_BANK	    MAKE_ID('B','A','N','K')	// Button bank to open
#define ID_BTNW	    MAKE_ID('B','T','N','W')	// Button window
#define ID_FUNC	    MAKE_ID('F','U','N','C')	// Function definition
#define ID_BUTN	    MAKE_ID('B','U','T','N')	// Button definition
#define ID_TYPE	    MAKE_ID('T','Y','P','E')	// Filetype definition
#define ID_HKEY	    MAKE_ID('H','K','E','Y')	// Hotkey definition
#define ID_SETS	    MAKE_ID('S','E','T','S')	// Settings
#define ID_LSTR	    MAKE_ID('L','S','T','R')	// Lister definition
#define ID_GLST	    MAKE_ID('G','L','S','T')	// Lister definition
#define ID_LBUT	    MAKE_ID('L','B','U','T')	// Lister buttons
#define ID_ENVR	    MAKE_ID('E','N','V','R')	// Environment definition
#define ID_TBAR	    MAKE_ID('T','B','A','R')	// Toolbar pathname
#define ID_LMEN	    MAKE_ID('L','M','E','N')	// Lister menu pathname
#define ID_UMEN	    MAKE_ID('U','M','E','N')	// User menu pathname
#define ID_SCRP	    MAKE_ID('S','C','R','P')	// Scripts pathname
#define ID_DESK	    MAKE_ID('D','E','S','K')	// Desktop
#define ID_IPOS	    MAKE_ID('I','P','O','S')	// Icon positioning
#define ID_STRT	    MAKE_ID('S','T','R','T')	// Start Menu
#define ID_PATH	    MAKE_ID('P','A','T','H')	// Path list
#define ID_BPIC	    MAKE_ID('B','P','I','C')	// Background picture
#define ID_SNDX	    MAKE_ID('S','N','D','X')	// Sound entry

#endif
