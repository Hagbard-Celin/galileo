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

#ifndef _GALILEOFM_CONFIG_FLAGS_H
#define _GALILEOFM_CONFIG_FLAGS_H

// colours
enum
{
    ENVCOL_FILES_UNSEL,
    ENVCOL_DIRS_UNSEL,
    ENVCOL_FILES_SEL,
    ENVCOL_DIRS_SEL,
    ENVCOL_SOURCE,
    ENVCOL_DEST,
    ENVCOL_DEVICES,
    ENVCOL_VOLUMES,
    ENVCOL_GAUGE,

    ENVCOL_MAX=16
};

// CFG_SETS	copy_flags
#define		COPY_DATE	(1<<0)
#define		COPY_PROT	(1<<1)
#define		COPY_NOTE	(1<<2)
#define		COPY_ARC	(1<<3)
#define		COPY_CHECK	(1<<4)
#define		COPY_UPDATE	(1<<5)

// CFG_SETS	delete_flags
#define		DELETE_ASK	(1<<0)
#define		DELETE_FILES	(1<<1)
#define		DELETE_DIRS	(1<<2)
#define		DELETE_SET	(1<<3)

#endif

