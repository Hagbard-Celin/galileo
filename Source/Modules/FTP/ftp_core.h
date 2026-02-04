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

#ifndef _FTP_CORE_H
#define _FTP_CORE_H

#define	INDEX_INDEX	(1 << 0)
#define	INDEX_FILES	(1 << 1)
#define	INDEX_BIG	(1 << 2)				// index file exceeds limit
#define	INDEX_FILES_BIG (1 << 3)				// files.bbs exceeds limit

#define	NAME_INDEX "INDEX"
#define	NAME_FBBS  "Files.BBS"

// Internal ftp() flags
#define FTPFLAG_ASYNCH	(1<<0)					// Don't get reply

//	Different FTP sytem types we might eventually recognise
enum
{
	FTP_UNKNOWN,
	FTP_UNIX,
	FTP_AMIGA,
	FTP_WINDOWSNT,						// Use SITE command to enable Unix style ls format
	FTP_WIN32,						// NT or Windows95/98 - works like Unix
	FTP_OS2,						// Different ls options and ls format
	FTP_MACOS,						// ls output has no 'links' field
	FTP_DGUX,						// LIST can't use options but NLST -alF is fine
	FTP_ULTRIX,
	FTP_VAX_VMS,						// Incompatible list format
	FTP_VM_CMS						// Incompatible list format
};

//	FTP flags
enum
{
	FTP_NO_PASV	= 1 << 31,				// Doesn't support or allow 'PASV' command - passive transfers
	FTP_NO_REST	= 1 << 30,				// Doesn't support 'REST' command - restart transer
	FTP_NO_SIZE	= 1 << 29,				// Doesn't support 'SIZE' command - size of file
	FTP_NO_MDTM	= 1 << 28,				// Doesn't support 'MDTM' command - last modified time of file
	FTP_NO_CHMOD	= 1 << 27,				// Doesn't support 'SITE CHMOD' command - set protection bits

	FTP_IS_UNIX	= 1 << 15,				// Operating system is a version of Unix (DG/UX)
	FTP_IS_WUFTPD	= 1 << 14,				// FTP server is wu-ftpd

	FTP_PASSIVE	= 1 << 7,				// Use passive transfers
};

int  ftp       (struct ftp_info *, const char *cmd);

// Internal version of some commands supporting flags
int _ftpa      (struct ftp_info *, unsigned long flags, const char *fmt, ...);
int _getreply  (struct ftp_info *, unsigned long flags, int (*updatefn)(void *,int,char *), void *updateinfo);

int  ftp_cdup  (struct ftp_info *, int (*updatefn)(void *,int,char *), void *updateinfo);
int  ftp_chmod (struct ftp_info *, unsigned int mode, char *name);
int  ftp_cwd   (struct ftp_info *, int (*updatefn)(void *,int,char *), void *updateinfo, char *path);
int  ftp_mdtm  (struct ftp_info *, char *name);
int  ftp_mkd   (struct ftp_info *, char *name);
int  ftp_pwd   (struct ftp_info *);
int  ftp_rename(struct ftp_info *, char *oldname, char *newname);
int  ftp_size  (struct ftp_info *, char *name);

#endif
