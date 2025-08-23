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

#define SCRIPTF_NO_LOCK		(1<<0)
#define SCRIPTF_NO_SOUND	(1<<1)

#include "//Program/main_commands.h"

enum
{
	PICMENU_TYPE,
	PICMENU_CENTERED,
	PICMENU_TILED,
	PICMENU_STRETCHED,
	PICMENU_REMAP,
	PICMENU_NONE,
	PICMENU_GUI,
	PICMENU_ICON,
	PICMENU_IMAGE,
	PICMENU_EXACT,
	PICMENU_BORDER,
	PICMENU_NORMAL,
	PICMENU_BLACK,
	PICMENU_WHITE,
};

typedef struct
{
	NewConfigWindow		newwin;

	struct Window		*window;
	ObjectList		*objlist;
	ObjectList		*option_list;
	CFG_ENVR		*config;

	Att_List		*mode_list;		// List of screen modes
	char			mode_name[128];		// Current mode name
	struct Rectangle	mode_size_limit;	// Limits of screen mode size
	struct Rectangle	mode_size_default;	// Screen mode default size
	unsigned short		mode_max_colours;	// Screen mode max colours
	short			palette_colours;	// Number of colours in the palette
	short			palette_count;		// Total number of colours
	short			palette_array[16];	// Palette array
	short			*palette_table;

	short			sel_colour;		// Colour selected in palette
	char			gun_bits[4];		// Bits per gun
	short			slide_gun;		// Gun we are sliding

	Att_List		*options;

	Att_List		*lister_items;		// Lister display items
	struct FontRequester	*font_req;		// Font requester

	struct MsgPort		*app_port;
	struct AppWindow	*appwindow;

	GL_Object		*example;
	UBYTE			*colour_selection;

	short			palette_edit_pens[16];
	unsigned short		palette_edit_penalloc;
	unsigned short		palette_edit_freepen;
	short			alloc_pen_count;

	short			output_dims[4];
	IPCData			*ipc;

	short			own_screen;
	Cfg_Environment		*env;

	IPCData			*main_ipc;

	short			option;
	UWORD			initial_pen_alloc;
	Att_Node		*option_node;

	unsigned long		font_pens;
	unsigned short		font_pen_count;
	unsigned char		font_pen_table[16];

	Att_List		*desktop_drives[2];

	struct FontRequester	*icon_font_req;		// Font requester

	short			orig_colour_sel;
	GL_Object		*palette_gadget;
	short			p96_flag;
	short			colour_number;

	short			custom_pen[2];

	short			hide_type;

	char			last_saved[256];
	char			*settings_name;

	Att_List		*icon_settings;
	Att_List		*path_list;

	CfgDragInfo		drag;

	struct MinList		sound_list;
	Att_List		*script_list;
} config_env_data;


extern ObjectDef _lister_format_gadgets[];

BOOL _config_env_open(config_env_data *,struct Screen *);
void _config_env_close(config_env_data *);
void _config_env_cleanup(config_env_data *data);
void _config_env_set(config_env_data *data,short option);
void _config_env_store(config_env_data *data,short option);
void _config_env_screenmode_init(config_env_data *data,BOOL);
void _config_env_screenmode_fix_gadgets(config_env_data *data);
void _config_env_screenmode_init_mode(config_env_data *data,BOOL);
void _config_env_screenmode_fix_gadgets(config_env_data *data);
void _config_env_screenmode_set_mode(config_env_data *data,Att_Node *node);
void _config_env_screenmode_check_values(config_env_data *data);
void _config_env_palette_update_sliders(config_env_data *data);
void _config_env_palette_change_colour(config_env_data *data,int gun,ULONG val);
void set_rgb32(config_env_data *data,short col,ULONG *palette);
BOOL get_palette_file(config_env_data *data,char *name);
void _config_env_update_example(config_env_data *data,UBYTE *ptr);
void _config_env_palette_init(config_env_data *data);
void _config_env_colours_init(config_env_data *data);
void _config_env_update_output(config_env_data *data);
void _config_env_update_output_dims(config_env_data *data);

IPCMessage *_config_env_output_window_set(config_env_data *data,UWORD);
void _config_env_size_instructions(struct Window *window,UWORD);
void _config_env_reset_palette(config_env_data *data);
void _config_env_grabwb(config_env_data *data);
void _config_env_update_listersize(config_env_data *data);
void _config_env_status_list(ObjectList *list,ULONG id,long,long);
void _config_env_fix_font_pens(config_env_data *data);
unsigned char _env_map_font_colour(config_env_data *data,unsigned char col);
void _config_env_build_drive_list(config_env_data *data,short);
void _config_env_fix_drive_list(config_env_data *data,short);
BOOL _config_env_update_drives_list(config_env_data *data,short);
void config_env_check_device(config_env_data *data);

void _env_select_icon_font(
	config_env_data *data,
	char *name,
	UBYTE *size,
	UBYTE *fpen,
	UBYTE *bpen,
	UBYTE *mode,
	short bufsize);


extern MenuData button_toolbar_menu[];

void config_env_fix_picture_gads(config_env_data *);
void config_env_picture_menu(config_env_data *,short);

void _config_env_screenmode_show_depth(config_env_data *);

void config_env_build_pathlist(config_env_data *,BOOL);
void config_env_path_add(config_env_data *);
void config_env_path_sel(config_env_data *);
void config_env_path_del(config_env_data *);
void config_env_path_edit(config_env_data *);
void config_env_paths_end_drag(config_env_data *,BOOL);

extern UWORD _environment_iconsettings_labels[];

void config_env_show_sound(config_env_data *);
void config_env_store_sound(config_env_data *);
void config_env_test_sound(config_env_data *);
void env_init_sounds(config_env_data *);
