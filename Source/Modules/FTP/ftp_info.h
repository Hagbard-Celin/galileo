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

#ifndef _FTP_INFO_H
#define _FTP_INFO_H

#ifndef _FTP_GLOBALS_H
#include "ftp_globals.h"
#endif
#ifndef _FTP_SOCKETADDR_H
#include "ftp_socketaddr.h"
#endif

#define IOBUFSIZE	(8* 1024)				// What is the most effective size for this
								// depends on speed of link. Largest observed
								// recv() size localhost is 8K
								// for agnus, largest is 536 bytes !

#define	BUFIOBUFSIZE	1024					// size for sgetc buffer for each socket/ftp_info

//
//	Every FTP server has an ftp_info structure:
//
struct ftp_info
{
	struct galileoftp_globals *fi_og;			// Global data pointer - we really shouldn't need this at this low level!!
	struct Task		  *fi_task;			// Task who owns this socket
	struct sockaddr_in	  fi_addr;			// Socket address
	int			  fi_cs;			// Control socket
	LONG			  fi_port;			// Port 21 or User selected port
	int			  fi_timeout;
	ULONG			  fi_flags;			// Server info: which commands are not supported, OS type, etc
	ULONG			  fi_abortsignals;		// Signal the process (ipc->proc) with this when abort pressed (CTRL_C or CTRL_D)
	char			  fi_iobuf[IOBUFSIZE + 1];	// I/O buffer for control socket and get, put, list
	char			  fi_bufiobuf[BUFIOBUFSIZE + 1];// Buffered I/O buffer used by iread & friends
	int			  fi_reply;			// FTP reply code
	int			  fi_errno;			// Last error code (for stuff other than ftp replies)
	int			  fi_aborted;			// Did we abort the last get, put, or getput?
	LONG			  fi_ioerr;			// Result from IoErr() on dos error
	char			  fi_serverr[IOBUFSIZE + 1];	// Copy off error server error reply

	int			  fi_found_index;		// GP Market flags for index, files.bbs etc
	int			  fi_found_index_size;		// GP Size of index file found
	int			  fi_found_fbbs_size;		// GP Size of Files.BBS
	int			  fi_doing_noop;		// GP Flag to suppress log output
	int			  fi_buffer_left;		// GP
	char			  *fi_buffer_pos;		// GP
};


#endif
