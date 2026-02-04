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

#ifndef _GALILEO_LISTVIEW_H
#define _GALILEO_LISTVIEW_H

#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif


// ListView noborder flags
#define GLVNBF_LEFT	(1<<0)
#define GLVNBF_TOP	(1<<1)
#define GLVNBF_RIGHT	(1<<2)
#define GLVNBF_BOTTOM	(1<<3)
#define GLVNBF_ALL	(GLVNB_LEFT|GLVNB_TOP|GLVNB_RIGHT|GLVNB_BOTTOM)

// Listview node data
#define lve_Flags		ln_Type			// Listview entry flags
#define lve_Pen			ln_Pri			// Listview entry pen
#define LVEF_SELECTED		(1<<0)			// Entry is selected
#define LVEF_USE_PEN		(1<<1)			// Use pen to render entry
#define LVEF_TEMP		(1<<2)			// Temporary flag for something

typedef struct
{
    struct RastPort	    *rp;
    struct DrawInfo	    *drawinfo;
    struct Node		    *node;
    unsigned short	    line;
    struct IBox		    box;
} ListViewDraw;

#endif
