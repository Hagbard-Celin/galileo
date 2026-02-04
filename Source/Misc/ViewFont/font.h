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
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <proto/asl.h>
#include <proto/wb.h>


#define CATCOMP_NUMBERS
#include "font.strings"
#include "//Library/galileofmbase.h"
#include <gfm/devices.h>
#include <gfm/layout_protos.h>
#include "//Library/galileofmpragmas.h"

#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>
#endif

enum
{
	ARG_FONT,
	ARG_SIZE,
	ARG_BOLD,
	ARG_ITALIC,
	ARG_ULINE,
	ARG_SCREEN,
	ARG_COUNT
};

typedef struct
{
	struct Screen		*screen;
	struct Window		*window;
	struct GalileoLocale	locale;

	ConfigWindow		win_dims;
	NewConfigWindow		new_win;
	ObjectList		*list;

	struct TextAttr		attr;
	struct TextFont		*font;

	char			font_name[256];
	short			font_size;
	short			font_style;

	char			title[80];

	ULONG 			arg_array[ARG_COUNT];

	char			font_text[256];

	struct MsgPort		*appport;
	struct AppWindow	*appwindow;

	struct RDArgs		*args;
	BOOL			resized;
	BOOL			first;

	struct Hook		refresh_hook;

	char			last_font_name[256];
	char			**size_labels;

	struct Window		*about;

    BPTR 			lock;
} font_data;

BOOL font_open(font_data *data);
void font_close(font_data *data);
void font_free(font_data *data);
void font_get_font(font_data *data);
void font_show_font(font_data *data,BOOL refresh);
void font_get_name(font_data *data,char *name);
void font_ask_name(font_data *data);

extern ConfigWindow font_window;
extern ObjectDef font_objects[];

enum
{
	GAD_FONT_LAYOUT,

	GAD_FONT_FONT,
	GAD_FONT_SIZE,
	GAD_FONT_PRINT,
	GAD_FONT_UP,
	GAD_FONT_DOWN,
	GAD_FONT_BOLD,
	GAD_FONT_ITALIC,
	GAD_FONT_ULINE,
	GAD_FONT_DISPLAY,
	GAD_FONT_FONT_POPUP,
	GAD_FONT_CYCLE,

	MENU_OPEN_FONT,
	MENU_SAVE_SETTINGS,
	MENU_QUIT,
	MENU_ABOUT,
};


extern struct Library *GalileoFMBase;
extern struct LocaleBase *LocaleBase;
extern struct Library *LayersBase;
extern struct Library *AslBase;
extern struct Library *DiskfontBase;
extern struct LocaleBase *LocaleBase;
extern struct Library *WorkbenchBase;

#ifdef RESOURCE_TRACKING
extern struct Library *ResTrackBase;
#endif

extern MenuData font_menus[];

void init_locale_data(struct GalileoLocale *locale);
ULONG __asm font_refresh(register __a0 struct Hook *hook,register __a1 struct IntuiMessage *msg);
short font_get_size(font_data *data,short dir);
void font_build_labels(font_data *data,struct FontContentsHeader *fch);
void font_show_about(font_data *data);
void font_save_settings(font_data *data);
void font_read_settings(font_data *data);
void read_parse_set(char **ptr,unsigned short *val);

void lsprintf __ARGS((char *,...));
