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

#ifndef _GALILEOFM_REQUESTER_H
#define _GALILEOFM_REQUESTER_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif

#ifndef REF_CALLBACK
#define REF_CALLBACK(name)	void __asm (*name)(register __d0 ULONG,register __a0 APTR,register __a1 APTR)
#endif

#define SRF_LONGINT		(1<<0)	// Integer gadget
#define SRF_CENTJUST		(1<<1)	// Center justify
#define SRF_RIGHTJUST		(1<<2)	// Right justify
#define SRF_SCREEN_PARENT	(1<<5)	// Parent is a screen
#define SRF_IPC			(1<<6)	// Listen to IPC port
#define SRF_SIGNAL		(1<<7)	// Wait for a signal
#define SRF_HISTORY		(1<<8)	// History supplied
#define SRF_PATH_FILTER		(1<<9)	// Filter path characters
#define SRF_BUFFER		(1<<10)	// Buffer supplied (not really used)
#define SRF_CHECKMARK		(1<<11)	// Checkmark supplied
#define SRF_SECURE		(1<<12)	// Secure field
#define SRF_MOUSE_POS		(1<<13)	// Position over mouse pointer
#define SRF_FILEREQ		(1<<14)	// File requester supplied
#define SRF_BUFFER2		(1<<15)	// Second string buffer

#define AR_Requester	TAG_USER + 1		// Pointer to requester

#define AR_Window	TAG_USER + 2		// Window
#define AR_Screen	TAG_USER + 3		// Screen
#define AR_Message	TAG_USER + 4		// Text message
#define AR_Button	TAG_USER + 5		// Button label
#define AR_ButtonCode	TAG_USER + 6		// Code for this button
#define AR_Title	TAG_USER + 7		// Title string
#define AR_Buffer	TAG_USER + 8		// String buffer
#define AR_BufLen	TAG_USER + 9		// Buffer length
#define AR_History	TAG_USER + 10		// History list
#define AR_CheckMark	TAG_USER + 11		// Check mark text
#define AR_CheckPtr	TAG_USER + 12		// Check mark data storage
#define AR_Flags	TAG_USER + 13		// Flags
#define AR_Buffer2	TAG_USER + 14		// String buffer 2
#define AR_BufLen2	TAG_USER + 15		// Buffer length 2

enum
{
    REQTYPE_FILE,
    REQTYPE_SIMPLE,
};

long AsyncRequest(IPCData *,long,struct Window *,REF_CALLBACK(),APTR,struct TagItem *);

#endif

