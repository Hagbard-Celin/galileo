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

extern struct Library *LocaleBase;
void __asm LSprintf(register __a3 char *,register __a0 char *,register __a1 APTR);

// printf routine, calls locale if available
unsigned long __asm printf_hook(
	register __a0 struct Hook *hook,
	register __a1 char ch)
{
	char *ptr=(char *)hook->h_Data;
	*ptr++=ch;
	hook->h_Data=(APTR)ptr;
	return 0;
}

void __stdargs __saveds DPrintF(char *buffer,char *string,long data,...)
{
	// Locale library available?
	if (LocaleBase)
	{
		struct Hook hook;

		// Fill out hook
		hook.h_Entry=(unsigned long (*)())printf_hook;
		hook.h_Data=(APTR)buffer;
		FormatString(0,string,(APTR)&data,&hook);
	}

	// Otherwise, use lsprintf()
	else LSprintf(buffer,string,(APTR)&data);
}
