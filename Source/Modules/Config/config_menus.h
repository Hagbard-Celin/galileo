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

#define MENUTYPE_USER		0
#define MENUTYPE_LISTER		1
#define MENUTYPE_KEYS		2
#define MENUTYPE_START		3

enum
{
	MENU_MENU,
	MENU_ITEM,
	MENU_SUB,
};

typedef struct
{
	IPCData			*ipc;		// IPC pointer
	IPCData			*owner_ipc;	// Owner IPC

	struct ListLock		proc_list;	// Process list
	struct List		edit_list;	// List of button editors

	NewConfigWindow		newwin;
	struct Window		*window;	// Window pointer
	ObjectList		*objlist;	// Object list

	Cfg_ButtonBank		*bank;		// Current button bank
	Cfg_ButtonBank		*restore_bank;	// Backup of original bank

	short			change;		// Has anything changed?
	short			ret_change;	// Return change?

	Point			window_pos;
	BOOL			pos_valid;
	ConfigWindow		dims;

	Att_List		*menu_list[3];
	Att_Node		*sel_item[3];

	struct AppWindow	*app_window;
	struct MsgPort		*app_port;

	ULONG			command_list;
	short			type;

	char			last_saved[256];

	CfgDragInfo		drag;
	short			active;
} config_menus_data;

typedef struct
{
	struct MinNode		node;
	Cfg_Button		*button;
	Cfg_ButtonFunction	*func;
	short			flags;
	IPCData			*ipc;
} menu_node;

#define MNF_SEP		(1<<0)

extern ConfigWindow	config_menu_window,config_menukeys_window;
extern ObjectDef	config_menu_objects[],config_menukeys_objects[];
extern MenuData		_lister_menu_menus[];
extern long		config_menus_title[];
extern char		*config_menus_default[];

BOOL config_menus_open(config_menus_data *,struct Screen *);
void config_menus_init(config_menus_data *);
void config_menus_close(config_menus_data *);
void config_menus_build_list(config_menus_data *,short);
void config_menus_check_disable(config_menus_data *,short);
Att_Node *config_menus_new_node(Att_List *,Cfg_Button *,Cfg_ButtonFunction *);
void config_menus_select_item(config_menus_data *,short,BOOL);
void config_menus_add_menu(config_menus_data *,BOOL,unsigned short);
void config_menus_add_item(config_menus_data *,short,BOOL,unsigned short,APTR);
void config_menus_del_menu(config_menus_data *);
void config_menus_del_item(config_menus_data *,short);
void config_menus_active(config_menus_data *,short);

void config_menus_new_name(config_menus_data *,short);
void config_menus_edit_item(config_menus_data *,short);
void config_menus_receive_edit(config_menus_data *,FunctionReturn *);
menu_node *config_menus_find_editor(config_menus_data *,Cfg_Button *,Cfg_ButtonFunction *);
void config_menu_kill_button_editors(config_menus_data *,Cfg_Button *);
void config_menus_editor_goodbye(config_menus_data *,IPCData *);
BOOL config_menus_check_change(config_menus_data *,BOOL);
void config_menus_move(config_menus_data *,short,unsigned short,unsigned short);
void config_menus_swap(config_menus_data *,Att_Node *,Att_Node *,short,Att_Node *);

BOOL config_menus_save(config_menus_data *,BOOL);
void config_menus_load(config_menus_data *,short);
Cfg_ButtonBank *config_menus_load_bank(config_menus_data *,Cfg_ButtonBank *,char *);
void config_menus_restore(config_menus_data *);

short config_menus_which_list(config_menus_data *,short,short);
void config_menus_appmsg(config_menus_data *,struct AppMessage *);
BOOL config_menus_import_bank(config_menus_data *,short,char *,Cfg_Button *);
BOOL config_menus_clip_button(config_menus_data *,Cfg_Button *,Point *);
void config_menus_end_drag(config_menus_data *,short,unsigned short);
void config_menus_move_item(config_menus_data *,short,Att_Node *,short,short,unsigned short);

void config_menus_add_func(config_menus_data *data,short type,struct WBArg *arg);

void config_menus_sort(
	config_menus_data *data,
	short type,
	short ascending);
