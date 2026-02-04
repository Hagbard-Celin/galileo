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

#ifndef GALILEOFM_LAYOUT_H
#define GALILEOFM_LAYOUT_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef LIBRARIES_GADTOOLS_H
#include <libraries/gadtools.h>
#endif

#ifndef GALILEOFM_OBJECTLIST_H
#include <gfm/objectlist.h>
#endif
#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef _GALILEOFM_GADTOOLS_H
#include <gfm/gadtools.h>
#endif

#define MENUID(menu) ((ULONG)GTMENUITEM_USERDATA(menu))

// Set by the user
#define WINDOW_SCREEN_PARENT	(1<<0)	// Parent is a screen
#define WINDOW_NO_CLOSE		(1<<1)	// No close gadget
#define WINDOW_NO_BORDER	(1<<2)	// No border
#define WINDOW_LAYOUT_ADJUST	(1<<3)	// Adjust window size to fit objects
#define WINDOW_SIMPLE		(1<<4)	// Simple refresh
#define WINDOW_AUTO_REFRESH	(1<<5)	// Refresh window automatically
#define WINDOW_AUTO_KEYS	(1<<6)	// Handle keys automatically
#define WINDOW_OBJECT_PARENT	(1<<7)	// Parent is an existing object
#define WINDOW_REQ_FILL		(1<<8)	// Backfill as a requester
#define WINDOW_NO_ACTIVATE	(1<<9)	// Don't activate
#define WINDOW_VISITOR		(1<<10)	// Open as visitor window
#define WINDOW_SIZE_RIGHT	(1<<11)	// Size gadget, in right border
#define WINDOW_SIZE_BOTTOM	(1<<12)	// Size gadget, in bottom border
#define WINDOW_ICONIFY		(1<<13)	// Iconify gadget

// Opens a window
typedef struct _NewConfigWindow {
    void		    *parent;	// Parent to open on
    ConfigWindow	    *dims;	// Window dimensions
    char		    *title;	// Window title
    struct GalileoLocale    *locale;	// Locale to use
    struct MsgPort	    *port;	// Message port to use
    ULONG		    flags;	// Flags
    struct TextFont	    *font;	// Alternative font to use
} NewConfigWindow;

struct IntuiMessage *GetWindowMsg(struct MsgPort *);
struct Window *OpenConfigWindow(NewConfigWindow *);
void CloseConfigWindow(struct Window *);
struct MenuItem *FindMenuItem(struct Menu *,UWORD);
void AddWindowMenus(struct Window *,MenuData *);
ObjectList *AddObjectList(struct Window *,ObjectDef *);
void DisplayObject(struct Window *,GL_Object *,int fg,int bg,char *txt);
void DisableObject(ObjectList *,ULONG,BOOL);
long GetGadgetValue(ObjectList *,UWORD);
void SetGadgetValue(ObjectList *,UWORD,ULONG);
void UpdateGadgetValue(ObjectList *,struct IntuiMessage *,UWORD);
void SetWindowBusy(struct Window *);
void ClearWindowBusy(struct Window *);
void SetConfigWindowLimits(struct Window *,ConfigWindow *,ConfigWindow *);
void ReplyWindowMsg(struct IntuiMessage *);
void FreeObjectList(ObjectList *);
BOOL GetObjectRect(ObjectList *,ULONG,struct Rectangle *);

// misc.c
void ActivateStrGad(struct Gadget *,struct Window *);

#endif
