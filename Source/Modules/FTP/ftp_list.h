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

#ifndef _FTP_LIST_H
#define _FTP_LIST_H

//
//	Stuff to do with parsing LIST output
//

// Sizes of fields for FTP LIST ouput conversion routines
#define OGBLEN     32
#define GBMLEN     32
#define BDLEN      12
#define SECONDSLEN 12

struct ls_parse_buf
{
char ls_own_grp_byt[OGBLEN+1];	//	Could be Owner, Group, or Bytes
char ls_grp_byt_mon[GBMLEN+1];	//	Could be Group, Bytes, or Month
char ls_byt_dat    [ BDLEN+1];	//	Could be Bytes or Date
char ls_name[FILENAMELEN+2+3];	//	Allow for quotes + link symbol
char ls_seconds[SECONDSLEN+1];	//	Seconds from 1/1/78 (ascii)
LONG ls_protect;		//	Protection flags
char ls_comment[COMMENTLEN+1];	//	Used for link's real path
};

#endif
