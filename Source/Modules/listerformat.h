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
#include "listerformat.strings"
#include "modules_lib.h"

extern ConfigWindow
	listformat_window,listformat_window_big,
	listformat_window_ftp,listformat_window_ftp_big;
extern ObjectDef
	listformat_objects[],listformat_ftp_objects[],listformat_nosave_objects[],
	listformat_save_objects[],listformat_ftp_use_objects[],listformat_ftp_use_objects_big[];

enum
{
	GAD_LISTER_LAYOUT,
	GAD_LISTER_USE,
	GAD_LISTER_CANCEL,
	GAD_LISTER_FORMAT_SELITEMS,
	GAD_LISTER_FORMAT_ITEMS,
	GAD_LISTER_FORMAT_SEPARATION,
	GAD_LISTER_FORMAT_REVERSE,
	GAD_LISTER_REJECT_ICONS,
	GAD_LISTER_HIDDEN_BIT,
	GAD_LISTER_SHOW,
	GAD_LISTER_HIDE,
	GAD_LISTER_SAVE,
	GAD_LISTER_FUEL_GAUGE,
	GAD_LISTER_INHERIT,
	GAD_FTP_DEFAULTS,
	GAD_DEFAULTS,
	GAD_SET_AS_DEFAULTS,
};

typedef struct {
	NewConfigWindow		newwin;
	struct Window		*window;
	ObjectList		*objlist;
	ListFormat		format;
	Att_List		*item_list;
	Att_List		*item_sel_list;
	DragInfo		*drag;
	short			drag_x,drag_y;
	short			drag_from;
	Att_Node		*drag_node;
	GL_Object		*sel_lister;
	ListFormatStorage	*def_format;
	ListFormatStorage	*def_ftp_format;
	ULONG			mod_id;
	ListFormatStorage	def_format_copy;
} config_lister_data;

void _config_lister_init_format(config_lister_data *data);
void _config_lister_cleanup(config_lister_data *data);
void _config_lister_get_values(config_lister_data *data);
void listformat_get_values(config_lister_data *data,ListFormat *format);
void listerformat_end_drag(config_lister_data *data,BOOL ok);
void listerformat_position(config_lister_data *data,Att_Node *node);
void listerformat_defaults(config_lister_data *data);
void listerformat_setas_defaults(config_lister_data *data);
void listerformat_ftp_defaults(config_lister_data *data);
void listerformat_check_default(config_lister_data *data);
