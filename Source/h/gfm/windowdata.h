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

#ifndef GALILEOFM_WINDOWDATA_H
#define GALILEOFM_WINDOWDATA_H

#ifndef	EXEC_PORTS_H
#include <exec/ports.h>
#endif
#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef LIBRARIES_GADTOOLS_H
#include <libraries/gadtools.h>
#endif
#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif
#ifndef	LIBRARIES_ASL_H
#include <libraries/asl.h>
#endif
#ifndef GALILEOFM_WINDOWID_H
#include <gfm/windowid.h>
#endif
#ifndef _GALILEOFMBASE_LOCALE_H
#include <gfm/galileolocale.h>
#endif

typedef struct _WindowData
{
    WindowID		    id;		    // Window ID information

    ULONG		    pad3;
    struct FileRequester    *request;	    // Window's file requester
    APTR		    visinfo;	    // Visual info
    struct DrawInfo	    *drawinfo;	    // Draw info
    struct GalileoLocale    *locale;	    // Locale info
    struct MsgPort	    *window_port;   // Window message port (if supplied)
    struct NewMenu	    *new_menu;	    // NewMenu structure allocated
    struct Menu		    *menu_strip;    // Menu strip allocated
    struct Requester	    *busy_req;	    // Window busy requester
    ULONG		    data;	    // Window-specific data
    ULONG		    flags;	    // Flags
    APTR		    memory;	    // User memory chain, freed when window closes

    ULONG		    pad;
    struct FontRequester    *font_request;  // Window's font requester

    ULONG		    userdata;
    struct TagItem	    *user_tags;

    struct List		    boopsi_list;    // BOOPSI list

    struct Hook		    *backfill;	    // Backfill hook

    struct MinList	    object_list;
    struct MinList	    compound_gadgets;
} WindowData;

#define DATA(win) ((WindowData *)win->UserData)
#define DRAWINFO(win) (DATA(win)->drawinfo)
#define WINREQUESTER(win) (DATA(win)->request)
#define CFGDATA(win) (((WindowData *)win->UserData)->data)

#endif
