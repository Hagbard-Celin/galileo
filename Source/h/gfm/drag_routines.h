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

#ifndef _GALILEOFM_DRAG_ROUTINES_H
#define _GALILEOFM_DRAG_ROUTINES_H

#ifndef	GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#ifndef	GRAPHICS_GELS_H
#include <graphics/gels.h>
#endif

typedef struct _DragInfo
{
    struct RastPort	*rastport;	// Stores RastPort this bob belongs to
    struct ViewPort	*viewport;	// Stores ViewPort

    WORD		width;		// Bob width
    WORD		height;		// Bob height

    struct VSprite	sprite;		// VSprite structure
    struct Bob		bob;		// BOB structure

    unsigned long	flags;		// Flags

    struct RastPort	drag_rp;	// RastPort we can draw into
    struct BitMap	drag_bm;	// BitMap we can draw into

    struct Window	*window;	// Window pointer

    long		offset_x;
    long		offset_y;
    long		userdata;
    long		pad[4];

    struct DBufPacket	*dbuf;		// Double-buffering packet

    long		dest_width;
    long		dest_height;
} DragInfo;

typedef struct
{
    struct VSprite  head;   // GEL list head sprite
    struct VSprite  tail;   // GEL list tail sprite
    struct GelsInfo info;   // GEL info
} DragInfoExtra;

#define DRAGF_VALID		(1<<0)	// Bob is valid
#define DRAGF_OPAQUE		(1<<1)	// Bob should be opaque
#define DRAGF_DONE_GELS		(1<<2)	// Installed GelsInfo
#define DRAGF_NO_LOCK		(1<<3)	// Don't lock layers
#define DRAGF_TRANSPARENT	(1<<4)	// Bob should be transparent (use with opaque)
#define DRAGF_DBUF		(1<<5)	// Double-buffering
#define DRAGF_NOSORT		(1<<6)	// Don't resort list on draw
#define DRAGF_NOWAIT		(1<<7)	// Don't WaitTOF on draw
#define DRAGF_FREE		(1<<8)
#define DRAGF_CUSTOM		(1<<9)	// Custom rendering
#define DRAGF_NO_MASK		(1<<10)	// No masking

#define DRAGF_NEED_GELS		(1<<0)	// Need GELs initialised
#define DRAGF_REMOVE		(1<<0)	// Remove only

#endif
