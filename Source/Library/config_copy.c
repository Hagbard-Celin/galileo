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

#include "galileofmlib.h"
#include "config.h"
#include "/Program/galileo_config.h"


// Copy a button bank
Cfg_ButtonBank *__asm __saveds L_CopyButtonBank(register __a0 Cfg_ButtonBank *orig)
{
	Cfg_ButtonBank *bank;
	Cfg_Button *button;

	// Valid bank?
	if (!orig) return 0;

	// Allocate a new bank
	if (!(bank=L_NewButtonBank(0,0))) return 0;

	// Copy bank details
	bank->window=orig->window;
	strcpy(bank->path,orig->path);
	strcpy(bank->backpic,orig->backpic);

	// Copy buttons
	for (button=(Cfg_Button *)orig->buttons.lh_Head;
		button->node.ln_Succ;
		button=(Cfg_Button *)button->node.ln_Succ)
	{
		Cfg_Button *copy;

		// Copy this button
		if (!(copy=L_CopyButton(button,bank->memory,-1))) break;

		// Add to list
		AddTail(&bank->buttons,&copy->node);
	}

	// Did we copy all the buttons?
	if (button->node.ln_Succ)
	{
		// Failed; free what we did get
		L_CloseButtonBank(bank);
		bank=0;
	}

	// Start menu?
	else
	if (orig->startmenu &&
		(bank->startmenu=L_AllocMemH(bank->memory,sizeof(CFG_STRT))))
		CopyMem((char *)orig->startmenu,(char *)bank->startmenu,sizeof(CFG_STRT));

	return bank;
}


// Copy a button
Cfg_Button *__asm __saveds L_CopyButton(
	register __a0 Cfg_Button *orig,
	register __a1 APTR memory,
	register __d0 short type)
{
	Cfg_Button *button;

	// Valid button?
	if (!orig) return 0;

	// Allocate a new button
	if (button=L_NewButton(memory))
	{
		Cfg_ButtonFunction *func,*newfunc;
		BOOL ok=1;

		// Copy button information
		button->button=orig->button;

		// Fix type
		if (type!=-1)
		{
			if (type==0) button->button.flags&=~BUTNF_GRAPHIC;
			else button->button.flags|=BUTNF_GRAPHIC;
		}

		// Go through button functions
		for (func=(Cfg_ButtonFunction *)orig->function_list.mlh_Head;
			func->node.ln_Succ;
			func=(Cfg_ButtonFunction *)func->node.ln_Succ)
		{
			// Allocate new function
			if (newfunc=L_NewButtonFunction(memory,0))
			{
				// Copy function
				if (!(L_CopyButtonFunction(func,memory,newfunc)))
					ok=0;

				// Add to button function list
				AddTail((struct List *)&button->function_list,&newfunc->node);
			}

			// Fail
			else
			{
				ok=0;
				break;
			}
		}

		// Did we copy all the button functions?
		if (!ok)
		{
			// Failed; free what we did get
			L_FreeButton(button);
			button=0;
		}
	}

	return button;
}


// Copy a filetype
Cfg_Filetype *__asm __saveds L_CopyFiletype(
	register __a0 Cfg_Filetype *orig,
	register __a1 APTR memory)
{
	Cfg_Filetype *type;

	// Valid filetype?
	if (!orig) return 0;

	// Allocate a new filetype
	if (type=L_NewFiletype(memory))
	{
		short a;
		Cfg_Function *function;

		// Copy filetype information
		type->type=orig->type;

		// Copy recognition string and icon path
		type->recognition=L_CopyString(memory,orig->recognition);
		type->icon_path=L_CopyString(memory,orig->icon_path);

		// Copy action strings
		if (orig->actions && type->actions)
			for (a=0;a<16;a++)
				type->actions[a]=L_CopyString(memory,orig->actions[a]);

		// Copy function list
		for (function=(Cfg_Function *)orig->function_list.lh_Head;
			function->node.ln_Succ;
			function=(Cfg_Function *)function->node.ln_Succ)
		{
			Cfg_Function *copy;

			// Copy this function
			if (!(copy=L_CopyFunction(function,memory,0))) break;

			// Add to function list
			AddTail(&type->function_list,&copy->node);
		}

		// Did we copy all the functions?
		if (function->node.ln_Succ)
		{
			// Failed; free what we did get
			L_FreeFiletype(type);
			type=0;
		}
	}

	return type;
}


// Copy a function
Cfg_Function *__asm __saveds L_CopyFunction(
	register __a0 Cfg_Function *orig,
	register __a1 APTR memory,
	register __a2 Cfg_Function *newfunc)
{
	Cfg_Function *function;
	Cfg_Instruction *instruction;

	// Valid function?
	if (!orig) return 0;

	// Function supplied?
	if (newfunc)
    {
    	function=newfunc;
    }

	// Allocate a new function
	else
    {
		if (!(function=L_NewFunction(memory,0)))
			return 0;
    }
	// Copy function information
	function->function=orig->function;

	// Copy instructions
	for (instruction=(Cfg_Instruction *)orig->instructions.mlh_Head;
		instruction->node.mln_Succ;
		instruction=(Cfg_Instruction *)instruction->node.mln_Succ)
	{
		Cfg_Instruction *copy;

		// Allocate a new instruction
		if (!(copy=L_AllocMemH(memory,sizeof(Cfg_Instruction))) ||
			!(copy->string=L_CopyString(memory,instruction->string)))
		{
			L_FreeMemH(copy);
			break;
		}

		// Copy type
		copy->type=instruction->type;

		// Add to instruction list
		AddTail((struct List *)&function->instructions,(struct Node *)copy);
	}

	// Did we copy all the instructions?
	if (instruction->node.mln_Succ)
	{
		// Failed; free what we did get
		if (!newfunc) L_FreeFunction(function);
		function=0;
	}

	return function;
}


// Copy a button function
Cfg_ButtonFunction *__asm __saveds L_CopyButtonFunction(
	register __a0 Cfg_ButtonFunction *func,
	register __a1 APTR memory,
	register __a2 Cfg_ButtonFunction *newfunc)
{
	// Copy name
	newfunc->node.ln_Name=L_CopyString(memory,func->node.ln_Name);

	// Copy label
	newfunc->label=L_CopyString(memory,func->label);

	// Does original function have an image?
	if (func->image)
	{
		// Copy image
		if (!(newfunc->image=L_CopyImage(func->image)) && func->label)

			// Try to open image
			newfunc->image=L_OpenImage(func->label,0);
	}

	// Copy function
	if (!(L_CopyFunction((Cfg_Function *)func,memory,(Cfg_Function *)newfunc)))
		return 0;

	return newfunc;
}
