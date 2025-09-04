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

/* Prototypes for functions defined in
ftp/ftp_addrsupp.c
 */

extern char const * logfile;

void Save_Config(struct display_globals *, BOOL);

BOOL save_sites(struct display_globals *dg,struct Window *win,short gadgetid);

BOOL save_options(struct window_params *, int, short);

Att_List *read_sites(struct window_params *, short);

Att_List *import_sites(struct window_params *, short);

struct ftp_config *read_options(struct window_params *, int);

void read_build_addressbook(struct galileoftp_globals *,IPCData *);

struct connect_msg *get_blank_connectmsg(struct galileoftp_globals *);

struct site_entry *get_blank_site_entry(struct galileoftp_globals *og);

void sort_list(Att_List *);

int get_site_entry(struct galileoftp_globals *og,struct site_entry *e,IPCData *ipc);

BOOL  get_global_options(struct galileoftp_globals *og);

void set_config_to_default(struct ftp_config *oc);

void copy_site_entry(struct galileoftp_globals *og,struct site_entry *e,struct site_entry *old);

BPTR setup_config(struct galileoftp_globals *);

void cleanup_config(BPTR);

BOOL export_sites(struct display_globals *dg,struct Window *win);

