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

// Allocate and parse arguments
FuncArgs *__asm __saveds L_ParseArgs(
	register __a0 char *template,
	register __a1 char *args)
{
	short count,len;
	FuncArgs *funcargs;

	// Valid arguments and template?
	if (!template || !args || !*template || !*args)
		return 0;

	// Count the number of arguments in template
	for (len=0,count=1;template[len];len++)
	{
		// Add one for every comma
		if (template[len]==',') ++count;
	}

	// Get length of argument string
	len=strlen(args)+2;

	// Allocate FuncArgs
	if (!(funcargs=AllocVec(sizeof(FuncArgs)+sizeof(LONG *)*count*2+len,MEMF_CLEAR)) ||
		!(funcargs->FA_RDArgs=AllocDosObject(DOS_RDARGS,0)))
	{
		L_DisposeArgs(funcargs);
		return 0;
	}

	// Initialise pointers
	funcargs->FA_ArgArray=(LONG *)(funcargs+1);
	funcargs->FA_Arguments=funcargs->FA_ArgArray+count;
	funcargs->FA_ArgString=(char *)(funcargs->FA_Arguments+count);

	// Copy arg string and add a newline
	strcpy(funcargs->FA_ArgString,args);
	strcat(funcargs->FA_ArgString,"\n");

	// Initialise RDArgs
	funcargs->FA_RDArgs->RDA_Source.CS_Buffer=funcargs->FA_ArgString;
	funcargs->FA_RDArgs->RDA_Source.CS_Length=strlen(funcargs->FA_ArgString);

	// Call RDArgs
	if (!(ReadArgs(template,funcargs->FA_ArgArray,funcargs->FA_RDArgs)))
	{
		L_DisposeArgs(funcargs);
		return 0;
	}

	// Store argument count
	funcargs->FA_Count=count;

	// Copy argument pointers
	for (len=0;len<count;len++)
		funcargs->FA_Arguments[len]=funcargs->FA_ArgArray[len];

	return funcargs;
}


// Free FuncArgs
void __asm __saveds L_DisposeArgs(register __a0 FuncArgs *args)
{
	if (args)
	{
		short arg;

		// Custom arguments?
		if (args->FA_Arguments)
		{
			// Free any custom arguments
			for (arg=0;arg<args->FA_Count;arg++)
			{
				// Got an argument that's different?
				if (args->FA_Arguments[arg] &&
					args->FA_Arguments[arg]!=args->FA_ArgArray[arg])
				{
					// Free it
					FreeVec((APTR)args->FA_Arguments[arg]);
				}
			}
		}

		// Free result of ReadArgs() call
		FreeArgs(args->FA_RDArgs);

		// Free RDArgs structure
		FreeDosObject(DOS_RDARGS,args->FA_RDArgs);

		// Free FuncArgs
		FreeVec(args);
	}
}
