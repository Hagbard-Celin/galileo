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
#include "pathformat.strings"
#include "modules_lib.h"

extern ConfigWindow pathformat_window;
extern ObjectDef pathformat_objects[];

#define CxBase (data->cxbase)

enum
{
	GAD_PATHFORMAT_LAYOUT,
	GAD_PATHFORMAT_SAVE,
	GAD_PATHFORMAT_CANCEL,
	GAD_PATHFORMAT_PATHS,
	GAD_PATHFORMAT_PATH_FOLDER,
	GAD_PATHFORMAT_PATH,
	GAD_PATHFORMAT_KEY,
	GAD_PATHFORMAT_ADD,
	GAD_PATHFORMAT_REMOVE,
	GAD_PATHFORMAT_EDIT,
	GAD_PATHFORMAT_NEW_LISTER,
};

enum
{
	MODE_NONE,
	MODE_NAME,
	MODE_ICON,
	MODE_ACTION
};

typedef struct
{
	NewConfigWindow		newwin;
	struct Window		*window;
	ObjectList		*objlist;

	struct List		*paths;
	Att_List		*path_list;
	Att_Node		*path_sel;

	struct Library		*cxbase;

	IPCData			*ipc;
	IPCData			*main_ipc;
	ListFormat		*def_format;

	struct AppWindow	*appwindow;
	struct MsgPort		*appport;

	APTR			memory;

	ULONG			seconds;
	ULONG			micros;
} config_path_data;

BOOL config_paths_open(config_path_data *data,struct Screen *screen);
void config_paths_cleanup(config_path_data *data);
void config_paths_build_list(config_path_data *data);
void config_paths_disable(config_path_data *data,BOOL state);
void config_paths_add(config_path_data *data,BOOL activate);
void config_paths_change(config_path_data *data);
void config_paths_select(config_path_data *data,Att_Node *node,BOOL);
void config_paths_get_key(config_path_data *data);
void config_paths_remove(config_path_data *data,BOOL);
void config_paths_edit(config_path_data *data);
