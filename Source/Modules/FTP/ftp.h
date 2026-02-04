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

#ifndef _FTP_H
#define _FTP_H

#define CATCOMP_NUMBERS
#include "ftp.strings"
#include "modules_lib.h"
#include "module.h"

#include "ftp_ad_internet.h"
#ifndef _FTP_CORE_H
#include "ftp_core.h"
#endif
#ifndef _FTP_INFO_H
#include "ftp_info.h"
#endif

#define CMDBUFSIZE 256
#define REPLYTIMEOUT 60


#define	VERSION_GALILEOFMLIB	0				// min galileo library version required

// FTP error codes (other than FTP replies)
enum
{
	FTPERR_NO_MEM = 1,					// Out of memory
	FTPERR_SOCKET_FAIL,					// A call to socket() failed
	FTPERR_BIND_FAIL,					// A call to bind() failed
	FTPERR_GETSOCKNAME_FAIL,				// A call to getsockname() failed
	FTPERR_LISTEN_FAIL,					// A call to listen() failed
	FTPERR_TIMEOUT,						// sgetc() timed out
	FTPERR_FAKE_421,					// getreply() returns 421 but didn't read it in

	FTPERR_XFER_SRCERR	= 1 << 15,			// Error on source end of get(), put(), getput()
	FTPERR_XFER_DSTERR	= 1 << 16,			// Error on destination end of get(), put(), getput()
	FTPERR_XFER_RARERR	= 1 << 17,			// Other error during get(), put(), list()

	FTPERR_XFER_MASK	= FTPERR_XFER_SRCERR | FTPERR_XFER_DSTERR | FTPERR_XFER_RARERR,
};

// PORT command flags
#define PORT_QUIET	(1<<0)					// Don't leave reply in reply buffer

// Internal getreply() flags
#define GETREPLY_QUIET	(1<<0)					// Don't overwrite iobuf

int  gethost   (struct sockaddr_in *, char *, struct Library *SocketBase);

int  ftpa      (struct ftp_info *, const char *fmt, ...);
int  getreply  (struct ftp_info *);

void ftp_abor  (struct ftp_info *);

int  ftp_dele  (struct ftp_info *, char *name);
int  ftp_image (struct ftp_info *);
int  ftp_pasv  (struct ftp_info *);
int  ftp_port  (struct ftp_info *, unsigned long flags, unsigned char *host, unsigned char *port);
int  ftp_rest  (struct ftp_info *, unsigned int offset);
int  ftp_rmd   (struct ftp_info *, char *name);
int  ftp_syst  (struct ftp_info *);

int  connect_host   (struct ftp_info *, int (*updatefn)(void *,int,char *), void *updateinfo, char *host, int port);
void disconnect_host(struct ftp_info *);
void lostconn       (struct ftp_info *);

int  login (struct ftp_info *, int (*updatefn)(void *,int,char *), void *updateinfo, char *user, char *passwd);
void logout(struct ftp_info *);

int          list(struct ftp_info *, int (*updatefn)(void *,const char *line), void *updateinfo, const char *cmd, const char *pathname);
unsigned int get (struct ftp_info *, int (*updatefn)(void *,unsigned int,unsigned int), void *updateinfo, char *remote, char *local, BOOL restart);
unsigned int put (struct ftp_info *, int (*updatefn)(void *,unsigned int,unsigned int), void *updateinfo, char *local, char *remote, unsigned int restart);

int do_lister_srcport(IPCMessage *msg);

// Convert PASV reply to address
BOOL pasv_to_address(struct sockaddr_in *address, const char *buf);

#endif
