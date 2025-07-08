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

#include "galileofmlib.h"
#include "layout_routines.h"

#define GAD_POPUP_ID	(UWORD)-7
#define GAD_CHECK_ID	(UWORD)-6
#define GAD_SIGNAL	(UWORD)-5
#define GAD_STRING2_ID	(UWORD)-4
#define GAD_TEXT_ID	(UWORD)-3
#define GAD_STRING_ID	(UWORD)-1

typedef struct
{
	ConfigWindow req_dims;
	NewConfigWindow new_win;
	short gadget_count;
	short max_gadget_width;
	short max_text_width;
	short text_lines;
	short fine_pos,start_pos,width,gad_space,gad_fine_space;
	short object_count;
	struct Window *window;
	ObjectDef *objects;
	ObjectList *objlist;
	unsigned char *keys;
	struct TagItem tags[9],tags2[7],rel_tags[2];
	BOOL strings_2;
	short string_len;
	IPCData *ipc;
	char *string_buffer;
	ULONG waitbits;
	short first_object;
	char **gadgets;
	struct MyLibrary *libbase;
	short signal;
	struct GalileoSimpleRequest *simple;

	APTR parent;
	APTR memory;

	char *string_buffer_2;
	int string_len_2;
} simplereq_data;

BOOL _simplereq_open(simplereq_data *data,void *parent);
void _simplereq_close(simplereq_data *data);
BOOL simple_build(simplereq_data *data);
