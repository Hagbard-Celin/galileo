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

#ifndef GALILEOFM_CONFIG_DISPLAY_H
#define GALILEOFM_CONFIG_DISPLAY_H

// env_flags
#define ENVF_CLOCK	    (1<<0)	// Clock on
#define ENVF_BACKDROP	    (1<<1)	// Backdrop window
#define ENVF_CHANGED	    (1<<2)	// Environment has changed
#define ENVF_DESKTOP_FOLDER (1<<3)	// Desktop folder popup
#define ENVF_USE_PATHLIST   (1<<4)	// Use path list

// display_options
#define DISPOPTF_NO_BACKDROP	    (1<<0)	// No display backdrop
#define DISPOPTF_NO_ALLOCPEN	    (1<<1)	// No alloc pens
#define DISPOPTF_SHOW_APPICONS	    (1<<2)	// Show AppIcons
#define DISPOPTF_SHOW_TOOLS	    (1<<3)	// Show Tools menu
#define DISPOPTF_HIDE_BAD	    (1<<4)	// Hide bad disks
#define DISPOPTF_DEFPUBSCR	    (1<<5)	// Default public screen
#define DISPOPTF_ICON_POS	    (1<<6)	// Use Workbench icon positions
#define DISPOPTF_SHIFT_APPICONS	    (1<<7)	// Shift AppIcons to Tools menu
#define DISPOPTF_USE_WBPATTERN	    (1<<8)	// Use WBPattern prefs
#define DISPOPTF_REMGALILEOPOS	    (1<<9)	// Remove Galileo icon positions
#define DISPOPTF_REALTIME_SCROLL    (1<<10)	// Real-time icon scrolling
#define DISPOPTF_THIN_BORDERS	    (1<<11)	// Thin borders

// env_BackgroundFlags
#define ENVBF_CENTER_PIC	(1<<0)
#define ENVBF_PRECISION_NONE	(1<<1)
#define ENVBF_PRECISION_GUI	(1<<2)
#define ENVBF_PRECISION_ICON	(1<<3)
#define ENVBF_PRECISION_IMAGE	(1<<4)
#define ENVBF_PRECISION_EXACT	(1<<5)
#define ENVBF_STRETCH_PIC	(1<<6)
#define ENVBF_USE_COLOUR	(1<<7)

#define ENVBF_COL_R_GET(x)  (((x)>>24)&0xff)
#define ENVBF_COL_R_SET(x)  ((x)<<24)
#define ENVBF_COL_G_GET(x)  (((x)>>16)&0xff)
#define ENVBF_COL_G_SET(x)  ((x)<<16)
#define ENVBF_COL_B_GET(x)  (((x)>>8)&0xff)
#define ENVBF_COL_B_SET(x)  ((x)<<8)
#define ENVBF_COL_MASK	    0xffffff00

// desktop flags
#define DESKTOPF_DISTINCT	(1<<0)	    // Distinct icon positions
#define DESKTOPF_NO_CACHE	(1<<1)	    // Don't cache icons
#define DESKTOPF_NO_REMAP	(1<<2)	    // Don't remap icons
#define DESKTOPF_NO_BORDERS	(1<<3)	    // No icon borders
#define DESKTOPF_NO_ARROW	(1<<4)	    // Disable left-out arrow
#define DESKTOPF_SPLIT_LABELS	(1<<5)	    // Split long icon labels
#define DESKTOPF_TRANSPARENCY	(1<<6)	    // Full icon transparency
#define DESKTOPF_NO_NOLABELS	(1<<7)	    // No labelless icons
#define DESKTOPF_SMART_ICONCOPY	(1<<8)	    // Smart icon copying
#define DESKTOPF_FORCE_SPLIT	(1<<9)	    // Force split of labels
#define DESKTOPF_QUICK_DRAG	(1<<10)	    // Quick icon dragging
#define DESKTOPF_NO_CUSTOMDRAG	(1<<11)	    // No custom dragging

enum
{
    DESKTOP_ICON,
    DESKTOP_HIDE,
    DESKTOP_LEAVEOUT,
    DESKTOP_ICONPOS,
    DESKTOP_HIDE_BAD,
};

#endif
