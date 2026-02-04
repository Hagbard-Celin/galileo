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

#ifndef _GFM_FUNCTION_EDITOR_H
#define _GFM_FUNCTION_EDITOR_H

#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef _GALILEOFM_GADTOOLS_H
#include <gfm/gadtools.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif

#define FUNCEDF_NO_KEY		(1<<0)
#define FUNCEDF_LABEL		(1<<1)
#define FUNCEDF_CENTER		(1<<2)
#define FUNCEDF_SAVE		(1<<3)
#define FUNCEDF_IMAGE		(1<<4)

typedef struct
{
	APTR			object;
	ULONG			object_flags;
	Cfg_Function		*function;
} FunctionReturn;


extern ConfigWindow
	_function_editor_window,
	_function_editor_window_center,
	_function_editor_label_window,
	_function_ed_req_window;
extern ObjectDef
	_function_editor_label_objects[],
	_function_editor_image_objects[],
	_function_editor_normal_objects[],
	_function_editor_objects[],
	_function_editor_objects_use[],
	_function_editor_objects_save[],
	_function_ed_req_objects[];

#define FUNCTYPE_BUTTON		1
#define FUNCTYPE_MENU		2
#define FUNCTYPE_FILETYPE	3
#define FUNCTYPE_HOTKEY		4

typedef struct _FunctionEditorEntry {
	Att_Node	*node;		// Points back to node
	int		type;		// Entry type
	char		buffer[256];	// Entry buffer
} FunctionEditorEntry;

extern long _funced_flaglist[];
extern UWORD _function_type_labels[];
extern UWORD _argument_list[];

#define END_DELETE	1
#define END_DISABLE	2


#define FB_OUTPUTWIN    0
#define FB_OUTPUTFILE   1
#define FB_WB2F         2
#define FB_GALILEOFM2F      3
#define FB_RUNASYNC     4
#define FB_CDSOURCE     5
#define FB_CDDEST       6
#define FB_DOALLFILES   7
#define FB_RECURSEDIR   8
#define FB_RELOADFILE   9
#define FB_AUTOICONIFY  10
#define FB_NOQUOTE      11
#define FB_RESCANSOURCE 12
#define FB_RESCANDEST   13
#define FB_SHELLSTARTUP 14
#define FB_GALILEOFMSTARTUP 15

#define FF_OUTPUTWIN    (1<<FB_OUTPUTWIN)
#define FF_OUTPUTFILE   (1<<FB_OUTPUTFILE)
#define FF_WB2F         (1<<FB_WB2F)
#define FF_GALILEOFM2F      (1<<FB_GALILEOFM2F)
#define FF_RUNASYNC     (1<<FB_RUNASYNC)   
#define FF_CDSOURCE     (1<<FB_CDSOURCE)
#define FF_CDDEST       (1<<FB_CDDEST)
#define FF_DOALLFILES   (1<<FB_DOALLFILES)
#define FF_RECURSEDIR   (1<<FB_RECURSEDIR)
#define FF_RELOADFILE   (1<<FB_RELOADFILE)
#define FF_AUTOICONIFY  (1<<FB_AUTOICONIFY)
#define FF_NOQUOTE      (1<<FB_NOQUOTE)
#define FF_RESCANSOURCE (1<<FB_RESCANSOURCE)
#define FF_RESCANDEST   (1<<FB_RESCANDEST)
#define FF_SHELLSTARTUP (1<<FB_SHELLSTARTUP)
#define FF_GALILEOFMSTARTUP (1<<FB_GALILEOFMSTARTUP)

#define FF_ALLFLAGS		(~0)
#define FF_HOTKEYFLAGS	(FF_GALILEOFMSTARTUP|FF_SHELLSTARTUP|FF_NOQUOTE|FF_OUTPUTWIN|FUNCF_WAIT_CLOSE)


enum
{
	GAD_FUNCEDREQ_LISTER,
	GAD_FUNCEDREQ_OKAY,
	GAD_FUNCEDREQ_CANCEL,
};

extern MenuData _funced_menus[];

#endif

