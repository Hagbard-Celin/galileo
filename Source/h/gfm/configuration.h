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

#ifndef GALILEOFM_CONFIGURATION_H
#define GALILEOFM_CONFIGURATION_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
//#ifndef _GALILEOFM_CFG_BUTTONBANK_H
//#include <gfm/cfg_buttonbank.h>
//#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef _GALILEOFM_CFG_INSTRUCTION_H
#include <gfm/cfg_instruction.h>
#endif
#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif
#ifndef GALILEOFM_CFG_FILETYPE_H
#include <gfm/cfg_filetype.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif

Cfg_Function *NewFunction(APTR memory,UWORD type);
Cfg_Instruction *NewInstruction(APTR memory,short type,char *string);
Cfg_ButtonFunction *NewButtonFunction(APTR memory,UWORD type);

SaveFunction(APTR,Cfg_Function *);

void FreeFunction(Cfg_Function *function);
void FreeButtonFunction(Cfg_ButtonFunction *function);
Cfg_Function *ReadFunction(APTR iff,APTR memory,struct List *func_list,Cfg_Function *function);
Cfg_Function *FindFunctionType(struct List *list,UWORD type);
void FreeInstruction(Cfg_Instruction *ins);
Cfg_Function *CopyFunction(Cfg_Function *orig,APTR,Cfg_Function *new);
void FreeInstructionList(Cfg_Function *func);
Cfg_Lister *NewLister(char *path);
SaveListerDef(APTR,Cfg_Lister *lister);
void FreeListerDef(Cfg_Lister *lister);
Cfg_Filetype *NewFiletype(APTR memory);
void FreeFiletype(Cfg_Filetype *type);
SaveFiletypeList(Cfg_FiletypeList *list,char *name);
Cfg_Filetype *CopyFiletype(Cfg_Filetype *,APTR);
Cfg_FiletypeList *ReadFiletypes(char *name,APTR memory);
void ConvertStartMenu(Cfg_ButtonBank *);

#endif
