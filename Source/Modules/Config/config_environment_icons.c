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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au

*/

#include "config_lib.h"
#include "config_environment.h"

void _env_select_icon_font(
	config_env_data *data,
	char *name,
	UBYTE *size,
	UBYTE *fpen,
	UBYTE *bpen,
	UBYTE *mode,
	short bufsize)
{
	short initial_fpen,initial_bpen;
	char *mode_list[6];

	// No requester?
	if (!data->icon_font_req) return;

	// Fill out mode list array
	mode_list[0]=GetString(locale,MSG_FONT_DRAWMODE);
	mode_list[1]=GetString(locale,MSG_FONT_TEXT);
	mode_list[2]=GetString(locale,MSG_FONT_FIELD_TEXT);
	mode_list[3]=GetString(locale,MSG_FONT_SHADOW);
	mode_list[4]=GetString(locale,MSG_FONT_OUTLINE);
	mode_list[5]=0;

	// Fix initial pens
	initial_fpen=*fpen;
	initial_bpen=*bpen;
	if (initial_fpen>=4 && initial_fpen<252 && data->palette_table)
		initial_fpen=data->palette_table[initial_fpen-4];
	if (initial_bpen>=4 && initial_bpen<252 && data->palette_table)
		initial_bpen=data->palette_table[initial_bpen-4];

	// Make window busy
	SetWindowBusy(data->window);

	// Show font requester
	if (AslRequestTags(data->icon_font_req,
			ASLFO_Window,data->window,
			ASLFO_TitleText,GetString(locale,MSG_SELECT_FONT),
			ASLFO_InitialName,name,
			ASLFO_InitialSize,(ULONG)*size,
			ASLFO_InitialFrontPen,initial_fpen,
			ASLFO_InitialBackPen,initial_bpen,
			ASLFO_InitialDrawMode,(ULONG)*mode,
			ASLFO_Flags,FOF_PRIVATEIDCMP|FOF_DOFRONTPEN|FOF_DOBACKPEN|FOF_DODRAWMODE,
			ASLFO_MaxFrontPen,data->font_pen_count,
			ASLFO_MaxBackPen,data->font_pen_count,
			ASLFO_FrontPens,data->font_pen_table,
			ASLFO_BackPens,data->font_pen_table,
			ASLFO_ModeList,mode_list,
			TAG_END))
	{
		// Store name and size
		stccpy(name,data->icon_font_req->fo_Attr.ta_Name,bufsize);
		*size=data->icon_font_req->fo_Attr.ta_YSize;

		// Store colours
		*fpen=_env_map_font_colour(data,data->icon_font_req->fo_FrontPen);
		*bpen=_env_map_font_colour(data,data->icon_font_req->fo_BackPen);
		*mode=data->icon_font_req->fo_DrawMode;
	}

	// Make window unbusy
	ClearWindowBusy(data->window);
}
