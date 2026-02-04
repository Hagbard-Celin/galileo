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

#ifndef GALILEOFM_BUTTON_FLAGS_H
#define GALILEOFM_BUTTON_FLAGS_H

#define BTNWF_ICONIFIED		(1<<0)	// Window iconified initially
#define BTNWF_GFX		(1<<1)	// Buttons are graphical in nature
#define BTNWF_LISTER_BUTTONS	(1<<2)	// Lister buttons
#define BTNWF_NO_BORDER		(1<<3)	// No border
#define BTNWF_BORDERLESS	(1<<4)	// Borderless buttons
#define BTNWF_SMART_REFRESH	(1<<5)	// Smart refresh window
#define BTNWF_HORIZ		(1<<6)	// Horizontal
#define BTNWF_VERT		(1<<7)	// Vertical
#define BTNWF_TOOLBAR		(1<<8)	// Toolbar bank
#define BTNWF_NO_DOGEARS	(1<<9)	// No dog-ears
#define BTNWF_CHANGED		(1<<10)	// Bank has changed
#define BTNWF_DRAG_RIGHT	(1<<11)	// Drag bar on right side
#define BTNWF_NO_SCALE_IMAGES	(1<<12)	// No image scaling
#define BTNWF_NONE		(1<<13)	// No drag bar
#define BTNWF_AUTO_CLOSE	(1<<14)	// Auto close bank
#define BTNWF_AUTO_ICONIFY	(1<<15)	// Auto iconify bank
#define BTNWF_RIGHT_BELOW	(1<<16)	// Border right or below
#define BTNWF_FIX_MENU		(1<<17)	// Menus have been fixed
#define BTNWF_ACTIVE_POPUP	(1<<18)	// Active popups

#define BUTNF_RIGHT_FUNC    (1<<0)	// Button has a right button function
#define BUTNF_MID_FUNC	    (1<<1)	// Button has a middle button function
#define BUTNF_GRAPHIC	    (1<<2)	// Button is a graphic button
#define BUTNF_NO_BORDER	    (1<<3)	// No border for button
#define BUTNF_NEW_FORMAT    (1<<4)	// New format button
#define BUTNF_GLOBAL	    (1<<5)	// System-global
#define BUTNF_TITLE	    (1<<6)	// Menu title
#define BUTNF_ITEM	    (1<<7)	// Menu item
#define BUTNF_SUB	    (1<<8)	// Sub item

#define BUTNF_TEMP	    (1<<31)	// Temporary flag


#endif
