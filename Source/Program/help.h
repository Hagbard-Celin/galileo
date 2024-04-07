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

#ifndef _GALILEOFM_HELP
#define _GALILEOFM_HELP

#include <proto/amigaguide.h>

#define GENERIC_BUTTON			"Button"
#define GENERIC_TOOLBAR_BUTTON		"ToolbarButton"
#define GENERIC_LISTER_MENU		"ListerMenu"
#define GENERIC_USER_MENU		"UserMenu"
#define HELP_ARROW_BUTTON		"Toolbar Arrow"
#define HELP_LISTER_MENU_POPUP		"Lister Menu PopUp"
#define HELP_LISTER_PATH_POPUP		"Lister Path PopUp"
#define HELP_LISTER_STATUS_POPUP	"Lister Status PopUp"
#define HELP_LISTER_FILE_COUNT		"Lister File Count"
#define HELP_LISTER_HELP		"Lister"
#define HELP_PROGRAM_GROUP		"Program Group"
#define HELP_MAIN			"Main"
#define HELP_APPMENUITEM		"AppMenuItem"

typedef struct
{
	USHORT		id;
	char		*name;
} MenuHelp;

void help_get_help(short,short,unsigned short);
void help_button_help(Cfg_Button *,Cfg_Function *,unsigned short,short,char *);
void help_menu_help(long,char *);
void help_show_help(char *,char *);
void __saveds help_proc(void);
AMIGAGUIDECONTEXT help_open_database(struct NewAmigaGuide *,char *,struct Screen *,char *);

enum
{
	HELPCMD_base,

	HELPCMD_LINK,
};

#define HELP_REAL	(1<<31)
#define HELP_NEWFILE	(1<<30)

#endif
