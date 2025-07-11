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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au

*/

#define ID_POSI		MAKE_ID('P','O','S','I')
#define ID_LOUT		MAKE_ID('L','O','U','T')
#define ID_ICON		MAKE_ID('I','C','O','N')

typedef struct
{
	struct Node		node;

	short			x;
	short			y;
	ULONG			flags;
	char			label[40];
	char			name[1];
} startmenu_record;


typedef struct
{
	struct Node		node;

	short			icon_x;
	short			icon_y;
	ULONG			flags;
	char			icon_label[32];
	char			name[1];
} leftout_record;

#define LEFTOUTF_NO_POSITION	(1<<0)

typedef struct position_record
{
	struct Node		node;

	short			icon_x;		// Position of icon
	short			icon_y;		// Position of icon

	struct IBox		text_dims;	// Window position in text mode
	struct IBox		icon_dims;	// Window position in icon mode

	ListFormatStorage	format;		// Format of lister

	UWORD			code;		// Hotkey
	UWORD			qual;
	UWORD			qual_mask;
	UWORD			qual_same;

	ULONG			flags;		// Flags

	char			pad;
	char			name[1];	// Full pathname
} position_rec;

#define PTYPE_POSITION		1
#define PTYPE_LEFTOUT		2
#define PTYPE_APPICON		3
#define PTYPE_STARTMENU		4

#define LISTERMODE_ICON		1
#define LISTERMODE_SHOW_ALL	2
#define LISTERMODE_ICON_ACTION	4

#define POSITIONF_USER		(1<<16)		// User-edited position
#define POSITIONF_NEW		(1<<17)		// New creation
#define POSITIONF_OPEN_NEW	(1<<18)		// Open new lister
#define POSITIONF_FORMAT	(1<<19)		// Valid path format
#define POSITIONF_ICON		(1<<20)		// Open in icon mode
#define POSITIONF_SHOW_ALL	(1<<21)		// Show All
#define POSITIONF_ICON_ACTION	(1<<22)		// Icon action mode
#define POSITIONF_TEMP		(1<<23)		// Temporary entry only
#define POSITIONF_POSITION	(1<<24)		// Has position info
#define POSITIONF_NEW_FLAG	(1<<25)

void GetPositions(struct ListLock *list,APTR memory,char *name);
position_rec *GetListerPosition(
	char *,
	char *,
	struct DiskObject *,
	struct IBox *,
	short *,
	ListFormat *,
	struct Window *,
	struct ListerWindow *,
	unsigned long);
position_rec *PositionUpdate(struct ListerWindow *,short);
void CopyPositions(struct ListLock *,struct List *,APTR);
void FreePositions(struct List *);
void SavePositions(struct List *,char *);
void PositionRemove(struct ListerWindow *,BOOL);


#define GLPF_USE_MODE		(1<<0)		// Mode supplied


#define POSUPF_SAVE		(1<<0)
#define POSUPF_FORMAT		(1<<1)
#define POSUPF_DEFAULT		(1<<2)
#define POSUPF_FAIL		(1<<3)
