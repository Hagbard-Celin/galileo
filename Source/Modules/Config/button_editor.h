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

typedef struct
{
	struct MinNode		node;
	Cfg_ButtonFunction	*func;
	IPCData			*editor;
} ButtonEditorNode;

typedef struct
{
	struct _bank_node	*bank;
	Cfg_Button		*button;
	Cfg_Button		*orig_button;
} ButtonReturn;

typedef struct
{
	struct Window		*window;
	Cfg_Button		*button;
	IPCData			*owner_ipc;
	struct _ButtonEdData	*data;
	FunctionStartup		func_startup;
	ButtonReturn		retdata;

	ConfigWindow		*win_text_def;
	ConfigWindow		*win_image_def;
	ObjectDef		*obj_def;

	PaletteBoxData		palette_data;

	ObjectDef		*obj_name;
	ObjectDef		*obj_image;

	ULONG			command_list;

	IPCData			*main_owner;

	ULONG			flags;
} ButtonEdStartup;

#define BUTTONEDF_NO_ADD	(1<<0)

typedef struct _ButtonEdData
{
	NewConfigWindow		newwin;

	IPCData			*ipc;
	struct Window		*window;	// Window
	ObjectList		*objlist;	// List of objects

	Cfg_Button		*button;	// Button being edited
	short			which;		// Function being editor (left/right/etc)
	GL_Object		*display;	// Button display area
	struct Rectangle	button_rect;	// Button display rectangle
	ButtonEdStartup		*startup;

	Att_List		*func_list;

	struct ListLock		proc_list;

	struct AppWindow	*appwindow;
	struct MsgPort		*app_port;

	CfgDragInfo		drag;

	GL_Object		*lister;
	ULONG			last_sec,last_mic;
	short			change;
	struct MinList		editors;

	short			change_bit;

	struct IBox		display_box;

	ImageRemap		remap;
} ButtonEdData;

extern ConfigWindow
	_button_editor_window;
extern ObjectDef
	_button_editor_objects[],
	_button_editor_image_objects[],
	_button_editor_label_objects[];

enum
{
	FUNCTION_LEFT,
	FUNCTION_RIGHT,
	FUNCTION_MIDDLE,
};

// prototypes
void __asm ButtonEditorTr(void);
void __asm ButtonEditor(void);
ULONG __asm _buttoned_init(register __a0 IPCData *,register __a1 ButtonEdStartup *);
void _buttoned_show_button(ButtonEdData *);
void _button_editor_edit_function(ButtonEdData *);
BOOL _button_editor_receive_edit(ButtonEdData *,FunctionReturn *);
void button_editor_init(ButtonEdStartup *);
void palette_editor_init(PaletteBoxData *);
void function_editor_init(FunctionStartup *,ULONG);
void buttoned_copy_string(ButtonEdData *,char *,char **);
Cfg_ButtonFunction *buttoned_get_function(ButtonEdData *,short);
void buttoned_start_drag(ButtonEdData *,struct IntuiMessage *);
void buttoned_end_drag(ButtonEdData *,BOOL);
short buttoned_get_drop(ButtonEdData *,short,short);
void buttoned_get_button(ButtonEdData *,Cfg_Button *,Point *);
void buttoned_free_function(ButtonEdData *,Cfg_ButtonFunction *,BOOL);
Cfg_ButtonFunction *buttoned_copy_function(ButtonEdData *,Cfg_Button *,Cfg_ButtonFunction *,Cfg_ButtonFunction *,short);
Cfg_ButtonFunction *buttoned_find_function(ButtonEdData *,short);
BOOL buttoned_function_empty(Cfg_ButtonFunction *);
void buttoned_fix_functionlist(ButtonEdData *);
ButtonEditorNode *buttoned_find_editor(ButtonEdData *,Cfg_ButtonFunction *);
void buttoned_add_function(ButtonEdData *);
void buttoned_del_function(ButtonEdData *);
void buttoned_build_function_list(ButtonEdData *data);
BOOL buttoned_swap_functions(ButtonEdData *,short,short);
void buttoned_refresh_functionlist(ButtonEdData *data);
void _button_editor_change_label(ButtonEdData *data,UWORD id,BOOL);
void _buttoned_close_window(ButtonEdData *data);
void buttoned_delete_function(ButtonEdData *data,Cfg_ButtonFunction *func,BOOL);
BOOL buttoned_check_empty(Cfg_ButtonFunction *func);
