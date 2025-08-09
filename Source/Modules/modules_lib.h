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
#ifndef _GALILEOFM_MODULES_LIB
#define _GALILEOFM_MODULES_LIB

#include "/Library/galileofmbase.h"
#include "/Library/galileofmpragmas.h"
#include "/Program/galileo_config.h"
#include "modules.h"

#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>

extern ULONG callerid;
#endif

#define VALID_QUALIFIERS (IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|\
			 IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|\
			 IEQUALIFIER_RSHIFT|IEQUALIFIER_LALT|IEQUALIFIER_RALT)

extern struct Library *GalileoFMBase;
extern struct Library *IconBase;
extern struct Library *LayersBase;
extern struct Library *UtilityBase;
extern struct Library *WorkbenchBase;
extern struct Library *GadToolsBase;
extern struct Library *DiskfontBase;
extern struct Library *AslBase;
extern struct RxsLib  *RexxSysBase;
extern struct GalileoLocale *locale;
extern ModuleInfo module_info;

#ifdef RESOURCE_TRACKING
extern struct Library * ResTrackBase;
#endif

void init_locale_data(struct GalileoLocale *);
/*void KPrintF __ARGS((char *,...));*/
void lsprintf __ARGS((char *,...));

#endif
