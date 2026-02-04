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

#ifndef _GALILEOFM_BUTTON_PROTOS_H
#define _GALILEOFM_BUTTON_PROTOS_H

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
#ifndef _GALILEOFM_FUNCTYPE_H
#include <gfm/functype.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif

Cfg_Button *CopyButton(Cfg_Button *orig,APTR,short);
Cfg_ButtonFunction *CopyButtonFunction(Cfg_ButtonFunction *,APTR,Cfg_ButtonFunction *);
Cfg_Button *NewButtonWithFunc(APTR memory,char *label,short type);
Cfg_Button *NewButton(APTR memory);
Cfg_ButtonBank *CopyButtonBank(Cfg_ButtonBank *orig);
SaveButton(APTR,Cfg_Button *);
Cfg_Button *ReadButton(APTR iff,APTR memory);
void CloseButtonBank(Cfg_ButtonBank *bank);
void FreeButton(Cfg_Button *button);
void FreeButtonList(struct List *list);
Cfg_ButtonBank *NewButtonBank(BOOL,short);
SaveButtonBank(Cfg_ButtonBank *bank,char *name);
Cfg_ButtonBank *OpenButtonBank(char *name, BPTR *parent_lock);

#endif

