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

#ifndef _GALILEOFM_FUNCTION_COPY_INCLUDE
#define _GALILEOFM_FUNCTION_COPY_INCLUDE

#include "galileofm.h"
#include <gfm/strings_protos.h>
#include <gfm/config_display.h>
#include "function_launch_protos.h"
#include "lister_protos.h"
#include "misc_protos.h"
#include "callback_protos.h"
#include "app_msg_protos.h"
#include "function_protos.h"
#include "icons.h"
#include "lsprintf_protos.h"


#define COPY_INITIAL_BUFFER 8192
#define COPY_MIN_BUFFER		2048
#define COPY_LOW_THRESH		500000
#define COPY_HIGH_THRESH	1000000

enum
{
	COPY_NAME,
	COPY_TO,
	COPY_QUIET,
	COPY_UPDATEC,
	COPY_MOVESAME,
	COPY_NEWER,
};

enum
{
	COPYAS_NAME,
	COPYAS_NEWNAME,
	COPYAS_TO,
	COPYAS_QUIET,
	COPYAS_MOVESAME,
};

enum
{
	ENCRYPT_NAME,
	ENCRYPT_TO,
	ENCRYPT_PASSWORD,
	ENCRYPT_QUIET,
};

enum
{
	CLONE_NAME,
	CLONE_NEWNAME,
	CLONE_QUIET,
};

#define COPY_TOP_LEVEL		(1<<14)
#define ENCRYPT_DECRYPT		(1<<15)
#define COPY_UPDATE_COPY	(1<<16)
#define COPY_NEWER_COPY		(1<<17)
#define COPY_ASSIGN_ICON	(1<<18)

typedef struct
{
	short			valid;
	short			confirm_each;

	union
	{
		struct
		{
			char	new_name_edit[108];
			char	old_name_edit[108];
			BOOL	wild_copy;
		} copy;

		struct
		{
			char	password[24];
			short	decrypt;
		} encrypt;

		struct
		{
			short	hard;
		} makelink;
	} func;
} CopyData;

static void copy_icon_position(FunctionHandle *handle,char *name,struct DiskObject *icon);


static int function_copy_file(
	struct FileInfoBlock *s_info,
	struct FileInfoBlock *d_info,
	FunctionHandle *handle,
	char *,char *,BPTR,BPTR,long *,char *,ULONG);

#endif
