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

enum
{
	ARG_NAME,
	ARG_DESKTOP,
	ARG_LISTER,
	ARG_REQ,
	ARG_TILE,
	ARG_CENTER,
	ARG_STRETCH,
	ARG_PRECISION,
	ARG_BORDER,
};

extern char *precision_key[];

// SETASBACKGROUND internal function
GALILEOFM_FUNC(function_setbackground)
{
	FunctionEntry *entry;
	short which=PATTERN_MAIN;
	USHORT flags;
	ULONG colour;

	// Can only do this under v39
	if (IntuitionBase->LibNode.lib_Version<39)
		return 0;

	// Get first entry
	if (!(entry=function_get_entry(handle)))
		return 0;

	// Build full name
	function_build_source(handle,entry,handle->work_buffer);

	// Arguments?
	if (instruction->funcargs)
	{
		// Which type?
		if (instruction->funcargs->FA_Arguments[ARG_DESKTOP])
			which=PATTERN_MAIN;
		else
		if (instruction->funcargs->FA_Arguments[ARG_LISTER])
			which=PATTERN_LISTER;
		else
		if (instruction->funcargs->FA_Arguments[ARG_REQ])
			which=PATTERN_REQ;
	}

	// Get flags and colour
	flags=environment->env->env_BackgroundFlags[which];
	colour=environment->env->env_BackgroundBorderColour[which];

	// Arguments?
	if (instruction->funcargs)
	{
		// Tile?
		if (instruction->funcargs->FA_Arguments[ARG_TILE])
			flags&=~(ENVBF_CENTER_PIC|ENVBF_STRETCH_PIC);
		else

		// Stretch?
		if (instruction->funcargs->FA_Arguments[ARG_STRETCH] && which==PATTERN_MAIN)
		{
			flags&=~ENVBF_CENTER_PIC;
			flags|=ENVBF_STRETCH_PIC;
		}
		else

		// Center
		if (instruction->funcargs->FA_Arguments[ARG_CENTER])
		{
			flags&=~ENVBF_STRETCH_PIC;
			flags|=ENVBF_CENTER_PIC;
		}

		// Precision?
		if (instruction->funcargs->FA_Arguments[ARG_PRECISION])
		{
			short a;

			// Find keyword match
			for (a=0;precision_key[a];a++)
			{
				if (stricmp((char *)instruction->funcargs->FA_Arguments[ARG_PRECISION],precision_key[a])==0)
				{
					// Clear precision flags
					flags&=~(ENVBF_PRECISION_NONE|ENVBF_PRECISION_GUI|ENVBF_PRECISION_ICON|ENVBF_PRECISION_IMAGE|ENVBF_PRECISION_EXACT);

					// Set appropriate flag
					flags|=1<<(a+1);
					break;
				}
			}
		}

		// Border?
		if (instruction->funcargs->FA_Arguments[ARG_BORDER])
		{
			// Off?
			if (stricmp((char *)instruction->funcargs->FA_Arguments[ARG_BORDER],"off")==0)
				flags&=~ENVBF_USE_COLOUR;
			else
			{
				flags|=ENVBF_USE_COLOUR;
				colour=Atoh((char *)instruction->funcargs->FA_Arguments[ARG_BORDER],-1)<<8;
			}
		}
	}

	// Enable pattern if it's disabled
	if (environment->env->display_options&DISPOPTF_NO_BACKDROP)
	{
		// Ask user
		if (!(function_request(
				handle,
				GetString(&locale,MSG_PATTERN_OFF),
				0,
				GetString(&locale,MSG_OKAY),
				GetString(&locale,MSG_CANCEL),0)))
			return 0;

		// Enable it
		environment->env->display_options&=~DISPOPTF_NO_BACKDROP;
	}

	// Switch to using Galileo picture definition
	environment->env->display_options&=~DISPOPTF_USE_WBPATTERN;

	// Store picture in appropriate pattern
	stccpy(environment->env->env_BackgroundPic[which],handle->work_buffer,sizeof(environment->env->env_BackgroundPic[which])-1);

	// Set flags and colour
	environment->env->env_BackgroundFlags[which]=flags;
	environment->env->env_BackgroundBorderColour[which]=colour;

	// Launch process to read pattern
	misc_startup(NAME_PATTERNS,CMD_REMAP_PATTERNS,0,0,TRUE);
	return 1;
}
