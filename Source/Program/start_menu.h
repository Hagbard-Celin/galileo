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

#ifndef _GALILEOFM_STARTMENU_H
#define _GALILEOFM_STARTMENU_H

#define STARTMENUF_POPPED	(1<<0)

typedef struct _StartMenu
{
	CFG_STRT		data;
	char			*buttons;
	BPTR			buttons_parent_lock;
	short			x;
	short			y;

	IPCData			*ipc;
	struct Window		*window;
	APTR			imptr;
	Cfg_ButtonBank		*bank;
	struct SignalSemaphore	lock;

	struct Gadget		drag;

	WindowID		id;
	struct DrawInfo		*drawinfo;

	struct Rectangle	button;
	ImageRemap		remap;

	PopUpHandle		*menu;
	APTR			memory;

	short			im_width;
	short			im_height;
	short			label_width;
	short			no_alt;

	short			changed;

	struct AppWindow	*app_window;
	struct MsgPort		*app_port;

	struct TextFont		*font;
	struct TextFont		*label_font;

	ULONG			flags;

	PatternInstance		pattern;
	PatternData		pattern_data;

	short			last_id;
} StartMenu;

#endif
