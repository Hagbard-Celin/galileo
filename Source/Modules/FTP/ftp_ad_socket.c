/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2026 Hagbard Celine

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


#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif
#ifndef _FTP_GLOBALS_H
#include "ftp_globals.h"
#endif
#ifndef _FTP_CORE_H
#include "ftp_core.h"
#endif
#ifndef _FTP_SOCKETADDR_H
#include "ftp_socketaddr.h"
#endif
#include "ftp_extern_pragmas.h"
#ifndef _FTP_SOCKETTYPE_H
#include "ftp_socket.h"
#endif
#ifdef _DEBUG
#include <clib/debug_protos.h>
#endif
#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>

extern ULONG callerid;
#endif


int accept(int i,struct sockaddr *sap,LONG *ip, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return (int)amitcp_Accept((LONG)i, sap, ip);
    }
    else
    {
	return as225_accept(i, sap, (int *)ip);
    }
}

int bind(int i1,struct sockaddr *sap,int i2, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Bind(i1, sap, i2);
    }
    else
    {
	return as225_bind(i1, sap, i2);
    }
}

void cleanup_sockets(struct Library *SocketBase)
{
    if (og.og_socketlib == INETSOCK)
    {
	 as225_cleanup_sockets();
    }
}

int connect(int i1,struct sockaddr *sap,int i2, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Connect(i1, sap, i2);
    }
    else
    {
	return as225_connect(i1, sap, i2);
    }
}

struct hostent *gethostbyname(char *cp, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_GetHostByName(cp);
    }
    else
    {
	return as225_gethostbyname(cp);
    }
}

int gethostname(char *cp,int i, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_GetHostName(cp, i);
    }
    else
    {
	return as225_gethostname(cp, i);
    }
}

struct servent *getservbyname(char *cp1,char *cp2, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_GetServByName(cp1, cp2);
    }
    else
    {
	return as225_getservbyname(cp1, cp2);
    }
}

struct servent *getservbyport(int i,char *cp, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_GetServByPort(i, cp);
    }
    else
    {
	return as225_getservbyport(i, cp);
    }
}

int getsockname(int i,struct sockaddr *sap,LONG *ip, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_GetSockName(i, (struct sockaddr *)sap, ip);
    }
    else
    {
	return as225_getsockname(i, sap, (int *)ip);
    }
}

u_long inet_addr(char *cp, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Inet_Addr(cp);
    }
    else
    {
	return as225_inet_addr(cp);
    }
}

int listen(int i1,int i2, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Listen(i1, i2);
    }
    else
    {
	return as225_listen(i1, i2);
    }
}

int recv(int i1,char *cp,int i2,int i3, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Recv(i1, cp, i2, i3);
    }
    else
    {
	return as225_recv(i1, cp, i2, i3);
    }
}

int s_close(int i, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_CloseSocket(i);
    }
    else
    {
	return as225_s_close(i);
    }
}

int selectwait(int i,fd_set *fd1,fd_set *fd2,fd_set *fd3,struct timeval *tvp,ULONG *lp, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_WaitSelect(i, fd1, fd2, fd3, tvp, lp);
    }
    else
    {
	return as225_selectwait(i, fd1, fd2, fd3, tvp, (long *)lp);
    }
}

int send(int i1,char *cp,int i2,int i3, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Send(i1, cp, i2, i3);
    }
    else
    {
	return as225_send(i1, cp, i2, i3);
    }
}

setsockopt(int i1,int i2,int i3,char *cp,int i4, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_SetSockOpt(i1, i2, i3, cp, i4);
    }
    else
    {
	return as225_setsockopt(i1, i2, i3, cp, i4);
    }
}

ULONG setup_sockets(UWORD uw,int *ip, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	amitcp_SetErrnoPtr(ip, sizeof(*ip));
	return TRUE;
    }
    else
    {
	return as225_setup_sockets(uw, ip);
    }
}

int shutdown(int i1,int i2, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Shutdown(i1, i2);
    }
    else
    {
	return as225_shutdown(i1, i2);
    }
}

int socket(int i1,int i2,int i3, struct Library *SocketBase)
{
    if (og.og_socketlib == AMITCPSOCK)
    {
	return amitcp_Socket(i1, i2, i3);
    }
    else
    {
	return as225_socket(i1, i2, i3);
    }
}

