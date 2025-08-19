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

#ifndef _GALILEOFM_DIRLIST_PROTOS
#define _GALILEOFM_DIRLIST_PROTOS

#include "dirlist.h"

// Prototypes
// dirlist_read.c
void read_directory(Lister *,char *,BPTR,int);
Lister *read_new_lister(char *path,BPTR lock,Lister *parent,UWORD qual);
void removefile(DirBuffer *,DirEntry *);

// dirlist_scroll.c
void lister_update_slider(Lister *,int);
lister_pos_slider(Lister *,short);
void lister_scroll(Lister *,int,int);
void lister_show_selected(Lister *,int);
void lister_show_char(Lister *,char *);
void lister_show_entry(Lister *,DirEntry *);
BOOL lister_fix_horiz_len(Lister *lister);
void lister_calc_fieldwidth(Lister *lister,DirEntry *entry);
void lister_set_fieldwidth(Lister *lister,short which,char *string);

// dirlist_util.c
DirEntry *get_entry(struct MinList *list,BOOL selected,short type);
DirEntry *checktot(DirBuffer *);
DirEntry *checkdirtot(DirBuffer *);
DirEntry *checkdevtot(DirBuffer *);
DirEntry *checkalltot(DirBuffer *);
DirEntry *find_entry(struct MinList *,char *,long *,short);
DirEntry *get_entry_ord(struct MinList *,long);
BOOL list_is_custom(DirBuffer *);
void setdirsize(DirEntry *,unsigned long,Lister *);
void do_parent_root(Lister *,char *,BPTR);
void lister_sel_show(Lister *lister,char *name);

unsigned short scroller_top(
	unsigned short pot,
	unsigned short total,
	unsigned short visible);

long find_entry_offset(struct MinList *list,DirEntry *find);

void lister_check_max_length(Lister *,char *,short *,short);
void lister_check_max_length_tabs(Lister *,char *,short *,short);
short lister_get_length(Lister *lister,char *string);

#endif
