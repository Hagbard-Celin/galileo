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
#include <clib/alib_protos.h>
#include <proto/input.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/asl.h>
#include <graphics/gfxmacros.h>
#include <datatypes/textclass.h>
#include <gfm/simplerequest_protos.h>
#include <gfm/wbarg.h>
#include <gfm/rect.h>
#include <gfm/windowid_protos.h>
#include <gfm/cli_launch.h>
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "modules_lib.h"
#include "modules_internal_protos.h"

#define HEX_DISPLAY_WIDTH	16

enum
{
	READCOM_BASE,
	READCOM_READ,
};

#define READCHUNK_SIZE		65536
#define READ_READ_SIZE		32768

typedef struct
{
	struct MinNode			node;
	long				size;
	char				buffer[1];
} text_chunk;

typedef struct
{
	struct MinNode			node;
	char				*text;
	unsigned short			length;
} read_line;

#define LINES_PER_BLOCK	512

typedef struct
{
	read_line			lines[LINES_PER_BLOCK];
} read_block;

typedef struct
{
	struct Screen			*screen;
	struct Screen			*my_screen;
	struct Window			*window;
	struct DrawInfo			*drawinfo;
	APTR				vi;
	struct Menu			*menus;
	struct List			boopsi_list;

	IPCData				*ipc;
	char				*file;

	APTR				memory;

	char				*text_buffer;
	long				text_buffer_size;

	struct MinList			text;
	read_line			*top_line;
	long				top_position;

	long				lines;
	long				columns;

	long				v_visible;
	long				top;
	long				h_visible;
	long				left;

	struct Gadget			*vert_scroller;
	struct Gadget			*horiz_scroller;

	short				tab_size;
	short				mode;

	struct Rectangle		disprect;

	char				title[128];
	char				date[16];
	long				size;

	char				search_text[80];
	short				search_flags;

	read_line			*search_line;
	short				search_char;
	short				search_len;

	read_line			*search_last_line;
	short				search_last_linepos;
	short 				search_last_c_x;
	short 				search_last_c_xend;

	long				text_lines;
	char				hex_display[80];

	short				ansi_pen_array[16];
	unsigned short			pen_alloc;

	long				style;
	struct TextFont			*font;

	BOOL				no_next;

	short				slider_div;
	short				flags;

	read_block			*current_block;
	short				current_line;

	char				screen_name[40];

	IPCData				*main_ipc;

	char				*initial_search;

	long				sel_top_pos;
	long				sel_top_line;
	long				sel_bottom_pos;
	long				sel_bottom_line;
	struct Rectangle		text_box;

	char				line_buffer[368];

	struct AppWindow		*appwindow;
	struct MsgPort			*appport;

	struct List			*files;
	struct Node			*current;

	struct FileRequester		*filereq;

	struct read_startup		*startup;

	BOOL				flags_ok;

	WindowID			window_id;

	TimerHandle			*scroll_timer;
	short				scroll;

	struct IOStdReq			input_req;
	struct Library			*input_base;

	struct MinList			text_data;
	text_chunk			*search_last_chunk;

	APTR				progress;
	short				abort_bit;

	struct AppIcon			*app_icon;
	struct DiskObject		*app_diskobj;
} read_data;

struct read_startup
{
	struct List	*files;
	char		initial_search[80];
	IPCData		*owner;
	struct IBox	dims;
	BOOL		got_pos;
};

#define READF_DONE_ANSI		(1<<0)
#define READF_HIGH_OK		(1<<1)
#define READF_RESIZED		(1<<2)

extern MenuData read_menus[];
extern ConfigWindow search_window;
extern ObjectDef search_objects[];

#define SEARCHF_NOCASE		(1<<0)
#define SEARCHF_WILDCARD	(1<<1)
#define SEARCHF_ONLYWORDS	(1<<2)
#define SEARCHF_REVERSE		(1<<3)

enum
{
	MODE_NORMAL,
	MODE_ANSI,
	MODE_HEX,
	MODE_SMART,
};

#define ABS(x) (((x)<0)?(-x):(x))

extern ULONG ansi_palette[PEN_COUNT];

void read_parse_set(char **ptr,unsigned short *);
void read_free(read_data *,struct read_startup *,IPCData *);
BOOL read_file(read_data *,ULONG);
struct Window *read_open_window(read_data *);
void read_close_window(read_data *);
BOOL read_set_mode(read_data *,short,BOOL);
BOOL read_view(read_data *);
void read_calc_size(read_data *);
void read_init(read_data *);
void read_build_title(read_data *);
void read_show_text(read_data *,long,long,short);
void read_update_text(read_data *,long,long,short);
void read_display_text(read_data *,read_line *,long,text_chunk *);
void read_get_width(read_data *);
void read_search(read_data *,BOOL);
BOOL read_search_line(read_data *,read_line *,short);
void read_show_search(read_data *,long);
void read_remove_highlight(read_data *);
BOOL read_pick_screen_mode(read_data *);
void read_pick_font(read_data *);
read_line __asm *read_alloc_node(register __a0 read_data *);
void read_print(read_data *);
void read_clipboard_copy(read_data *);
void clip_write_data(struct IOClipReq *,char *,long);
void read_save(read_data *);
void read_pick_editor(read_data *,short);
void read_check_bounds(read_data *);
void read_parse_string(char **,char *,short);
text_chunk *read_hex_pointer(read_data *,long *);
