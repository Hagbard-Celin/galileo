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

#ifndef _GALILEO_PATTERN_PROTOS_H
#define _GALILEO_PATTERN_PROTOS_H

#define PATTILE_SIZE	64

#include <intuition/classusr.h>
#include <prefs/wbpattern.h>

#define PATF_LOCK	0x00000002
#define PATF_STRETCH	0x10000000
#define PATF_CENTER	0x20000000
#define PATF_TILE	0x40000000
#define PATF_FILL	0x80000000

#define PATF_RANDOM	0x00004000

#define PATF_WB_MODE(f)		(((f)&0x3000000)>>20)
#define PATF_LISTER_MODE(f)	(((f)&0x30000)>>16)

#define ID_FIP0 MAKE_ID('F','I','P','0')
#define ID_WPOS MAKE_ID('W','P','O','S')

void GetPattern(PatternData *,struct Screen *,ULONG);
void GetPatternBorder(PatternData *,struct Screen *,ULONG);
void FreePattern(PatternData *);

void __asm backdrop_pattern_rotate(register __a0 UWORD *,register __d0 short);
void __asm __saveds PatternBackfill(
	register __a0 PatternInstance *,
	register __a1 struct BackFillInfo *,
	register __a2 struct RastPort *);

#define MOD(x,y) ((x)<0 ? (y)-((-(x))%(y)) : (x)%(y))

BOOL pattern_check_random(PatternData *,char *);

#endif
