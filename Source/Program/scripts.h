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

enum
{
	SCRIPT_STARTUP,
	SCRIPT_SHUTDOWN,
	SCRIPT_HIDE,
	SCRIPT_REVEAL,
	SCRIPT_DISKINSERT,
	SCRIPT_DISKREMOVE,
	SCRIPT_DOUBLECLICK,
	SCRIPT_OPEN_LISTER,
	SCRIPT_OPEN_BUTTONS,
	SCRIPT_OPEN_GROUP,
	SCRIPT_CLOSE_LISTER,
	SCRIPT_CLOSE_BUTTONS,
	SCRIPT_CLOSE_GROUP,
	SCRIPT_BAD_DISKINSERT,
	SCRIPT_RIGHT_DOUBLECLICK,
	SCRIPT_MID_DOUBLECLICK,
	SCRIPT_PRESTARTUP,
};

BOOL RunScript(short,char *);
void InitScripts(void);
void FreeScripts(void);
BOOL RunScript_Node(Att_Node *,char *);
BOOL RunScript_Name(char *,char *);
Cfg_ButtonFunction *FindScript_Function(char *);
void InitSoundEvents(BOOL force);

extern Att_List *script_list;

#define SCRIPTF_NO_LOCK		(1<<0)
#define SCRIPTF_NO_SOUND	(1<<1)
#define SCRIPTF_STARTUP		(1<<2)
#define SCRIPTF_SYNC		(1<<3)
