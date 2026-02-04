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

#ifndef _GALILEOFM_GUI_ELEMENT_H
#define _GALILEOFM_GUI_ELEMENT_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#define FILENAMELEN	256				// Unix allows greater than Amiga's 30


/********************************/

//
//	Some structures copied from the Galileo includes...
//


// Window dimensions
typedef struct
{
    struct IBox	    wd_Normal;
    struct IBox	    wd_Zoomed;
    unsigned short  wd_Flags;
} WindowDimensions;

// Defines information about a GUI element
typedef struct
{
	BOOL		 visible;			// Indicates visibility

	struct IBox	 box;				// Dimensions in an IBox
	struct Rectangle rect;				// Dimensions in a Rectangle

	UWORD		 fg_pen;			// Foreground pen to use
	UWORD		 bg_pen;			// Background pen to use
	struct TextFont	 *font;				// Font to use

	struct RastPort	 rast;				// RastPort copy
} GUI_Element;

#endif
