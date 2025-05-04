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
#include "update.strings"
#include "modules_lib.h"

#define UPDATEF_DONE_PATHS	(1<<0)
#define UPDATEF_DONE_LEFTOUTS	(1<<1)
#define UPDATEF_DONE_GROUPS	(1<<2)
#define UPDATEF_DONE_STORAGE	(1<<3)
#define UPDATEF_DONE_FILETYPES	(1<<4)
#define UPDATEF_DONE_POSITION	(1<<5)
#define UPDATEF_DONE_PATH_KEYS	(1<<6)
#define UPDATEF_DONE_COMMANDS	(1<<7)
#define UPDATEF_DONE_DESKTOP	(1<<8)
#define UPDATEF_DONE_THEMES 	(1<<9)
#define UPDATEF_DONE_IFFCHUNK	(1<<10)
#define UPDATEF_DONE_PATHFORMAT	(1<<11)
#define UPDATEF_DONE_FIXGROUPS	(1<<13)

struct Window *open_status(struct Screen *);
BOOL update_do_leftouts(struct List *,APTR);
BOOL update_groups(void);
BOOL update_iffchunk(void);
short update_convert_leftouts(APTR file,APTR memory,struct List *list);
BOOL update_storage(void);
void update_filetypes(void);
void update_envarc(void);
void update_envarc_do(char *path);
APTR read_position_info(struct List *list);
void update_do_pathkeys(struct List *list);
