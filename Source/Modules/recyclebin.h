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

#define CATCOMP_NUMBERS
#include "recyclebin.strings"
#include "modules_lib.h"

#define NAME_RECYCLEBIN		"Recycle Bin"
#define ENV_RECYCLEBIN		"Galileo/Recycle Bin"

extern ConfigWindow recyclebin_window;
extern ObjectDef recyclebin_objects[];

typedef struct
{
	struct SignalSemaphore	sem;
	IPCData			*ipc;
} RecycleSemaphore;

typedef struct
{
	char		*string;
	struct Screen	*screen;
	IPCData		*ipc;
	IPCData		*main_ipc;
	ULONG		mod_id;
	EXT_FUNC	(func_callback);
	long		err;
} RecyclePacket;

enum
{
	CMD_DELETE,
	CMD_PASS,
};

typedef struct
{
	ULONG			a4;
	struct Library		*library;
	struct Library		*module;

	IPCData			*ipc;
	struct MsgPort		*notify_port;
	APTR			notify_req;

	struct AppIcon		*appicon;
	struct MsgPort		*app_port;

	long			icon_x;
	long			icon_y;

	RecycleSemaphore	sem;

	TimerHandle		*timer;

	char			buffer[256];

	struct Window		*window;
	ObjectList		*objlist;
	NewConfigWindow		newwin;

	char			name[256];
	EXT_FUNC		(callback);

	APTR			function_handle;
	struct MsgPort		*reply_port;
} recyclebin_data;

enum
{
	GAD_RECYCLEBIN_LAYOUT=1,
	GAD_RECYCLEBIN_STORAGE,
	GAD_RECYCLEBIN_ADD,
	GAD_RECYCLEBIN_DELETE,
	GAD_RECYCLEBIN_STATS,
	GAD_RECYCLEBIN_EMPTY,
};

void recycle_bin(void);
ULONG __asm recyclebin_startup(
	register __a0 IPCData *,
	register __a1 recyclebin_data *);
void recyclebin_cleanup(recyclebin_data *,short);
void recycle_bin_install(RecyclePacket *,recyclebin_data *);
void read_parse_set(char **,long *);
void recyclebin_open_window(recyclebin_data *data);
void recyclebin_close_window(recyclebin_data *data);
long recyclebin_delete(recyclebin_data *,char *);
