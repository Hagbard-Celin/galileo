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
#include <gfm/cfg_flags.h>
#include "lsprintf_protos.h"

// Turn a DateStamp into a string
void date_build_string(struct DateStamp *date,char *buffer,int pad)
{
	char time_buf[LEN_DATSTRING],date_buf[LEN_DATSTRING],buf[40];

	// Get date and time strings
	DateToStrings(date,date_buf,time_buf,1);

	// Build date and time string
	lsprintf(buf,"%-9s %s",date_buf,time_buf);
	buf[22]=0;

	// If padding selected, pad buffer with spaces at the start
	buffer[0]=0;
	if (pad)
	{
		short len=GUI->date_length+9;
		pad=((environment->env->settings.date_flags&DATE_12HOUR)?len+1:len)-strlen(buf);
		if (pad>0) stccpy(buffer,str_space_string,pad);
	}

	// Add date string to end of buffer
	strcat(buffer,buf);
}

// Get date as a string
void date_string(long days,char *string,short format,BOOL paren)
{
	struct DateTime dt;
	char buf[20];

	// Fill out datetime
	dt.dat_Stamp.ds_Days=days;
	dt.dat_Stamp.ds_Minute=0;
	dt.dat_Stamp.ds_Tick=0;
	dt.dat_Format=(format==-1)?environment->env->settings.date_format:format;
	dt.dat_Flags=0;
	dt.dat_StrDay=0;
	dt.dat_StrDate=(paren)?buf:string;
	dt.dat_StrTime=0;

	// Convert string
	DateToStr(&dt);

	// Want parentheses?
	if (paren)
	{
		char *ptr,*ptr1;

		// Strip leading spaces
		for (ptr=buf;*ptr==' ';ptr++);

		// Strip trailing spaces
		for (ptr1=ptr+strlen(ptr)-1;ptr1>ptr && *ptr1==' ';--ptr1);
		if (*(ptr1+1)==' ') *(ptr1+1)=0;

		// Build string
		lsprintf(string,"(%s)",ptr);
	}
}


// Get time as a string
void time_string(long minutes,long tick,char *string)
{
	// Build string
	lsprintf(string,"%02ld:%02ld:%02ld",minutes/60,minutes%60,tick/TICKS_PER_SECOND);
}
