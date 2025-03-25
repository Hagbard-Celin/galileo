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

#include "galileofm.h"

// LOADFONTS internal function
GALILEOFM_FUNC(function_loadfonts)
{
	FunctionEntry *entry;
	ULONG change=0;
	BOOL run_prefs=0;

	// Get first entry
	if (entry=function_get_entry(handle))
	{
		APTR iff;

		// Build full name
		function_build_source(handle,entry,handle->func_work_buf);

		// Open IFF file
		if (iff=IFFOpen(handle->func_work_buf,IFF_READ,ID_PREF))
		{
			ULONG chunk;
			while ((chunk=IFFNextChunk(iff,ID_FONT))==ID_FONT)
			{
				struct FontPrefs fonts;
				if (IFFReadChunkBytes(iff,&fonts,sizeof(struct FontPrefs)))
				{
					// Icon label font?
					if (fonts.fp_Type==FP_WBFONT)
					{
						// Store new font
						stccpy(environment->env->font_name[FONT_ICONS],fonts.fp_Name,40);
						stccpy(environment->env->font_name[FONT_WINDOW_ICONS],fonts.fp_Name,40);
						environment->env->font_size[FONT_ICONS]=fonts.fp_TextAttr.ta_YSize;
						environment->env->font_size[FONT_WINDOW_ICONS]=fonts.fp_TextAttr.ta_YSize;

						// Store colours/style
						environment->env->icon_fpen=fonts.fp_FrontPen;
						environment->env->icon_bpen=fonts.fp_BackPen;
						environment->env->icon_style=fonts.fp_DrawMode;
						environment->env->iconw_fpen=fonts.fp_FrontPen;
						environment->env->iconw_bpen=fonts.fp_BackPen;
						environment->env->iconw_style=fonts.fp_DrawMode;
						change|=CONFIG_CHANGE_ICON_FONT|CONFIG_CHANGE_ICON_FONT_WINDOWS;
					}

					// Screen font (only on own screen)
					else
					if (fonts.fp_Type==FP_SCREENFONT)
					{
						// On own screen?
						if (GUI->screen)
						{
							// See if font is different
							if (stricmp(environment->env->font_name[FONT_SCREEN],fonts.fp_Name)!=0 ||
								environment->env->font_size[FONT_SCREEN]!=fonts.fp_TextAttr.ta_YSize)
							{
								// Store new font
								stccpy(environment->env->font_name[FONT_SCREEN],fonts.fp_Name,40);
								environment->env->font_size[FONT_SCREEN]=fonts.fp_TextAttr.ta_YSize;
								change|=CONFIG_CHANGE_DISPLAY;
							}
						}

						// On Workbench screen
						else
						if (GUI->screen_pointer)
						{
							// See if font is different
							if (stricmp(GUI->screen_pointer->RastPort.Font->tf_Message.mn_Node.ln_Name,fonts.fp_Name)!=0 ||
								GUI->screen_pointer->RastPort.Font->tf_YSize!=fonts.fp_TextAttr.ta_YSize)
							{
								run_prefs=1;
							}
						}
					}
				}
			}
			IFFClose(iff);
		}

		// End this entry
		function_end_entry(handle,entry,1);
	}

	// Change font?
	if (change)
	{
		// Run Font Prefs program?
		if (run_prefs)
		{
			// Build command
			lsprintf(handle->func_work_buf+256,"sys:prefs/font from %s use",handle->func_work_buf);
			CLI_Launch(
				handle->func_work_buf+256,
				GUI->screen_pointer,
				Lock(handle->func_source_path,ACCESS_READ),
				Open("nil:",MODE_OLDFILE),0,
				0,0);
		}

		// Send change command
		else rexx_send_reset(change,0);
	}
	return 0;
}
