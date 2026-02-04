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

#include <gfm/cfg_flags.h>
#include "galileofmlib.h"

// Parse a date/time string into separate date and time buffers
char *__asm __saveds L_ParseDateStrings(
	register __a0 char *string,
	register __a1 char *date_buffer,
	register __a2 char *time_buffer,
	register __a3 long *range)
{
	char *date_ptr=0,*time_ptr=0,*end_ptr;
	char work_buffer[80],cur_date_buf[20],cur_time_buf[20];
	struct DateTime datetime;
	long str_pos,str_len,temp,time_first=0;

	// Clear buffers and range code
	date_buffer[0]=0;
	time_buffer[0]=0;
	if (range) *range=0;

	// Copy string and get length
	strcpy(work_buffer,string);
	str_len=strlen(work_buffer);

	// Strip leading spaces
	for (str_pos=0;str_pos<str_len && isspace(work_buffer[str_pos]);str_pos++);
	if (str_pos==str_len) goto getout;

	// After a date
	if (range && work_buffer[str_pos]=='>')
	{
		++str_pos;
		*range=RANGE_AFTER;
		goto getout;
	}

	// Get pointer to start of date string
	date_ptr=&work_buffer[str_pos];

	// Find first space; null-terminate there
	for (++str_pos;str_pos<str_len;str_pos++)
	{
		if (isspace(work_buffer[str_pos]))
		{
			work_buffer[str_pos]=0;
			break;
		}

		// If we find a colon, time must be listed first
		if (work_buffer[str_pos]==':') time_first=1;
	}
	if (str_pos>=str_len) goto getout;

	// Strip leading spaces
	for (++str_pos;str_pos<str_len && isspace(work_buffer[str_pos]);str_pos++);
	if (str_pos>=str_len) goto getout;

	// Between two dates
	if (range && work_buffer[str_pos]=='>')
	{
		++str_pos;
		*range=RANGE_BETWEEN;
		goto getout;
	}

	// Get pointer to start of time string
	time_ptr=&work_buffer[str_pos];

	// Find first space; null-terminate there
	for (++str_pos;str_pos<str_len;str_pos++)
	{
		if (isspace(work_buffer[str_pos]))
		{
			work_buffer[str_pos]=0;
			break;
		}
	}
	if (str_pos==str_len) goto getout;

	// Scan for range indicator
	if (range)
	{
		// Store old position
		temp=str_pos;

		for (++str_pos;str_pos<str_len;str_pos++)
		{
			if (work_buffer[str_pos]=='>')
			{
				temp=str_pos+1;
				*range=RANGE_BETWEEN;
				break;
			}
		}

		// Restore position
		str_pos=temp;
	}

getout:

	// Get pointer to end of string we deal with
	end_ptr=&string[str_pos];

	// Swap date/time pointers?
	if (time_first)
	{
		char *temp;

		temp=date_ptr;
		date_ptr=time_ptr;
		time_ptr=temp;
	}

	// Get current date
	DateStamp(&datetime.dat_Stamp);

	// Initialise DateTime structure
	datetime.dat_Format=FORMAT_DOS;
	datetime.dat_StrDay=0;
	datetime.dat_StrDate=cur_date_buf;
	datetime.dat_StrTime=cur_time_buf;
	datetime.dat_Flags=0;

	// Convert to string
	DateToStr(&datetime);

	// Fucknose
	temp=0;

	// Valid date string supplied?
	if (date_ptr && (str_pos=strlen(date_ptr))<12 && str_pos>4)
		strcpy(date_buffer,date_ptr);

	// Otherwise
	else
	{
		// Range is not after?
		if (range && (*range)!=RANGE_AFTER)
		{
			temp=1;

			// Get current date
			strcpy(date_buffer,cur_date_buf);
		}

		// Otherwise, default date 1/1/78
		else
		{
			strcpy(date_buffer,"1-Jan-78");
		}
	}

	// Valid time supplied?
	if (time_ptr && (str_pos=strlen(time_ptr))<9 && str_pos>2)
	{
		strcpy(time_buffer,time_ptr);
		if (range && !(*range)) *range=RANGE_WEIRD;
	}

	// Nope
	else
	{
		// Current time?
		if (temp) strcpy(time_buffer,cur_time_buf);

		// Default to midnight
		else strcpy(time_buffer,"00:00:00");
	}

	// Return pointer to end of date/time string
	return end_ptr;
}

// Convert a string to a datestamp
BOOL __asm __saveds L_DateFromStrings(
	register __a0 char *date,
	register __a1 char *time,
	register __a2 struct DateStamp *ds,
	register __d0 ULONG method)
{
	struct DateTime datetime;
	char temptime[12],*ptr;
	BOOL ret;

	// Initialise DateTime structure
	datetime.dat_Format=method;
	datetime.dat_Flags=DTF_FUTURE;
	datetime.dat_StrDay=0;
	datetime.dat_StrDate=date;
	datetime.dat_StrTime=temptime;

	// Copy time
	strcpy(temptime,time);

	// If time is in 12 hour format, fix it
	if ((ptr=strchr(temptime,'a')) ||
		(ptr=strchr(temptime,'p')))
	{
		short hour;

		// Get hour
		hour=atoi(temptime);

		// Fix am
		if (*ptr=='a')
		{
			if (hour==12) hour=0;
		}

		// Or pm
		else
		{
			if (hour<12) hour+=12;
		}

		// Rebuild time string
		lsprintf(temptime,"%ld%s",hour,strchr(time,':'));
	}

	// Do conversion
	ret=StrToDate(&datetime);

	// Copy stamp
	*ds=datetime.dat_Stamp;
	return ret;
}

// Convert a DateStamp to two strings
void __asm __saveds L_DateToStrings(register __a0 struct DateStamp *date,
				    register __a1 char *date_buf,
				    register __a2 char *time_buf,
				    register __d0 int flags)
{
	struct DateTime datetime;

	// Initialise DateTime structure
	datetime.dat_Stamp=*date;
	datetime.dat_Format=gfmlib_data.date_format;
	datetime.dat_Flags=0;
	datetime.dat_StrDay=0;
	datetime.dat_StrDate=date_buf;
	datetime.dat_StrTime=0;

	// Sub-strings ok?
	if (flags==-1 || (flags==1 && gfmlib_data.locale_flags&DATE_SUBST))
		datetime.dat_Flags|=DTF_SUBST;

	// Convert date to a string
	DateToStr(&datetime);

	// No time buffer?
	if (!time_buf) return;

	// Build time string. 12 hour clock?
	if (flags>0 && gfmlib_data.locale_flags&DATE_12HOUR)
	{
		int hours;
		char ampm='a';

		// Get hours, convert to 12 hour clock
		hours=datetime.dat_Stamp.ds_Minute/60;
		if (hours>11)
		{
			ampm='p';
			hours-=12;
		}
		if (hours==0) hours=12;

		// Build time string
		lsprintf(time_buf,
			"%2ld:%02ld:%02ld%lc",
			hours,
			datetime.dat_Stamp.ds_Minute%60,
			datetime.dat_Stamp.ds_Tick/TICKS_PER_SECOND,
			ampm);
	}

	// 24 hour clock
	else lsprintf(time_buf,
		"%02ld:%02ld:%02ld",
		datetime.dat_Stamp.ds_Minute/60,
		datetime.dat_Stamp.ds_Minute%60,
		datetime.dat_Stamp.ds_Tick/TICKS_PER_SECOND);
}


// Get locale flags from the library
UWORD __asm __saveds L_GetLocaleSettings(register __d0 UBYTE type)
{
    // Get flags or dateformat
    if (!type)
	return gfmlib_data.locale_flags;
    else
	return gfmlib_data.date_format;
}


// Set locale flags in the library
void __asm __saveds L_SetLocaleFlags(register __d0 UWORD flags,
				     register __d1 UWORD dateformat)
{
	// Set flags and dateformat
	gfmlib_data.date_format = dateformat;
	gfmlib_data.locale_flags = flags;
}

