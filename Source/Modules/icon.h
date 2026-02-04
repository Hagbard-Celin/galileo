/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
CopyRight 2024,2025 Hagbard Celine

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
#include <stdio.h>
#include <math.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/asl.h>
#include <proto/mathffp.h>
#include <proto/mathtrans.h>
#include <libraries/mathffp.h>
#include <dos/filehandler.h>
#include <graphics/gfxmacros.h>
#include <workbench/icon.h>
#include <gfm/galileo_ipc.h>
#include <gfm/glistview.h>
#include <gfm/strings_protos.h>
#include <gfm/devices.h>
#include <gfm/paths_protos.h>
#include <gfm/simplerequest_protos.h>
#include <gfm/windowid_protos.h>
#include <gfm/drag_routines_protos.h>
#include <gfm/icon_cache_protos.h>
#include <gfm/popup_protos.h>
#include <gfm/rect.h>
#include <gfm/cfg_flags.h>
#include <gfm/wbarg.h>
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "modules_lib.h"
#include "/Program/main_commands.h"


#define BACKDROP_GROUP_OBJECT	(1<<30)

// Special flag for IconEdit launching
#define LAUNCHF_OPEN_UNDER_MOUSE	(1<<30)

// Our custom IPC events
#define ICONINFO_TOOLTYPE	980316
#define ICONINFO_ICON		980317

// Seperate enabled/disabled tooltypes
#define SORT_TOOLTYPE_SEPERATED	32

#define AREAVERTEX	190

// RMB popup menu
enum
{
	IIM_EDIT,
	IIM_PICK_EDITOR,
	IIM_BARN1,
	IIM_DISK,
	IIM_DRAWER,
	IIM_TOOL,
	IIM_PROJECT,
	IIM_BAR0,
	IIM_BORDERLESS,
	IIM_LABEL,
	IIM_BAR1,
	IIM_STRIP_NEWICONS,
	IIM_STRIP_OLD,
	IIM_BAR2,
	IIM_NEWICONS,
	IIM_OLDICONS,
	IIM_GARBAGE,

	IIM_COUNT
};

// Drag'n'drop popup menu
enum
{
	IDM_OLDICONS,
	IDM_NEWICONS,
	IDM_BOTHICONS,
	IDM_BAR,
	IDM_TOOLTYPES,

	IDM_COUNT
};

enum
{
	USEDPEN,
	FREEPEN,
	USEDSHADOWPEN,
	FREESHADOWPEN,
};

struct seglist
{
	ULONG Size;
	BPTR  Next;
	UBYTE Code;
};

typedef struct
{
	char			buf[256];
	char			handler[265];
} icon_temp;

typedef struct
{
	APTR			memhandle;
	struct Screen		*screen;
	struct Window		*window;
	IPCData			*ipc;

	struct FileInfoBlock	fib;
	struct InfoData		info;

	NewConfigWindow		new_win;
	ObjectList		*list;

	char			*icon_name;
	char			*prog_name;
	char			*name;
	char			title[256];
	char			*object_name;

	struct DiskObject	*icon;
	struct DiskObject	icon_copy;

	struct Image		*image[2];
	short			image_num;
	struct Rectangle	icon_area;

	Att_List		*prot_list[4];
	Att_List		*tool_list;
	Att_Node		*edit_tool;
	BOOL			tool_new;
	BOOL			protection;

	char			datebuf[16];
	char			timebuf[16];
	ULONG                   locale_flags;

	char			decimal_sep;

	BOOL			group;

	IPCData			*main_ipc;

	ULONG			disktype;

	struct DiskObject	*new_icon;
	ULONG			icon_highlight;
	short			icon_width;
	short			icon_height;

	struct MsgPort		*app_port;
	struct AppWindow	*app_window;

	struct MsgPort		*notify_port;
	APTR			notify_req;

	long			icon_type;

	unsigned long		seconds;
	unsigned long		micros;
	char			buffer[256];

	PopUpMenu		menu;
	PopUpItem		menu_items[IIM_COUNT+1];

	PopUpMenu		dropmenu;
	PopUpItem		dropmenu_items[IDM_COUNT+1];

	BOOL			wb_info;
	BOOL			remap;
	BOOL			newicon_flag;
	BOOL			strip_newicon;
	BOOL			strip_oldicon;

	char			*path;
	char			*newicon_path;

	short			icon_mode;

	struct Library		*newicon_base;
	struct Library		*MathBase;
	struct Library		*MathTransBase;

	struct DiskObject	*new_oldicon;
	struct DiskObject	*new_newicon;
	int			label;
	BOOL			first_remap;
	int			which_images;
	int			drag_item;
	int			drag_x;
	int			drag_y;
	DragInfo		*draginfo;
	ULONG			old_idcmp;
	ULONG			old_winflags;
	TimerHandle		*timer;
	int			tick_count;
	int			last_tick;
	BOOL			busy;

	short			use_iconedit;
	char			edit_command[256];

	BOOL			modified;

	char			tempname[256];

	UQUAD			f_size64;
	
	char			volume[32];
	
	unsigned short		pens[4];
	short			pen_alloc[4];

	struct TmpRas		tmpras;
	PLANEPTR		rasbuf;
	struct AreaInfo		areainfo;

	unsigned char		__aligned areabuf[AREAVERTEX*5];

	ULONG			filesystem;

	struct Process		*taskport;

	ULONG			icon_filesize;

	char			*nameptr;

	FLOAT			used_percent;
} icon_data;

void icon_free(icon_data *data);
void closewindow(icon_data *data);
short icon_info(icon_data *data,char *name,struct Node *);
void icon_draw_icon(icon_data *data);
void icon_fix_toolgads(icon_data *data,Att_Node *sel);
BOOL icon_save(icon_data *data,char *save_name,BOOL);
void image_to_bitmap(icon_data *,struct Image *image,struct BitMap *bitmap,short depth);
void diskinfo_show_graph(icon_data *,struct Rectangle *,unsigned long,unsigned long);
void get_icon_fsize(icon_data *data);
void icon_show_image_data(icon_data *data);

extern ConfigWindow *icon_windows[];
extern ObjectDef
	icon_info_objects[],
	icon_info_next_object[],
	icon_file_info_objects[],
	icon_tooldir_objects[],
	icon_tool_objects[],
	icon_project_objects[],
	icon_toolproj_objects[],
	icon_toolproj_right[],
	icon_disk_objects[],
	icon_pcent_free_objects[],
	icon_pcent_used_objects[];

extern short diskinfo_colours[4][3];
extern ULONG disktype_lookup[];

extern short protect_lookup[];
extern unsigned long filesystem_table[];

char *strstri(char *string,char *substring);

extern MenuData icon_menus[];

extern short icon_type_labels[];

BOOL icon_switch_image(icon_data *data,short new);

extern struct Image oldicon_image;

Att_Node *tooltype_newnode( Att_List *list, char *name, ULONG data, ULONG flags);

