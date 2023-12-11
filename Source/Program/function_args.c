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

#include "galileofm.h"

#define ARGBUFFER	514

// Allocate and parse arguments
FuncArgs *function_parse_args(char *template,char *args)
{
	short count,len;
	FuncArgs *funcargs;

	// Valid arguments and template?
	if (!template || !args || !template[0] || !args[0])
		return 0;

	// Count the number of arguments in template
	for (len=0,count=1;template[len];len++)
	{
		// Add one for every comma
		if (template[len]==',') ++count;
	}

	// Allocate FuncArgs
	if (!(funcargs=AllocVec(sizeof(FuncArgs)+ARGBUFFER+sizeof(LONG *)*count*2,MEMF_CLEAR)) ||
		!(funcargs->rdargs=AllocDosObject(DOS_RDARGS,0)))
	{
		function_free_args(funcargs);
		return 0;
	}

	// Initialise pointers
	funcargs->arg_string=((char *)funcargs)+sizeof(FuncArgs);
	funcargs->arg_array=(LONG *)(funcargs->arg_string+ARGBUFFER);
	funcargs->arguments=funcargs->arg_array+count;

	// Initialise RDArgs
	funcargs->rdargs->RDA_Source.CS_Buffer=funcargs->arg_string;
	funcargs->rdargs->RDA_Source.CS_Length=strlen(args)+1;

	// Copy arg string and add a newline
	strcpy(funcargs->arg_string,args);
	strcat(funcargs->arg_string,"\n");

	// Call RDArgs
	if (!(funcargs->rdargs_res=ReadArgs(template,funcargs->arg_array,funcargs->rdargs)))
	{
		function_free_args(funcargs);
		return 0;
	}

	// Store argument count
	funcargs->count=count;

	// Copy argument pointers
	for (len=0;len<count;len++)
		funcargs->arguments[len]=funcargs->arg_array[len];

	return funcargs;
}


// Free FuncArgs
void function_free_args(FuncArgs *args)
{
	if (args)
	{
		short arg;

		// Custom arguments?
		if (args->arguments)
		{
			// Free any custom arguments
			for (arg=0;arg<args->count;arg++)
			{
				// Got an argument that's different?
				if (args->arguments[arg] &&
					args->arguments[arg]!=args->arg_array[arg])
				{
					// Free it
					FreeVec((APTR)args->arguments[arg]);
				}
			}
		}

		// Free result of ReadArgs() call
		FreeArgs(args->rdargs_res);

		// Free RDArgs structure
		FreeDosObject(DOS_RDARGS,args->rdargs);

		// Free FuncArgs
		FreeVec(args);
	}
}
