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

#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <dos/filehandler.h>
#include <devices/trackdisk.h>
#include <gfm/simplerequest_protos.h>
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "modules_lib.h"

typedef struct
{
	struct Screen		*screen;
	struct Window		*window;
	IPCData			*ipc;

	ConfigWindow		win_dims;
	NewConfigWindow		new_win;
	ObjectList		*list;

	Att_List		*device_list;

	struct InfoData		info;
	struct FileInfoBlock	fib;
	char			disk_name[32];

	char			default_name[32];
	char			default_ffs;
	char			default_int;
	char			default_cache;
	char			default_long;
	char			default_trash;
	char			default_boot;
	char			default_verify;

	Point			window_pos;
	char			pos_valid;

	char			status_title[80];
	unsigned long	 	dos_type;
	int			latestffs_ver;

	char			selection[80];

	short			abort_bit;
} format_data;

BOOL format_open(format_data *data,BOOL);
void format_close(format_data *data);
void format_free(format_data *data);
void format_getlatestffs(format_data *data);
Att_List *get_device_list(char *);
void show_device_info(format_data *data);
BOOL start_format(format_data *data,unsigned short type,BOOL);
BOOL do_format(format_data *data,DiskHandle *disk,unsigned short type,short *);
BOOL do_raw_format(format_data *data,DiskHandle *disk,struct Window *status);
void do_install(format_data *data,DiskHandle *disk,struct Window *status);

extern ConfigWindow format_window;
extern ObjectDef format_objects[];

extern unsigned long bootblock_20[22];
