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

#ifndef _FTP_FD_SET_H
#define _FTP_FD_SET_H

#ifndef MAXFUPLIM
/*
 * MAXFUPLIM is the absolute limit of open files per process.  No process,
 * even super-user processes may increase u.u_maxof beyond MAXFUPLIM.
 * MAXFUPLIM means maximum files upper limit.
 * Important Note:  This definition should actually go into h/param.h, but
 * since it is needed by the select() macros which follow, it had to go here.
 * I did not put it in both files since h/param.h includes this file and that
 * would be error prone anyway.
 */
#define MAXFUPLIM 128			/* Max SOCKETS allowed */
#endif


typedef long fd_mask;


#define FD_SETSIZE MAXFUPLIM

#define NFDBITS (sizeof(fd_mask) * 8)	/* 8 bits per byte */


#ifndef howmany
#define howmany(x,y) (((x)+((y)-1))/(y))
#endif


typedef struct fd_set
{
	fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#endif
