/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2024 Hagbard Celine

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

#ifndef _GALILEOFM_CX
#define _GALILEOFM_CX

typedef struct _CxData
{
	struct NewBroker	nb;

	CxObj			*broker;

	CxObj			*popup_filter;
	CxObj			*rmb_hook;
	CxObj			*lmb_up_down;
	CxObj			*lmb_up_down_translator;

	struct InputEvent       lmb_up_down_ie1;
	struct InputEvent       lmb_up_down_ie2;
	struct InputEvent       lmb_up_down_ie3;

	struct Window		*button_window;
	struct Window		*popup_window;

	unsigned short		right_down;
	unsigned short		qual_down;
	short			start_popped;
} CxData;

enum
{
	CXCODE_POPUP,
	CXCODE_SELECT_UP_DOWN,
	CXCODE_NEW_LISTER,

	CXCODE_HOTKEY_BASE
};

void cx_install(CxData *);
void cx_remove(CxData *);
void cx_hotkey(long);
void cx_install_popup(CxData *);

BOOL cx_mouse_outside(struct Window *window,short,short);

#endif
