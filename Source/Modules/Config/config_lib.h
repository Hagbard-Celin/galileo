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
#ifndef _GALILEOFM_MODULES_LIB
#define _GALILEOFM_MODULES_LIB

#include "//Library/galileofmbase.h"
#include "//Library/galileofmpragmas.h"
#include "//Program/galileo_config.h"
#include "galileoconfig.h"
#include "//Modules/modules.h"


#define CATCOMP_NUMBERS
#include "config.strings"

#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>
#endif

extern struct Library		*GalileoFMBase;
extern struct Library		*LayersBase;
extern struct Library		*DiskfontBase;
extern struct Library		*GadToolsBase;
extern struct Library		*WorkbenchBase;
extern struct Library		*IconBase;
extern struct Library		*AslBase;
extern struct Library		*CxBase;
extern struct Library		*UtilityBase;
extern struct Library		*P96Base;
extern struct LocaleBase	*LocaleBase;
extern struct GalileoLocale	*locale;

#ifdef RESOURCE_TRACKING
extern struct Library *ResTrackBase;

extern ULONG callerid;
#endif


// Config sub-option handles
typedef struct _SubOptionHandle {
	int num;		// Option number
	ULONG name;		// Option name ID
	ObjectDef *objects;	// Object list
} SubOptionHandle;

void init_locale_data(struct GalileoLocale *);
/*void KPrintF __ARGS((char *,...));*/
void lsprintf __ARGS((char *,...));

Att_List *build_sub_options(SubOptionHandle *);

IPCData *__saveds Local_IPC_ProcStartup(
	ULONG *data,
	ULONG (*code)(IPCData *,APTR));

#define VALID_QUALIFIERS (IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|\
			 IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|\
			 IEQUALIFIER_RSHIFT|IEQUALIFIER_LALT|IEQUALIFIER_RALT)

typedef struct
{
	struct Window		*window;

	DragInfo		*drag;
	Att_Node		*drag_node;
	short			drag_x;
	short			drag_y;

	long			tick_count;
	long			last_tick;
	TimerHandle		*timer;

	unsigned long		old_flags;
	unsigned long		old_idcmp;

	short			lock_count;
	short			flags;
} CfgDragInfo;

BOOL config_drag_check(CfgDragInfo *drag);
void config_drag_move(CfgDragInfo *drag);
void config_drag_start(CfgDragInfo *,Att_List *,short,struct TagItem *,BOOL);
struct Window *config_drag_end(CfgDragInfo *,short);
BOOL config_drag_send_button(CfgDragInfo *,IPCData *,Cfg_Button *,Cfg_ButtonFunction *);
Cfg_Button *config_drag_get_button(Cfg_Button *button,Cfg_ButtonFunction *func);
void config_drag_start_window(CfgDragInfo *,struct Window *,struct Rectangle *,short,short);

char *function_label(Cfg_ButtonFunction *);
BOOL config_filereq(struct Window *window,ULONG title,char *path,char *defpath,short flags);
BOOL config_valid_path(char *path);

Cfg_Instruction *instruction_from_wbarg(struct WBArg *,APTR);
void parse_number(char **,unsigned short *);
void store_window_pos(struct Window *window,struct IBox *pos);

long __asm L_ShowPaletteBox(
	register __a0 struct Window *,
	register __a1 GalileoScreenData *,
	register __a2 short *,
	register __a3 short *,
	register __a4 struct TextAttr *,
	register __a5 ColourSpec32 *,
	register __d0 short *);

BOOL __asm L_FunctionExportASCII(
	register __a0 char *,
	register __a1 Cfg_Button *,
	register __a2 Cfg_Function *,
	register __d0 ULONG);

short error_saving(short,struct Window *);

#include "enums.h"
#include "config_data.h"
#include "function_export.h"
#include "function_editor.h"
#include "select_colours.h"
#include "button_editor.h"

#endif
