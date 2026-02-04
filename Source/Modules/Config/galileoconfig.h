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

#ifndef _GALILEOCONFIG
#define _GALILEOCONFIG

#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOSCREENDATA_H
#include <gfm/galileoscreendata.h>
#endif
#ifndef GALILEOFM_COLORSPEC_H
#include <gfm/colorspec.h>
#endif
#ifndef _GALILEOCONFIG_BUTTONSTARTUP_H
#include <gfm/buttonstartup.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef _GALILEOFM_CFG_ENVIRONMENT_H
#include <gfm/cfg_environment.h>
#endif
#ifndef GALILEOFM_CFG_FILETYPE_H
#include <gfm/cfg_filetype.h>
#endif
#ifndef _GALILEOCONFIG_IPC_COMMANDS_H
#include "galileoconfig_ipc_commands.h"
#endif
#include "galileoconfig_pragmas.h"

void Test(void);
unsigned long Config_Environment(Cfg_Environment *,struct Screen *,UWORD *,IPCData *,IPCData *,UWORD,ULONG *,char *,Att_List *);
int Config_Buttons(ButtonsStartup *,IPCData *,IPCData *,struct Screen *,ULONG);
short Config_Filetypes(struct Screen *,IPCData *,IPCData *,ULONG,char *);
ULONG Config_Menu(char *,IPCData *,IPCData *,struct Screen *,Cfg_ButtonBank *,char *,ULONG,char *,short,Att_List *);
BOOL ConvertConfig(char *,struct Screen *,IPCData *);
ULONG Config_Menus(IPCData *,IPCData *,struct Screen *,Cfg_ButtonBank *,ULONG,ULONG,char *);
Cfg_Function *Config_EditFunction(IPCData *,IPCData *,struct Window *,Cfg_Function *,APTR,ULONG);
long ShowPaletteBox(struct Window *,GalileoScreenData *,short *,short *,struct TextAttr *,ColourSpec32 *,short *);
BOOL FunctionExportASCII(char *,Cfg_Button *,Cfg_Function *,ULONG);

// Edit filetype
Cfg_Filetype *EditFiletype(Cfg_Filetype *,struct Window *,IPCData *,IPCData *,ULONG);


struct colrow_data
{
	short col,row;
	short flag;
};

enum
{
	MENUTYPE_USER,
	MENUTYPE_LISTER,
};

#endif
