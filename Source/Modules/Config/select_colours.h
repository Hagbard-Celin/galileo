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

typedef struct
{
	NewConfigWindow			newwin;

	union
	{
		struct
		{
			struct TagItem		palette_tags[4];
			ObjectDef		object_def[4];
		} stuff1;

		struct
		{
			char			gun_bits[3];
			char			pad;
			GL_Object		*palette_gadget[2];
		} stuff2;
	};

	IPCData				*owner_ipc;

	struct Library			*GalileoFMBase;
	struct Library			*IntuitionBase;

	unsigned long			flags;

	GalileoScreenData			screen_data;
	short				pen_array[18];

	unsigned char			fgpen;
	unsigned char			bgpen;

	IPCData				*main_ipc;

	short				pen_count;
	short				pad;

	struct TextAttr			*font;
	struct Window			*window;
	ObjectList			*list;

#if RESOURCE_TRACKING
    struct Library          *ResTrackBase;

    ULONG               pad1;
#else
	ULONG				pad1[2];
#endif
} PaletteBoxData;

#define PBF_NO_FG	(1<<0)
#define PBF_NO_EDIT	(1<<1)
#define PBF_DO_FONT	(1<<2)

extern ConfigWindow _palette_box_window,_palette_box_cust_window;
extern struct TagItem _palette_box_tags[];
extern ObjectDef _palette_box_objects[],_palette_box_cust_objects[];

void __saveds PaletteBox(void);
