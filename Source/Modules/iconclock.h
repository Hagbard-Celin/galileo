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

#include <ctype.h>
#include <stdlib.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/wb.h>

#define CATCOMP_NUMBERS
#include "iconclock.strings"
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules_lib.h"
#include "module.h"

#define ENV_ICONCLOCK		"Galileo/Icon Clock"
#define NAME_ICONCLOCK		"Icon Clock"

#define CLOCK_WIDTH	48
#define CLOCK_HEIGHT	17

#define NUMERAL_WIDTH	8
#define NUMERAL_HEIGHT	13
#define COLON_WIDTH	6
#define COLON_HEIGHT	13

extern struct Library __far	*MyLibBase;

typedef struct
{
	struct SignalSemaphore	sem;
	IPCData			*ipc;
} IconClockSemaphore;

typedef struct
{
	IPCData			*ipc;
	struct MsgPort		*notify_port;
	APTR			notify_req;

	struct DiskObject	clock_icon;
	struct Image		clock_image;

	struct RastPort		clock_rp;
	struct BitMap		clock_bm;

	struct AppIcon		*appicon;
	struct MsgPort		*app_port;

	long			icon_x;
	long			icon_y;
	char			buffer[256];

	IconClockSemaphore	sem;

	TimerHandle		*timer;
	long			day;
	char			date_buffer[20];

	struct Screen		*screen;
	short			pen;
	long			colour;

	IPCData			*main_ipc;

	struct Window		*about;
} iconclock_data;

void __asm icon_clock(void);
ULONG __asm iconclock_startup(
	register __a0 IPCData *,
	register __a1 iconclock_data *);
void iconclock_cleanup(iconclock_data *);
void iconclock_show(iconclock_data *);
void read_parse_set(char **,long *);
void iconclock_init(iconclock_data *,struct Screen *);

extern UWORD __chip numeral_data[10][13],colon_data[3][13];
void iconclock_info(iconclock_data *);
