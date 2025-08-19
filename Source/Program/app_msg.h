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

#ifndef _GALILEOFM_APPMSG
#define _GALILEOFM_APPMSG

#include "dirlist.h"

typedef struct _GalileoListerAppMessage {
    struct Message  glam_Message;	/* standard message structure */
    UWORD	    glam_Type;		/* message type */
    ULONG	    glam_UserData;	/* application specific */
    ULONG	    glam_ID;		/* application definable ID */
    LONG	    glam_NumArgs;	/* # of elements in arglist */
    ULONG	    glam_Flags;
    UWORD	    glam_Version;	/* will be >= GLAM_VERSION */
    UWORD	    glam_Class;		/* message class */
    WORD	    glam_MouseX;	/* mouse x position of event */
    WORD	    glam_MouseY;	/* mouse y position of event */
    Point	    glam_DragOffset;
    Lister	    *glam_Lister;
    BPTR	    glam_Lock;		/* a lock descriptor */
    DirEntry 	    *glam_OverEntry;
    struct GLAData  *glam_ArgData;
    struct WBArg    glam_ArgList[0];	/* the arguments themselves */
} GalileoListerAppMessage;


struct ArgArray
{
	struct MinList	aa_List;
	APTR		aa_Memory;
	ULONG		aa_Flags;
	LONG		aa_Count;
};

struct ArgArrayEntry
{
	struct MinNode	aae_Node;
	UWORD		aae_Flags;
	BPTR		aae_Lock;
	char		aae_String[1];
};

#define AAF_ALLOW_DIRS	(1<<0)

#define AAEF_DIR	    (1<<0)
#define AAEF_LINK	    (1<<1)
#define AAEF_FAKE_ICON	    (1<<2)
#define AAEF_DEV	    (1<<3)
#define AAEF_ASSIGN	    (1<<4)
#define AAEF_MULTI_ASSIGN   (1<<5)

#endif

