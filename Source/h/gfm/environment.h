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

#ifndef _GALILEOFM_ENVIRONMENT_H
#define _GALILEOFM_ENVIRONMENT_H

#ifndef _GALILEOFM_CFG_ENVIRONMENT_H
#include <gfm/cfg_environment.h>
#endif
#ifndef _GALILEOFM_READCFG_H
#include <gfm/readcfg.h>
#endif


#define OEDF_ENVR		(1<<0)
#define OEDF_SETS		(1<<1)
#define OEDF_TBAR		(1<<2)
#define OEDF_LMEN		(1<<3)
#define OEDF_UMEN		(1<<4)
#define OEDF_SCRP		(1<<5)
#define OEDF_HKEY		(1<<6)
#define OEDF_BANK		(1<<7)
#define OEDF_LSTR		(1<<8)
#define OEDF_DESK		(1<<9)
#define OEDF_PATH		(1<<10)
#define OEDF_SNDX		(1<<11)
#define OEDF_ALL		0xffffffff

struct OpenEnvironmentData
{
    APTR	    memory;
    APTR	    volatile_memory;
    ULONG	    flags;

    CFG_ENVR	    env;
    struct MinList  desktop;
    struct MinList  pathlist;
    struct MinList  soundlist;
    struct MinList  startmenus;
    struct MinList  buttons;
    struct MinList  listers;
    char	    toolbar_path[256];
    char	    menu_path[256];
    char	    user_menu_path[256];
    char	    scripts_path[256];
    char	    hotkeys_path[256];
};

BOOL OpenEnvironment(char *,struct OpenEnvironmentData *);
void UpdateEnvironment(CFG_ENVR *);

void DefaultEnvironment(CFG_ENVR *);

#endif

