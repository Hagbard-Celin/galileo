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

#ifndef AD_SOCKPROTO_H
#define AD_SOCKPROTO_H

/* AD uses this file to define macros from external global galileoftp_globals structute og*/

#include "ftp_ad_internet.h"
#include "ftp_extern_pragmas.h"

/* MACROS */

#define accept(i,sap,ip)	(og.og_socketlib == AMITCPSOCK ? amitcp_Accept( i, sap, ip ) \
				: as225_accept( i, sap, (int *)ip ))
#define bind(i1,sap,i2)		(og.og_socketlib == AMITCPSOCK ? amitcp_Bind( i1, sap, i2 ) \
				: as225_bind( i1, sap, i2 ))
#define cleanup_sockets()	(og.og_socketlib == AMITCPSOCK ? 0 : as225_cleanup_sockets())

#define connect(i1,sap,i2)	(og.og_socketlib == AMITCPSOCK ? amitcp_Connect( i1, sap, i2 ) \
				: as225_connect( i1, sap, i2 ))
#define gethostbyname(cp)	(og.og_socketlib == AMITCPSOCK ? amitcp_GetHostByName( cp ) \
				: as225_gethostbyname( cp ))
#define gethostname(cp,i)	(og.og_socketlib == AMITCPSOCK ? amitcp_GetHostName( cp, i ) \
				: as225_gethostname( cp, i ))
#define getservbyname(cp1,cp2)	(og.og_socketlib == AMITCPSOCK ? amitcp_GetServByName( cp1, cp2 ) \
				: as225_getservbyname( cp1, cp2 ))
#define getservbyport(cp1,cp2)	(og.og_socketlib == AMITCPSOCK ? amitcp_GetServByPort( cp1, cp2 ) \
				: as225_getservbyport( cp1, cp2 ))
#define getsockname(i,sap,ip)	(og.og_socketlib == AMITCPSOCK ? amitcp_GetSockName( i, sap, ip ) \
				: as225_getsockname( i, sap, (int *)ip ))
#define inet_addr(cp)		(og.og_socketlib == AMITCPSOCK ? amitcp_Inet_Addr( cp ) \
				: as225_inet_addr( cp ))
#define listen(i1,i2)		(og.og_socketlib == AMITCPSOCK ? amitcp_Listen( i1, i2 ) \
				: as225_listen( i1, i2 ))
#define recv(i1,cp,i2,i3)	(og.og_socketlib == AMITCPSOCK ? amitcp_Recv( i1, cp, i2, i3 ) \
				: as225_recv( i1, cp, i2, i3 ))
#define s_close(i)		(og.og_socketlib == AMITCPSOCK ? amitcp_CloseSocket( i ) \
				: as225_s_close( i ))
#define selectwait(i,fd1,fd2,fd3,tvp,lp)	(og.og_socketlib == AMITCPSOCK ? amitcp_WaitSelect( i, fd1, fd2, fd3, tvp, lp ) \
						: as225_selectwait( i, fd1, fd2, fd3, tvp, (long *)lp ))
#define send(i1,cp,i2,i3)	(og.og_socketlib == AMITCPSOCK ? amitcp_Send( i1, cp, i2, i3 ) \
				: as225_send( i1, cp, i2, i3 ))
#define setup_sockets(uw,ip)	((og.og_socketlib == AMITCPSOCK) ? (amitcp_SetErrnoPtr( ip, sizeof( *ip ) ), TRUE) \
				: (as225_setup_sockets( uw, ip )))
#define shutdown(i1,i2)		(og.og_socketlib == AMITCPSOCK ? amitcp_Shutdown( i1, i2 ) \
				: as225_shutdown( i1, i2 ))
#define socket(i1,i2,i3)	(og.og_socketlib == AMITCPSOCK ? amitcp_Socket( i1, i2, i3 ) \
				: as225_socket( i1, i2, i3 ))

#endif
