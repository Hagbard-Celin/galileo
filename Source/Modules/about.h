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

#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <graphics/gfxmacros.h>
#include <gfm/image_protos.h>
#include <gfm/imageremap_protos.h>
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "modules_lib.h"

typedef struct
{
	short	width;
	short	height;
	short	depth;
	short	planesize;
	UWORD	*imagedata;
	ULONG	*palette;
} image_data;

#define image_count	17
#define image_special	14

extern ConfigWindow	about_window;
extern ObjectDef	about_objects[];
extern __far UWORD 	  logo_image[logo_frames][472];

extern struct Library *DataTypesBase;
extern APTR logo_obj[logo_frames];

extern ImageRemap remap;

typedef struct
{
	struct Window	*window;

	GL_Object	*text_area;
	unsigned short	text_off;
	short		text_pos;
	short		text_disp;
	struct IBox	text_dims;
	struct RastPort	text_rp;
	struct Region	*text_reg;

	char		*text_line;
	short		text_len;
	short		text_size;

	char		*trans_info;
	char		*message;
} about_data;

extern unsigned char about_message[];
