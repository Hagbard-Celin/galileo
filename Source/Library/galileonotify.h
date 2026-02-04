/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#ifndef _GALILEOFM_GALILEONOTIFY_H
#define _GALILEOFM_GALILEONOTIFY_H

// Notification message
typedef struct
{
    struct Message	    gn_Msg;
    ULONG		    gn_Type;
    ULONG		    gn_UserData;
    ULONG		    gn_Data;
    ULONG		    gn_Flags;
    struct FileInfoBlock    *gn_Fib;
    char		    gn_Name[1];
} GalileoNotify;

#define NT_GALILEOFM_NOTIFY	199
#define GN_WRITE_ICON		(1<<0)		// Icon written
#define GN_APP_ICON_LIST	(1<<1)		// AppIcon added/removed
#define GN_APP_MENU_LIST	(1<<2)		// AppMenu added/removed
#define GN_CLOSE_WORKBENCH	(1<<3)		// Workbench closed
#define GN_OPEN_WORKBENCH	(1<<4)		// Workbench opened
#define GN_RESET_WORKBENCH	(1<<5)		// Workbench reset
#define GN_DISKCHANGE		(1<<6)		// Disk inserted/removed
#define GN_GALILEOFM_QUIT	(1<<7)		// Main program quit
#define GN_GALILEOFM_HIDE	(1<<8)		// Main program hide
#define GN_GALILEOFM_SHOW	(1<<9)		// Main program show
#define GN_GALILEOFM_START	(1<<10)		// Main program start
#define GN_DOS_ACTION		(1<<11)		// DOS action
#define GN_REXX_UP		(1<<12)		// REXX started
#define GN_FLUSH_MEM		(1<<13)		// Flush memory
#define GN_APP_WINDOW_LIST	(1<<14)		// AppWindow added/removed

#define GNF_ICON_REMOVED	(1<<0)		// Icon removed
#define GNF_WINDOW_REMOVED	(1<<0)		// AppWindow removed
#define GNF_ICON_CHANGED	(1<<1)		// Icon changed
#define GNF_ICON_IMAGE_CHANGED	(1<<2)		// Image was changed

#define GNF_DOS_CREATEDIR	(1<<0)		// CreateDir
#define GNF_DOS_DELETEFILE	(1<<1)		// DeleteFile
#define GNF_DOS_SETFILEDATE	(1<<2)		// SetFileDate
#define GNF_DOS_SETCOMMENT	(1<<3)		// SetComment
#define GNF_DOS_SETPROTECTION	(1<<4)		// SetProtection
#define GNF_DOS_RENAME		(1<<5)		// Rename
#define GNF_DOS_CREATE		(1<<6)		// Open file (create)
#define GNF_DOS_CLOSE		(1<<7)		// Close file
#define GNF_DOS_RELABEL		(1<<8)		// Relabel disk

APTR AddNotifyRequest(ULONG,ULONG,struct MsgPort *);
void RemoveNotifyRequest(APTR);

#endif

