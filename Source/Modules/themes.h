/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#define CATCOMP_NUMBERS
#include <stdlib.h>
#include <proto/asl.h>
#include <gfm/selectionlist.h>
#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "themes.strings"
#include "modules_lib.h"
#include "module.h"

#define SAVETHEME	0
#define LOADTHEME	1
#define BUILDTHEME	2
#define CONVERTTHEME	3

#define is_digit(c) ((c)>='0' && (c)<='9')
#define is_space(c) ((c)==' ' || (c)=='\t')

extern char *func_templates[];

long save_theme(struct Screen *,CONST GalileoCallbackInfo *,char *,BOOL);
void rexx_skip_space(char **);
short rexx_parse_word(char **,char *,short);
BOOL save_theme_background(APTR,CONST GalileoCallbackInfo *,char *,struct MsgPort *,char *,APTR);
BOOL save_theme_sound(APTR,CONST GalileoCallbackInfo *,char *,struct MsgPort *,char *,APTR);
BOOL save_theme_font(APTR,CONST GalileoCallbackInfo *,char *,struct MsgPort *);
BOOL save_theme_pens(APTR,CONST GalileoCallbackInfo *,char *,struct MsgPort *);
BOOL save_theme_palette(APTR,CONST GalileoCallbackInfo *,struct MsgPort *);
BOOL theme_copy_file(char *,char *);
Att_List *theme_build_list(char *path);
void write_theme_intro(APTR,char *);
void write_theme_outro(APTR);
short convert_theme(CONST GalileoCallbackInfo *,char *,BPTR,char *);
void convert_theme_file(char *);

extern char *pen_settings[];

enum
{
	APPLY_PALETTE,
	APPLY_FONTS,
	APPLY_PICTURES,
	APPLY_SOUNDS,

	APPLY_LAST,
};

extern char apply_lookup[],*sound_lookup[];
