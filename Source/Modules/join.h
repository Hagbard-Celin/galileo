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
#include "join.strings"
#include "modules_lib.h"
#include "module.h"

#define COPY_INITIAL_BUFFER 8192
#define COPY_MIN_BUFFER		2048
#define COPY_LOW_THRESH		500000
#define COPY_HIGH_THRESH	1000000

enum
{
	JOIN,
	SPLIT
};

enum
{
	JOINARG_FROM,
	JOINARG_TO
};

enum
{
	SPLITARG_FROM,
	SPLITARG_TO,
	SPLITARG_CHUNK,
	SPLITARG_STEM,
};

enum
{
	GAD_JOIN_LAYOUT,
	GAD_JOIN_LISTER,
	GAD_JOIN_ADD,
	GAD_JOIN_REMOVE,
	GAD_JOIN_CLEAR,
	GAD_JOIN_MOVE_UP,
	GAD_JOIN_MOVE_DOWN,
	GAD_JOIN_OK,
	GAD_JOIN_CANCEL,
	GAD_JOIN_TO_POPUP,
	GAD_JOIN_TO_FIELD,

	GAD_SPLIT_FROM_POPUP,
	GAD_SPLIT_FROM,
	GAD_SPLIT_TO_POPUP,
	GAD_SPLIT_TO,
	GAD_SPLIT_STEM,
	GAD_SPLIT_INTO,
	GAD_SPLIT_SKIP,
	GAD_SPLIT_OK,
	GAD_SPLIT_SIZES,
};

extern ConfigWindow join_window,split_window;
extern ObjectDef join_objects[],split_objects[];

typedef struct
{
	struct Screen		*screen;
	struct Window		*window;
	IPCData			*ipc;

	ConfigWindow		win_dims;
	NewConfigWindow		new_win;
	ObjectList		*list;

	struct MsgPort		*app_port;
	struct AppWindow	*app_window;

	Att_List		*join_list;

	char			source[256];
	char			dest[256];

	PathNode		*dest_path;

	struct timerequest	timer_req;
	struct Library		*TimerBase;

	short			function;

	char			buf[256];

	FuncArgs		*args;
} join_data;


BOOL join_open(join_data *);
void join_close(join_data *);
void join_free(join_data *);
void join_add_file(join_data *,char *,short);
void join_do_gadgets(join_data *,short);
void join_remove(join_data *);
void join_clear(join_data *);
void join_move(join_data *,short);
void join_add(join_data *);
short join_check_filename(join_data *);
BOOL join_do_join(join_data *);
BOOL join_join_files(join_data *);
short join_show_error(join_data *,short,char *,BOOL);
void split_do_gadgets(join_data *data,Att_Node *node);
short split_do_split(join_data *data);
short split_split_file(join_data *data);

extern char *func_templates[];
