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

#ifndef AD_INTERNET_INTERN_H
#define AD_INTERNET_INTERN_H

#ifndef _TYPES_
#include <sys/types.h>
#endif
#ifndef _FTP_FD_SET_H
#include "ftp_fd_set.h"
#endif
#ifndef _FTP_SOCKETADDR_H
#include "ftp_socketaddr.h"
#endif
#ifndef _FTP_SERVENT_H
#include "ftp_servent.h"
#endif


/* GENERIC */

int accept (int, struct sockaddr *, LONG *, struct Library *SocketBase);
int bind (int, struct sockaddr *, int, struct Library *SocketBase);
void cleanup_sockets (struct Library *SocketBase);
int connect (int, struct sockaddr *, int, struct Library *SocketBase);
struct hostent *gethostbyname (char *, struct Library *SocketBase);
int gethostname (char *, int, struct Library *SocketBase);
struct servent *getservbyname (char *, char *, struct Library *SocketBase);
struct servent *getservbyport (int, char *, struct Library *SocketBase);
int getsockname (int, struct sockaddr *, LONG *, struct Library *SocketBase);
u_long inet_addr (char *, struct Library *SocketBase);
int listen (int, int, struct Library *SocketBase);
int recv(int, char *, int, int, struct Library *SocketBase);
int s_close (int, struct Library *SocketBase);
int selectwait (int, fd_set *, fd_set *, fd_set *, struct timeval *, ULONG *, struct Library *SocketBase);
int send (int, char *, int, int, struct Library *SocketBase);
setsockopt(int i1,int i2,int i3,char *cp,int i4, struct Library *SocketBase);
ULONG setup_sockets (UWORD, int *, struct Library *SocketBase);
int shutdown (int, int, struct Library *SocketBase);
int socket(int, int, int, struct Library *SocketBase);

#endif
