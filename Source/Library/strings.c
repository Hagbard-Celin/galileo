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

#include <gfm/strings.h>
#include "galileofmlib.h"

// When one insists on perusing a scheme that is in no way supported
// by neither language or compiler....
// Tried using putreg(), but the global optimizer would not have it
// Stubbornness won, and this was created
void __asm set_d1(register __d1 ULONG value);

__asm __saveds L_StrCombine(register __a0 char *buf,
	register __a1 char *one,
	register __a2 char *two,
	register __d0 int lim)
{
	register int a;

	a=strlen(one); if (a>=lim) a=lim-1;
	strncpy(buf,one,a); buf[a]=0;
	return(L_StrConcat(buf,two,lim));
}

__asm __saveds L_StrConcat(register __a0 char *buf,
	register __a1 char *cat,
	register __d0 int lim)
{
	register int a,b;

	a=strlen(cat); b=strlen(buf);
	--lim;
	if (a+b<lim)
	{
		strncpy(&buf[b],cat,a); buf[b+a]=0;
		return(1);
	}
	if (lim>b) strncpy(&buf[b],cat,lim-b);
	buf[lim]=0;
	return 0;
}


STRPTR __asm __saveds L_JoinString(register __a0 APTR memory, register __a1 STRPTR first, register __a2 STRPTR second, register __a3 STRPTR third, register __d0 ULONG flags)
{
    STRPTR joined;
    ULONG size, len[3] = {0};

    if (!first || !first[0])
	return 0;
    else
    {
	if ((len[0] = strlen(first)) && flags&JSF_FS_ADDPART)
	{
	    if (first[len[0] - 1] != '/' && first[len[0] - 1] != ':')
		flags |= JSF_FS_SLASH;
	}
    }
    if (second)
    {
	if ((len[1] = strlen(second)) && flags&JSF_ST_ADDPART)
	{
	    if (second[len[1] - 1] != '/')
		flags |= JSF_ST_SLASH;
	}
    }
    if (third)
    {
	if ((len[2] = strlen(third)) && flags&JSF_E_ADDPART)
	{
	    if (third[len[2] - 1] != '/')
		flags |= JSF_E_SLASH;
	}
    }

    if ((size = len[0] + len[1] + len[2] + 1) == 1)
	return 0;

    if (flags&JSF_FS_SLASH)
    size++;
    if (flags&JSF_ST_SLASH)
    size++;
    if (flags&JSF_E_SLASH)
    size++;

    if (joined = L_AllocMemH(memory, size))
    {
	strcpy(joined, first);
	if (flags&JSF_FS_SLASH)
	    strcat(joined, "/");
	if (len[1])
	{
	    strcat(joined, second);
	    if (flags&JSF_ST_SLASH)
		strcat(joined, "/");
	}
	if (len[2])
	{
	    strcat(joined, third);
	}
	if (flags&JSF_E_SLASH)
	    strcat(joined, "/");

	size--;
	set_d1(size);
	return joined;
    }

    return 0;
}

// Copy a string
STRPTR __asm __saveds L_CopyString(register __a0 APTR memory, register __a1 STRPTR original)
{
    char *copy = 0;
    ULONG len = 0;

    if (original && (copy = L_AllocMemH(memory, (len = strlen(original))+1)))
	strcpy(copy,original);

    set_d1(len);
    return copy;
}


BOOL __asm __saveds L_GetLastPathComponent(register __a0 STRPTR dest, register __a1 STRPTR path)
{
    STRPTR tmp = 0;
    ULONG len, pos;

    if (!dest || !path || !(len = strlen(path)))
	return FALSE;

    if (path[len - 1] == ':')
    {
	tmp = path;
    }
    else
    {
	pos = len;
	if (path[len - 1] != '/')
	pos++;


	for (len = 0; --pos; len++)
	{
	    if (path[pos - 1] == ':' || path[pos - 1] == '/')
	    {
		tmp = path + pos;
		break;
	    }

	}
    }

    if (tmp)
    {
	len++;
	stccpy(dest, tmp, len);
	return TRUE;
    }

    return FALSE;
}


ULONG __asm __saveds L_Atoh(
	register __a0 unsigned char *buf,
	register __d0 short len)
{
	ULONG value=0,mult;
	short size=0,pos;

	for (pos=0;;pos++)
	{
		if (!buf[pos] || !((buf[pos]>='0' && buf[pos]<='9') || (buf[pos]>='a' && buf[pos]<='f') ||
			(buf[pos]>='A' && buf[pos]<='F'))) break;
		if ((++size)==len) break;
	}
	if (size==0) return 0;
	mult=1;
	for (pos=1;pos<size;pos++) mult<<=4;
	for (pos=0;pos<size;pos++)
	{
		short val=0;
		if (buf[pos]>='0' && buf[pos]<='9') val=buf[pos]-'0';
		else
		if (buf[pos]>='A' && buf[pos]<='F') val=10+(buf[pos]-'A');
		else
		if (buf[pos]>='a' && buf[pos]<='f') val=10+(buf[pos]-'a');
		value+=(val*mult);
		mult>>=4;
	}

	return value;
}
