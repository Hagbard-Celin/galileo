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

#include <stddef.h>
#include <dos/doshunks.h>
#include "galileofmlib.h"

#define VERF_IS_EXE	(1<<0)
#define VERF_GOT_HEADER	(1<<1)
#define VERF_FAIL	(1<<2)
#define VERF_MATCH	(1<<3)
#define VERF_IN_HUNK	(1<<4)
#define VERF_IN_REL	(1<<5)
#define VERF_IN_DREL	(1<<6)
#define VERF_ROMTAG	(1<<7)
#define VERF_CHECKREL	(1<<8)
#define VERF_DID_SEEK	(1<<9)
#define VERF_GOT_LIBID	(1<<10)
#define VERF_FIND_END	(1<<11)
#define VERF_DOING_TAIL	(1<<12)
#define VERF_DONE_TAIL	(1<<13)
#define VERF_FULLSCAN	(1<<14)

struct hunkdata
{
    ULONG data;
    ULONG datalen;
};

short version_skip(char *,short *,short);
BOOL version_readstring(WORD *version, WORD *revision, struct DateStamp *date, char *buffer, WORD size, WORD pos);

BOOL __asm __saveds L_GetFileVersion(
	register __a0 char *name,
	register __d0 short *version,
	register __d1 short *revision,
	register __a1 struct DateStamp *date,
	register __a2 APTR progress)
{
	char *buffer, *stringbuf;
	ULONG *buflongs, fileoffset = 0;
	ULONG tailsavepos = 0;
	BPTR file;
	ULONG filesize = 0, readsize;
	ULONG stridesize;
	short size,pos, longpos = 0;
	short match=0;
	ULONG numhunks = 0, firsthunk = 0;
	LONG relocs = -1, currelochunk = -1;
	ULONG libid = 0, libidpos = 0, romtaghunk = 0;
	LONG hunknum = -1, libidhunk = -1;
	struct hunkdata *hunks = 0;
	BOOL ok=0;
	struct TagItem tags[2];
	unsigned short verflags=0;
	ULONG reads = 0;
	short extra = 0;
	char match_string[6];

	// Initialise things
	*version=0;
	*revision=0;
	if (date) date->ds_Days=0;
	readsize = gfmlib_data.getfilever_bufsize;

	// Allocate buffer
	if (!(stringbuf= AllocVec(4624,0))) return 0;
	buffer = stringbuf + 520;
	buflongs = (ULONG *)buffer;

	stridesize = readsize - 32;

	// In case of attempted past-end-of-buffer reads
	*(ULONG *)(stringbuf + 512) = 0;
	*(ULONG *)(stringbuf + 520 + readsize) = 0;

	// Open file
	if (!(file=Open(name,MODE_OLDFILE)))
	{
		FreeVec(stringbuf);
		return 0;
	}

	if ((size=Read(file, buffer, readsize)) == -1)
	    goto ver_abort;

	if (size < readsize)
	{
	    WORD clearsize;

	    if ((clearsize = readsize - size) > 32)
		clearsize = 32;
	    memset(buffer + size, 0, clearsize);
	}

	// Check for exe, 37 comes from the minimum size of
	// a minimal HUNK_HEADER plus a HUNK_CODE with payload of
	// two longs, not necessarily a valid loadble exe but
	// enough to check for a version string
	if (size > 37)
	{
	    if (buflongs[0] == HUNK_HEADER)
	    {
		longpos = 1;

		while (buflongs[longpos])
		{
		    longpos += buflongs[longpos] + 1;
		}
		longpos++;
		if ((numhunks = buflongs[longpos]) && numhunks < 0x7FFFFFFF)
		{
		    if (!(hunks = AllocVec(numhunks * sizeof(struct hunkdata), MEMF_CLEAR)))
			goto ver_abort;
		    longpos++;
		    firsthunk = buflongs[longpos];
		    longpos += numhunks + 1;
		    verflags |= VERF_IS_EXE;
		}
	    }
	}

	// Got progress or not a exe?
	if (progress || (size == readsize && !(verflags&VERF_IS_EXE)))
	{
		D_S(struct FileInfoBlock, info);

		// Get file info	
		if (ExamineFH(file,info))
		{
			// Set file size
			filesize = info->fib_Size;
		}
		else
		    goto ver_abort;

		if (progress)
		{
		    verflags |= VERF_FULLSCAN;
		    if ((LONG)progress == -1)
			progress = 0;
		}

		// Set bar
		if (progress)
		{

		    // Initialise tags
		    tags[0].ti_Tag=PW_FileSize;
		    tags[1].ti_Tag=TAG_END;
		    tags[0].ti_Data = filesize;

		    L_SetProgressWindow(progress,tags);

		    // No file size?
		    if (!tags[0].ti_Data) progress=0;
		}
	}

	// Set tag to update size
	tags[0].ti_Tag=PW_FileDone;
	tags[0].ti_Data=0;

	// Build match string
	match_string[0]='$';
	match_string[1]='V';
	match_string[2]='E';
	match_string[3]='R';
	match_string[4]=':';

	// Read first 4k
	do
	{
		if (!(verflags&VERF_DOING_TAIL))
		{
		    extra = 0;
		    verflags &= ~VERF_DID_SEEK;
		}

		if (size < gfmlib_data.getfilever_bufsize)
		{
		    WORD clearsize;

		    if ((clearsize = gfmlib_data.getfilever_bufsize - size) > 32)
			clearsize = 32;
		    memset(buffer + size, 0, clearsize);
		}

		// Got progress?
		if (progress)
		{
			// Check for abort
			if (L_CheckProgressAbort(progress)) break;

			// Increment total, update display
			tags[0].ti_Data+=size;
			L_SetProgressWindow(progress,tags);
		}

		// Leave last 32 bytes
		if (size > stridesize && !(verflags&VERF_DOING_TAIL))
		{
			extra = size - stridesize;
			size = stridesize;
		}

		if (!(verflags&VERF_IS_EXE))
		{
		    reads++;

		    for (pos=0;pos<size && match<5;pos++)
		    {

			// Check for match
			if (buffer[pos]==match_string[match])
			{
				// Increment match count
				++match;
			}

			// Reset match count
			else match=0;

			if (match == 5)
			{
			    ULONG orgpos = 0;
			    WORD strbufsize;
			    WORD needbuf;
			    WORD stringpos;

			    stringpos = pos + 1;

			    if (size - stringpos < 512 && size == stridesize)
				needbuf = 1;
			    else
				needbuf = 0;

			    if (needbuf)
			    {
				orgpos = Seek(file, 0, OFFSET_CURRENT);
				Seek(file, fileoffset + stringpos, OFFSET_BEGINNING);
				if (!(IoErr()))
				{
				    if ((strbufsize = Read(file,stringbuf,512)) < size - stringpos)
					goto ver_abort;

				    if (strbufsize < 512)
				    {
					WORD clearsize;

					if ((clearsize = 512 - strbufsize) > 32)
					    clearsize = 32;
					memset(stringbuf + strbufsize, 0, clearsize);
				    }
				}
				else
				    goto ver_abort;
			    }
			    else
			    {
				strbufsize = size;

				if (size == stridesize && !(verflags&VERF_DOING_TAIL))
				    strbufsize += extra;
			    }

			    if (strbufsize > 0 || !needbuf)
			    {
				if (ok = version_readstring(version, revision, date,
							    (needbuf)?stringbuf:buffer,
							    strbufsize,
							    (needbuf)?0:stringpos))
				    break;
				else
				    match = 0;
			    }

			    if (orgpos)
			    {
				if (!(verflags&VERF_DID_SEEK) && !(verflags&VERF_DOING_TAIL))
				{
				    orgpos -= extra;
				    verflags |= VERF_DID_SEEK;
				}
				Seek(file, orgpos, OFFSET_BEGINNING);
				if (IoErr())
				    goto ver_abort;
			    }
			}
		    }
		    if (ok || (!(verflags&VERF_FULLSCAN) && gfmlib_data.getfilever_bailsize && reads >= gfmlib_data.getfilever_bailsize))
			break;

		    if (!(verflags&VERF_DONE_TAIL) && !match &&
			size == stridesize &&
			filesize > fileoffset + (stridesize << 1))
		    {
			tailsavepos = Seek(file, 0, OFFSET_CURRENT);
			Seek(file, -stridesize, OFFSET_END);
			if (IoErr())
			    goto ver_abort;
			readsize = stridesize;
			verflags |= VERF_DOING_TAIL;
		    }
		    else
		    if (verflags&VERF_DOING_TAIL)
		    {
			Seek(file, tailsavepos, OFFSET_BEGINNING);
			filesize -= stridesize - 32;
			readsize = gfmlib_data.getfilever_bufsize;
			match = 0;
			verflags &= ~VERF_DOING_TAIL;
			verflags |= VERF_DONE_TAIL;
		    }
		}
		else
		{
		    WORD longsize = size >> 2;

		    for (;longpos<longsize;longpos++)
		    {
			if (verflags&VERF_CHECKREL)
			{
			    ULONG testrel = buflongs[longpos]&0x3FFFFFFF;

			    if (testrel == HUNK_RELOC32)
			    {
				verflags |= VERF_IN_REL;
				verflags &= ~VERF_CHECKREL;
				continue;
			    }
			    else
			    if (testrel == HUNK_DREL32 || testrel == HUNK_RELOC32SHORT)
			    {
				verflags |= VERF_IN_DREL;
				verflags &= ~VERF_CHECKREL;
				continue;
			    }
			    else
			    {
				verflags &= ~(VERF_CHECKREL|VERF_ROMTAG);
				verflags |= VERF_FIND_END;
			    }
			}

			if (verflags&VERF_MATCH && !(verflags&(VERF_ROMTAG|VERF_GOT_LIBID)))
			{
			    if (libidhunk < 0)
			    {
				libidhunk = romtaghunk;
			    }
			    else
			    if (firsthunk)
				libidhunk -= firsthunk;

			    verflags |= VERF_GOT_LIBID;
			}

			if (verflags&VERF_GOT_LIBID && !(verflags&VERF_FAIL) && hunknum >= libidhunk)
			{
			    ULONG orgpos;
			    WORD libidsize;
			    ULONG error;

			    orgpos = Seek(file, 0, OFFSET_CURRENT);
			    Seek(file, hunks[libidhunk].data + libid, OFFSET_BEGINNING);
			    if (error = IoErr())
			    {
				verflags |= VERF_FAIL;
			    }
			    else
			    {
				if ((libidsize = Read(file,stringbuf,512)) > 0)
				{
				    if (libidsize < 512)
				    {
					WORD clearsize;

					if ((clearsize = 512 - libidsize) > 32)
					    clearsize = 32;
					memset(stringbuf + libidsize, 0, clearsize);
				    }

				    if ((ok = version_readstring(NULL, revision, date, stringbuf, libidsize, 0)) == 3)
					break;
				    else
					verflags |= VERF_FAIL;
				}
				else
				    verflags |= VERF_FAIL;
			    }

			    if (!(verflags&VERF_DID_SEEK))
			    {
				orgpos -= extra;
				verflags |= VERF_DID_SEEK;
			    }
			    Seek(file, orgpos, OFFSET_BEGINNING);
			}

			if (verflags&VERF_FIND_END)
			{
			    if (buflongs[longpos] == HUNK_END)
			    {
				if ((buflongs[longpos + 1]&0x3FFFFFFF) == HUNK_CODE ||
				    (buflongs[longpos + 1]&0x3FFFFFFF) == HUNK_DATA)
				{
				    verflags &= ~VERF_FIND_END;
				}
				else
				if ((buflongs[longpos + 1]&0x3FFFFFFF) == HUNK_BSS)
				{
				    hunknum++;
				    longpos++;
				    continue;
				}
			    }
			}
			else
			if (verflags&VERF_IN_REL)
			{
			    if (libidhunk < 0)
			    {
				if (relocs < 0)
				{
				    if (!(relocs = buflongs[longpos]))
				    {
					verflags &= ~VERF_IN_REL;
					if (libidhunk < 0)
					    verflags |= VERF_CHECKREL;
				    }
				}
				else
				if (currelochunk < 0)
				{
				    currelochunk = buflongs[longpos];
				}
				else
				{
				    if (buflongs[longpos] == libidpos)
				    {
					libidhunk = currelochunk;

					verflags &= ~(VERF_IN_REL|VERF_ROMTAG);
					verflags |= VERF_FIND_END;
					continue;
				    }

				    if (!--relocs)
				    {
					relocs--;
					currelochunk = -1;
				    }
				}

			    }
			    else
				verflags &= ~VERF_IN_REL;
			}
			else
			if (verflags&VERF_IN_DREL)
			{
			    if (libidhunk < 0)
			    {
				UWORD a, *wordread = (WORD *)(buflongs + longpos);

				for (a = 0; a < 2; a++)
				{
				    if (relocs < 0)
				    {
					if (!(relocs = wordread[a]))
					    verflags &= ~VERF_IN_REL;
				    }
				    else
				    if (currelochunk < 0)
				    {
					currelochunk = wordread[a];
				    }
				    else
				    {
					if (wordread[a] == libid)
					{
					    libidhunk = currelochunk;

					    verflags &= ~(VERF_IN_DREL|VERF_ROMTAG);
					    verflags |= VERF_FIND_END;
					    break;
					}
					relocs--;
				    }

				    if (!relocs)
				    {
					relocs--;
					currelochunk = -1;
					verflags &= ~VERF_IN_DREL;
					if (libidhunk < 0)
					    verflags |= VERF_CHECKREL;

				    }
				}
			    }
			    else
				verflags &= ~VERF_IN_DREL;
			}
			else
			if (!(verflags&VERF_GOT_HEADER))
			{
			    if ((buflongs[longpos]&0x3FFFFFFF) == HUNK_CODE || (buflongs[longpos]&0x3FFFFFFF) == HUNK_DATA)
				verflags |= VERF_GOT_HEADER;
			}
			else
			if (!(verflags&VERF_IN_HUNK))
			{
			    if (++hunknum >= numhunks)
				goto ver_abort;

			    if (hunks[hunknum].datalen = buflongs[longpos])
			    {
				hunks[hunknum].data = ((ULONG)(longpos + 1) << 2) + fileoffset;
				verflags |= VERF_IN_HUNK;
			    }
			    else
				verflags &= ~VERF_GOT_HEADER;
			}
			else
			{
			    UWORD *matchword = (UWORD *)(buflongs + longpos);
			    UWORD a;
			    struct Resident *romtag;
			    char *matchchar = (char *)matchword;

			    if (!(verflags&VERF_MATCH)/* && longpos <= limit*/)
			    {
				for (a = 0; a < 2; a++)
				{
				    if (matchword[a] == RTC_MATCHWORD &&
					*(ULONG *)(matchword + a + 1) + hunks[hunknum].data == (longpos << 2) + (a << 1) + fileoffset)
				    {
					romtag = (struct Resident *)(matchword + a);
					*version = romtag->rt_Version;
					libid = (ULONG)romtag->rt_IdString;
					libidpos = (longpos << 2) + (a << 1) + offsetof(struct Resident, rt_IdString) + fileoffset - hunks[hunknum].data;
					romtaghunk = hunknum;
					verflags |= VERF_MATCH|VERF_ROMTAG;
				    }
				}
			    }

			    for (a = 0; a < 4 && match<5; a++)
			    {
				// Check for match
				if (matchchar[a] == match_string[match])
				{
				    // Increment match count
				    ++match;

				    if (match == 5)
				    {
					ULONG orgpos = 0;
					WORD strbufsize;
					WORD needbuf;
					WORD stringpos;

					stringpos = (longpos << 2) + a + 1;

					if (size - stringpos < 512 && size == stridesize)
					    needbuf = 1;
					else
					    needbuf = 0;

					if (needbuf)
					{
					    orgpos = Seek(file, 0, OFFSET_CURRENT);
					    Seek(file, stringpos + fileoffset, OFFSET_BEGINNING);
					    if (!(IoErr()))
					    {
						if ((strbufsize = Read(file,stringbuf,512)) < size - stringpos)
						    goto ver_abort;

						if (strbufsize < 512)
						{
						    WORD clearsize;

						    if ((clearsize = 512 - strbufsize) > 32)
							clearsize = 32;
						    memset(stringbuf + strbufsize, 0, clearsize);
						}
					    }
					    else
						goto ver_abort;
					}
					else
					{
					    strbufsize = size;

					    if (size == stridesize)
						strbufsize += extra;
					}

					if (strbufsize > 0 || !needbuf)
					{
					    if ((ok = version_readstring(version, revision, date,
									 (needbuf)?stringbuf:buffer,
									 strbufsize,
									 (needbuf)?0:stringpos)) == 3)
					    {
						break;
					    }
					    else
						match = 0;
					}

					if (orgpos)
					{
					    if (!(verflags&VERF_DID_SEEK))
					    {
						orgpos -= extra;
						verflags |= VERF_DID_SEEK;
					    }
					    Seek(file, orgpos, OFFSET_BEGINNING);
					    if (IoErr())
						goto ver_abort;
					}
				    }
				}

				// Reset match count
				else match=0;
			    }
			    if (ok == 3)
				break;

			    if ((longpos << 2) + fileoffset == hunks[hunknum].data + (hunks[hunknum].datalen - 1 << 2))
			    {
				if (verflags&VERF_ROMTAG && libidhunk < 0)
				    verflags |= VERF_CHECKREL;
				else
				    verflags |= VERF_FIND_END;

				verflags &= ~(VERF_IN_HUNK|VERF_GOT_HEADER);
			    }
			}
		    }

		    if (ok == 3)
			break;
		}

		if (!(verflags&VERF_DOING_TAIL))
		{
		    // Any extra?
		    if (extra>0 && !(verflags&VERF_DID_SEEK))
		    {
			    // Seek back for extra
			    Seek(file,-extra,OFFSET_CURRENT);
		    }
		}

		fileoffset = Seek(file, 0, OFFSET_CURRENT);

		if (verflags&VERF_DONE_TAIL)
		{
		    if (fileoffset >= filesize - 1)
			break;
		    if (fileoffset + readsize >= filesize - 1)
			readsize = filesize - fileoffset - 1;
		}

		longpos = 0;
	}
	while ((size = Read(file, buffer, readsize)) > 0);

	// Got progress?
	if (progress)
	{
		// Show full bar
		tags[0].ti_Data=(ULONG)-1;
		L_SetProgressWindow(progress,tags);
	}

ver_abort:
	if (hunks)
	    FreeVec(hunks);

	// Close file, free buffer
	Close(file);
	FreeVec(stringbuf);

	return ok;
}

static const char *month_str[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

#define VRF_GOT_VER (1<<0)
#define VRF_GOT_BRA (1<<1)

WORD version_readstring(WORD *version, WORD *revision, struct DateStamp *date, char *buffer, WORD size, WORD pos)
{
    struct DateTime dt;
    short day,month,year;
    WORD flags = 0;
    WORD ok;
    WORD dateret;
    char datebuf[20];

    do
    {
	WORD tmppos;

	// Find first number
	while (pos<size && (buffer[pos]<'0' || buffer[pos]>'9'))
	{
	    if (buffer[pos] == '\0' || buffer[pos] == '\n' || buffer[pos] == '\r' ||
		(buffer[pos] == '(' && buffer[pos + 1]>='0' && buffer[pos + 1]<='9'))
		return NULL;

	    ++pos;
	}
	if (pos>=size) return NULL;

	tmppos = pos;

	// Version string or digit in name?
	while (tmppos<size && buffer[tmppos] >= '0' && buffer[tmppos] <= '9')
	{
	    if (buffer[tmppos] == '\0' || buffer[tmppos] == '\n' || buffer[tmppos] == '\r')
		return NULL;

	    ++tmppos;
	}
	if (pos>size) return NULL;

	if (tmppos < size - 1 && buffer[tmppos++]=='.' && buffer[tmppos] >= '0' && buffer[tmppos] <= '9')
	    flags |= VRF_GOT_VER;
	else
	    pos = tmppos;

    }
    while (!(flags&VRF_GOT_VER));

    // Get version number
    if (version)
	*version=atoi(buffer+pos);

    // Find period
    while (pos<size && buffer[pos]!='.')
    {
	// Fail if we find anything but number or period
	if (!(buffer[pos]>='0' && buffer[pos]<='9'))
	    return NULL;
	++pos;
    }
    if (pos>=size-1) return NULL;

    // Check next number
    ++pos;
    if (buffer[pos]>='0' && buffer[pos]<='9')
	    *revision=atoi(buffer+pos);
    else
	return NULL;

    // We're ok at this point
    ok=1;

    if (!date)
	return ok;

    // Find parentheses for date
    while (pos<size && buffer[pos]!='(' && buffer[pos]!=' ')
    {
	if (buffer[pos] == '\0' || buffer[pos] == '\n' || buffer[pos] == '\r')
	    return ok;

	++pos;
    }
    if (pos >= size - 1) return ok;

    if (buffer[pos] == '(')
	flags |= VRF_GOT_BRA;

    ++pos;

    do
    {
	// Find first number
	while (pos<size && (buffer[pos]<'0' || buffer[pos]>'9'))
	{
	    if (buffer[pos] == '\0' || buffer[pos] == '\n' || buffer[pos] == '\r')
		return ok;

	    if (buffer[pos++] =='(')
	    {
		if (flags&VRF_GOT_BRA)
		    return ok;
		else
		    flags |= VRF_GOT_BRA;
	    }

	    // Skip if it's a time string
	    if (buffer[pos + 2] ==':' && buffer[pos + 5] ==':')
		pos += 8;
	}
	if (pos>=size) break;

	// Get day
	day=atoi(buffer+pos);
	if (day<1 || day>31) break;

	// Skip a segment
	if ((dateret=version_skip(buffer,&pos,size)) < 0)
	{
	    if (dateret == -2 && !(flags&VRF_GOT_BRA))
		continue;
	    break;
	}

	// Get month
	month=atoi(buffer+pos);
	if (month<1 || month>12)
	{
	    char buf[4];

	    // Maybe this is a character month
	    buf[0]=buffer[pos];
	    buf[1]=buffer[pos+1];
	    buf[2]=buffer[pos+2];
	    buf[3]=0;

	    // See if it matches
	    for (month=0;month<12;month++)
		if (stricmp(month_str[month],buf)==0) break;

	    // Did it match?
	    if (month<12) ++month;
	    else break;
	}

	// Skip a segment
	if ((dateret=version_skip(buffer,&pos,size)) < 0)
	{
	    if (dateret == -2 && !(flags&VRF_GOT_BRA))
		continue;
	    break;
	}

	// Get year
	year=atoi(buffer+pos);

	// Check year
	if (year>1900 && year<2000) year-=1900;
	if (year >= 2000 && year < 3000) year -= 2000;
	if (year < 78 && year > 45) break;
	while (year>100) year-=100;

	// Build string
	lsprintf(datebuf,"%ld-%ld-%ld",day,month,year);

	// Convert to datestamp
	dt.dat_Format=FORMAT_CDN;
	dt.dat_Flags=0;
	dt.dat_StrDay=0;
	dt.dat_StrDate=datebuf;
	dt.dat_StrTime="00:00:00";
	StrToDate(&dt);

	// Copy datestamp
	*date=dt.dat_Stamp;

	ok = 3;
    }
    while (ok < 3);

    return ok;
}

// Skip a segment
short version_skip(char *buffer,short *posp,short size)
{
	WORD pos = *posp;

	// Find non-segment
	while (pos<size &&
			((buffer[pos]>='0' && buffer[pos]<='9') ||
			 (buffer[pos]>='a' && buffer[pos]<='z') ||
			 (buffer[pos]>='A' && buffer[pos]<='Z'))) ++pos;
	if (pos>=size) return -1;

	// Find segment
	while (pos<size &&
			!((buffer[pos]>='0' && buffer[pos]<='9') ||
			  (buffer[pos]>='a' && buffer[pos]<='z') ||
			  (buffer[pos]>='A' && buffer[pos]<='Z')))
	{
	    if (buffer[pos] =='\0' || buffer[pos] == '\n' || buffer[pos] == '\r')
		return -1;

	    if (buffer[pos++] =='(')
	    {
		*posp = pos;
		return -2;
	    }
	}
	if (pos>=size) return -1;

	*posp = pos;
	return pos;
}
