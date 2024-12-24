/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
CopyRight 2024 Hagbard Celine

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

#define CATCOMP_NUMBERS
#include <math.h>
#include "icon.strings"
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
	struct Screen		*screen;
	struct Window		*window;
	IPCData			*ipc;

	struct FileInfoBlock	fib;
	struct InfoData		info;

	NewConfigWindow		new_win;
	ObjectList		*list;

	char			icon_name[256];
	char			prog_name[256];
	char			name[256];
	char			title[256];
	char			object_name[256];

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

	char			path[256];
	char			newicon_path[256];

	short			icon_mode;

	struct Library		*newicon_base;

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


enum
{
	GAD_BASE,

	GAD_ICON_AREA,
	GAD_ICON_IMAGE,
	GAD_ICON_FILE_INFO,
	GAD_INFO_ICON_TYPE,
	GAD_INFO_FILE_DIMS,
	GAD_INFO_FILE_COLORS,
	GAD_INFO_FILE_MORE,
	GAD_INFO_FILE_SIZE,
	GAD_ICON_SAVE,
	GAD_ICON_NEXT,
	GAD_ICON_CANCEL,
	GAD_BOTTOM_LINE,

	GAD_ICON_BLOCKS,
	GAD_ICON_BYTES,
	GAD_ICON_STACK,
	GAD_ICON_PROTECTION_AREA,
	GAD_ICON_PROTECTION_LAYOUT,
	GAD_ICON_PROTECTION_HS,
	GAD_ICON_PROTECTION_PA,
	GAD_ICON_PROTECTION_RW,
	GAD_ICON_PROTECTION_ED,
	GAD_ICON_TYPE,
	GAD_ICON_LAST_CHANGED,
	GAD_ICON_COMMENT,
	GAD_TOOLTYPES_TOOL_AREA,
	GAD_TOOLTYPES_PROJECT_AREA,
	GAD_ICON_TOOLTYPES,
	GAD_ICON_TOOLTYPES_EDIT,
	GAD_ICON_TOOLTYPES_NEW,
	GAD_ICON_TOOLTYPES_DELETE,
	GAD_ICON_TOOLTYPES_SORT,
	GAD_ICON_DEFAULT_TOOL_GLASS,
	GAD_ICON_DEFAULT_TOOL,
	GAD_RIGHT_LAYOUT_AREA,
	GAD_PCENT_FREE_LAYOUT_AREA,
	GAD_PCENT_USED_LAYOUT_AREA,
	GAD_PCENT_FREE,
	GAD_PCENT_USED,
	GAD_SIZE_LAYOUT_AREA,
	GAD_LOWER_LAYOUT_AREA,
	GAD_ICON_STATUS,
	GAD_DEVICE,
	GAD_STATE,
	GAD_ERRORS,
	GAD_FILESYSTEM,
	GAD_HANDLER,
	GAD_VOL_NAME,
	GAD_DEV_NAME,
	GAD_USED_KEY_AREA,
	GAD_USED_KEY,
	GAD_FREE_KEY_AREA,
	GAD_FREE_KEY,
	GAD_PCENT_USED_KEY_AREA,
	GAD_PCENT_USED_KEY,
	GAD_PCENT_FREE_KEY_AREA,
	GAD_PCENT_FREE_KEY,
	GAD_USED,
	GAD_USED_MB,
	GAD_FREE,
	GAD_FREE_MB,
	GAD_CAPACITY,
	GAD_CAPACITY_MB,
	GAD_GRAPH,
	GAD_BLOCKSIZE,

	MENU_BORDERLESS,
	MENU_NO_LABEL,
	MENU_STRIP_NEWICONS,

	GAD_ICON_NAME1,
	GAD_ICON_NAME2,
	GAD_ICON_NAME3,

	GAD_VIRTUAL_PROTBITLIST,

	MENU_STRIP_OLD,
	MENU_NEWICONS,
	MENU_OLDICONS,

	MENU_ICON_EDIT,
	MENU_ICON_PICK_EDITOR,
	MENU_DISK,
	MENU_DRAWER,
	MENU_TOOL,
	MENU_PROJECT,
	MENU_GARBAGE,

	MENU_COPY_OLDICON,
	MENU_COPY_NEWICON,
	MENU_COPY_BOTHICONS,
	MENU_COPY_TOOLTYPES,
};

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

