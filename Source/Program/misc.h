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

#ifndef _GALILEOFM_MISC
#define _GALILEOFM_MISC

enum
{
	TYPE_MENU,
	TYPE_HOTKEYS,
	TYPE_SCRIPTS,
};

#define READFILE_OK		 0
#define READFILE_NOT_FOUND	-1
#define READFILE_NO_MEMORY	-2

typedef struct
{
	ULONG		command;
	struct Window	*window;
	APTR		data;
	IPCData		*ipc;
} MiscStartup;


struct read_startup
{
	struct List	*files;
	char		initial_search[80];
	IPCData		*owner;
	struct IBox	dims;
	BOOL		got_pos;
};

typedef struct
{
	struct _BackdropInfo	*backdrop;
	struct _BackdropObject	*object;
	UWORD			qual;
	ULONG			flags;
	char			data[1];
} iconopen_packet;


#define WWF_LEAVE_LOCKED	(1<<0)
#define WWF_NO_LOCK		(1<<1)


enum
{
	KEYMATCH_INVALID,
	KEYMATCH_NONE,
	KEYMATCH_BUTTONS,
	KEYMATCH_TOOLBAR,
	KEYMATCH_LISTERMENU,
	KEYMATCH_MENU,
	KEYMATCH_START,
	KEYMATCH_HOTKEYS,
	KEYMATCH_SCRIPTS,
};

#endif
