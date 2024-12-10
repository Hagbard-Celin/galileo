/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2024 Hagbard Celine

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
#include "/Modules/Config/galileoconfig.h"

struct qual_name
{
	unsigned short qual;
	unsigned char sym;
	char *name;
};

static const struct qual_name
	qualifiers[]={
		{IEQUALIFIER_LSHIFT,IXSYM_SHIFT|IXSYM_CAPS,"lshift"},
		{IEQUALIFIER_RSHIFT,IXSYM_SHIFT|IXSYM_CAPS,"rshift"},
		{IEQUALIFIER_CONTROL,0,"control"},
		{IEQUALIFIER_LALT,IXSYM_ALT,"lalt"},
		{IEQUALIFIER_RALT,IXSYM_ALT,"ralt"},
		{IEQUALIFIER_LCOMMAND,0,"lcommand"},
		{IEQUALIFIER_RCOMMAND,0,"rcommand"},
		{IEQUALIFIER_NUMERICPAD,0,"numpad"},
		{0,0,0},

		{0x40,0,"space"},
		{0x41,0,"backspace"},
		{0x42,0,"tab"},
		{0x43,0,"enter"},
		{0x44,0,"return"},
		{0x45,0,"esc"},
		{0x46,0,"del"},
		{0x4c,0,"up"},
		{0x4d,0,"down"},
		{0x4e,0,"right"},
		{0x4f,0,"left"},
		{0x5f,0,"help"},
		{0xff,0,0}};

extern long __asm GetCPUUsage(void);

void __asm __saveds L_BuildKeyString(
	register __d0 unsigned short code,
	register __d1 unsigned short qual,
	register __d2 unsigned short qual_mask,
	register __d3 unsigned short qual_same,
	register __a0 char *buffer)
{
	char *key_ptr=0,key[4];
	short a;

	// Initialise buffer
	buffer[0]=0;

	// Invalid code?
	if (code==0xffff) return;

	// Go through qualifiers we know about
	for (a=0;qualifiers[a].qual;a++)
	{
		// Qualifier not set?
		if (!(qual&qualifiers[a].qual) || (qual_same&qualifiers[a].sym))
		{
			// Qualifier mask set?
			if (!(qual_mask&qualifiers[a].qual))
				strcat(buffer,"-");

			// Otherwise, continue
			else continue;
		}

		// Add to buffer
		strcat(buffer,qualifiers[a].name);
		strcat(buffer," ");
	}

	// Synonyms
	if (qual_same&IXSYM_SHIFT) strcat(buffer,"shift ");
	if (qual_same&IXSYM_CAPS) strcat(buffer,"caps ");
	if (qual_same&IXSYM_ALT) strcat(buffer,"alt ");

	// Function key?
	if (code>=0x50 && code<=0x59)
	{
		lsprintf(key,"f%ld",code-0x4f);
		key_ptr=key;
	}

	// Go through list of codes
	else
	for (a++;qualifiers[a].qual!=0xff;a++)
	{
		// Code we're after?
		if (qualifiers[a].qual==code)
		{
			key_ptr=qualifiers[a].name;
			break;
		}
	}

	// Another key
	if (!key_ptr)
	{
		if (L_ConvertRawKey(code,0,key))
		{
			key_ptr=key;
			key[1]=0;
		}
	}

	// Key to tack on?
	if (key_ptr) strcat(buffer,key_ptr);
}


// Get valid qualifiers
USHORT __asm __saveds L_QualValid(register __d0 USHORT qual)
{
	qual&=	IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|
			IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT|
			IEQUALIFIER_LALT|IEQUALIFIER_RALT|
			IEQUALIFIER_CONTROL|IEQUALIFIER_NUMERICPAD;
	return qual;
}


// Convert a key from the raw key code
BOOL __asm __saveds L_ConvertRawKey(
	register __d0 USHORT code,
	register __d1 USHORT qual,
	register __a0 char *key)
{
	struct Device *ConsoleDevice;
	struct IOStdReq console_req;
	BOOL ret=0;

	// Open console device
	if (!OpenDevice("console.device",-1,(struct IORequest *)&console_req,0))
	{
		struct InputEvent event;

		// Get device base pointer
		ConsoleDevice=console_req.io_Device;

		// Build fake input event
		event.ie_NextEvent=0;
		event.ie_Class=IECLASS_RAWKEY;
		event.ie_SubClass=0;
		event.ie_Code=code;
		event.ie_Qualifier=qual;
		event.ie_EventAddress=0;

		// Convert key
		if ((RawKeyConvert(&event,key,1,0))>0) ret=1;

		// Close device
		CloseDevice((struct IORequest *)&console_req);
	}

	return ret;
}

// Get address of one-shot left mouse-button up/down injector
CxObj *__asm __saveds L_GetCxSelectUpDown(register __a6 struct MyLibrary *libbase)
{
    struct LibData *data;

    // Get data pointer
    data=(struct LibData *)libbase->ml_UserData;
    return data->cx_select_up_down;
}

// Set address of one-shot left mouse-button up/down injector
void __asm __saveds L_SetCxSelectUpDown(register __a1 CxObj * cxo,register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;
	data->cx_select_up_down = cxo;
}

// Write an icon for a file
void __asm __saveds L_WriteFileIcon(
	register __a0 char *source,
	register __a1 char *dest,
	register __a6 struct MyLibrary *libbase)
{
	char *buffer;
	BPTR lock;
	struct DiskObject *icon;

	// Get temporary buffer
	if (!(buffer=AllocVec(strlen(dest)+6,0)))
		return;

	// Tack on .info
	strcpy(buffer,dest);
	strcat(buffer,".info");

	// If icon already exists, don't overwrite
	if (lock=Lock(buffer,ACCESS_READ))
	{
		// Unlock and return
		UnLock(lock);
		FreeVec(buffer);
		return;
	}

	// Free buffer, get icon
	FreeVec(buffer);
	if (!(icon=L_GetCachedDiskObjectNew(source,1,libbase)))
		return;

	// Write icon and free it
	PutDiskObject(dest,icon);
	L_FreeCachedDiskObject(icon,libbase);
}


static const struct TagItem
	busytags[3]={
		{WA_BusyPointer,1},
		{WA_PointerDelay,TRUE},
		{TAG_DONE,0}};

static USHORT __chip
	busydata20[]={
		0x0000,0x0000,
		0x0400,0x07c0,0x0000,0x07c0,0x0100,0x0380,0x0000,0x07e0,
		0x07c0,0x1ff8,0x1ff0,0x3fec,0x3ff8,0x7fde,0x3ff8,0x7fbe,
		0x7ffc,0xff7f,0x7efc,0xffff,0x7ffc,0xffff,0x3ff8,0x7ffe,
		0x3ff8,0x7ffe,0x1ff0,0x3ffc,0x07c0,0x1ff8,0x0000,0x07e0,
		0x0000,0x0000};

void __asm __saveds L_SetBusyPointer(register __a0 struct Window *wind)
{
	if (IntuitionBase->LibNode.lib_Version>38) SetWindowPointerA(wind,busytags);
	else SetPointer(wind,busydata20,16,16,-6,0);
}

void __asm __saveds L_ActivateStrGad(
	register __a0 struct Gadget *gad,
	register __a1 struct Window *win)
{
	if (!(gad->Flags&GFLG_SELECTED) && !(gad->Flags&GFLG_DISABLED))
	{
		struct StringInfo *sinfo;

		sinfo=(struct StringInfo *)gad->SpecialInfo;
		sinfo->BufferPos=strlen(sinfo->Buffer);
		RefreshGList(gad,win,0,1);
		ActivateGadget(gad,win,0);
	}
}

/*
struct IFFHandle *__asm __saveds L_OpenIFFFile(
	register __a0 char *name,
	register __d0 int mode,
	register __d1 ULONG check_id)
{
	return 0;
}


void __asm __saveds L_CloseIFFFile(register __a0 struct IFFHandle *iff)
{
}
*/

unsigned long place_lookup[]={
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000
};

// Convert signed integer to a string
void __asm __saveds L_Itoa(
	register __d0 long num,
	register __a0 char *str,
	register __d1 char sep)
{
	short place,val;
	BOOL first=1;

	// Zero?
	if (num==0)
	{
		*str++='0';
		*str=0;
		return;
	}

	// Negative?
	if (num<0)
	{
		num=-num;
		*str++='-';
	}

	// Go through initially to get length of number
	for (place=9;place>=0;place--)
	{
		// Anything in this place?
		if (num>=place_lookup[place]) break;
	}

	// Go through remaining decimal places
	for (;place>=0;place--)
	{
		// Get value of this place
		val=UDivMod32(num,place_lookup[place]);

		// Need a separator?
		if (first) first=0;
		else if (sep && (place==8 || place==5 || place==2)) *str++=sep;

		// Store number
		*str++=(char)('0'+val);

		// Decrement number
		num-=UMult32(val,place_lookup[place]);
	}

	// Null-terminate string
	*str=0;
}


// Convert unsigned integer to a string
void __asm __saveds L_ItoaU(
	register __d0 unsigned long num,
	register __a0 char *str,
	register __d1 char sep)
{
	short place,val;
	BOOL first=1;

	// Zero?
	if (num==0)
	{
		*str++='0';
		*str=0;
		return;
	}

	// Go through initially to get length of number
	for (place=9;place>=0;place--)
	{
		// Anything in this place?
		if (num>=place_lookup[place]) break;
	}

	// Go through remaining decimal places
	for (;place>=0;place--)
	{
		// Get value of this place
		val=UDivMod32(num,place_lookup[place]);

		// Need a separator?
		if (first) first=0;
		else if (sep && (place==8 || place==5 || place==2)) *str++=sep;

		// Store number
		*str++=(char)('0'+val);

		// Decrement number
		num-=UMult32(val,place_lookup[place]);
	}

	// Null-terminate string
	*str=0;
}


// Return a disk size as a string
void __asm __saveds L_BytesToString(
	register __d0 unsigned long bytes,
	register __a0 char *string,
	register __d1 short places,
	register __d2 char sep)
{
	unsigned long div=0;
	char size_ch='K';

	// Less than a kilobyte?
	if (bytes<1024)
	{
		// Nothing?
		if (bytes<1) strcpy(string,"0K");
		else
		{
			L_ItoaU(bytes,string,sep);
			strcat(string,"b");
		}
		return;
	}

	// Convert to kilobytes
	bytes>>=10;

	// Fucking huge?
	if (bytes>1073741824) strcpy(string,"HUGE");

	// Gigabyte range?
	else
	if (bytes>1048576)
	{
		div=1048576;
		size_ch='G';
	}

	// Megabyte range?
	else if (bytes>4096)
	{
		div=1024;
		size_ch='M';
	}

	// Kilobytes
	else
	{
		L_ItoaU(bytes,string,sep);
		strcat(string,"K");
	}

	// Need to do a division?
	if (div)
	{
		short len;

		// Do division to string
		L_DivideToString(string,bytes,div,places,sep);

		// Tack on character
		len=strlen(string);
		string[len]=size_ch;
		string[len+1]=0;
	}
}


// Do division (fake float) into a string
void __asm __saveds L_DivideToString(
	register __a0 char *string,
	register __d0 unsigned long bytes,
	register __d1 unsigned long div,
	register __d2 short places,
	register __d3 char sep)
{
	unsigned long whole;
	unsigned long remainder;

	// Zero?
	if (div==0)
	{
		string[0]='0';
		string[1]=0;
		return;
	}

	// Do division
	whole=L_DivideU(bytes,div,&remainder,UtilityBase);

	// Get whole number string
	L_ItoaU(whole,string,sep);

	// Want remainder?
	if (places>0)
	{
		char rem_buf[20],form_buf[10];
		unsigned long rem100;

		// Convert to fraction
		rem100=UDivMod32(UMult32(remainder,100),div);

		// Round up
		if (places==1) rem100+=5;

		// Rounded up to next whole number?
		if (rem100>99)
		{
			// Move to next whole number
			rem100-=100;
			++whole;

			// Get whole number string again
			L_ItoaU(whole,string,sep);
		}	

		// Build formatting string
		lsprintf(form_buf,"%%0%ldld",places+1);

		// Convert remainder to a string, chop to desired decimal places
		lsprintf(rem_buf,form_buf,rem100);
		rem_buf[places]=0;

		// Not zero?
		if (atoi(rem_buf)!=0)
		{
			char *ptr=string+strlen(string);
			lsprintf(ptr,"%lc%s",decimal_point,rem_buf);
		}
	}
}


/*
void __asm __saveds L_DivideToString(
	register __a0 char *string,
	register __d0 unsigned long bytes,
	register __d1 unsigned long div,
	register __d2 short places)
{
	char rem_buf[20];
	unsigned long whole;
	unsigned long rem100;
	unsigned long remainder;
	unsigned long fraction,round;

	// Zero?
	if (div==0)
	{
		string[0]='0';
		string[1]=0;
		return;
	}

	// Get fraction and rounding needed
	for (a=0;a<places;a++) fraction*=10;
	round=UDivMod32(fraction,2);

	// Do division
	whole=L_DivideU(bytes,div,&remainder,UtilityBase);

	// Convert to fraction
	remainder=UDivMod32(remainder*fraction*fraction,div);
	rem100=UDivMod32(remainder+round,fraction);

	// Convert to a string, chop to desired decimal places
	lsprintf(rem_buf,"%ld",rem100);
	rem_buf[1]=0;

	// Build string
	if (atoi(rem_buf)==0) lsprintf(string,"%ld",whole);
	else lsprintf(string,"%ld.%s",whole,rem_buf);
}
*/

// Reply or free a message
void __saveds __asm L_ReplyFreeMsg(register __a0 struct Message *msg)
{
	if (msg)
	{
		if (msg->mn_ReplyPort) ReplyMsg(msg);
		else FreeVec(msg);
	}
}


// Get path from a WBArg
BOOL __saveds __asm L_GetWBArgPath(
	register __a0 struct WBArg *arg,
	register __a1 char *buffer,
	register __d0 long size,
	register __a6 struct MyLibrary *libbase)
{
	// Valid argument?
	if (!arg) return 0;

	// Clear buffer
	*buffer=0;

	// Get path
	if (arg->wa_Lock) L_DevNameFromLock(arg->wa_Lock,buffer,size,libbase);

	// Add filename
	if (arg->wa_Name && *arg->wa_Name) AddPart(buffer,arg->wa_Name,size);

	return (BOOL)*buffer;
}


// Find (and lock) a public screen
struct PubScreenNode *__asm __saveds L_FindPubScreen(
	register __a0 struct Screen *screen,
	register __d0 BOOL lock)
{
	struct List *pubscreenlist;
	struct PubScreenNode *node;
	char *name=0;

	// Lock the public screen list
	pubscreenlist=LockPubScreenList();

	// Scan it
	for (node=(struct PubScreenNode *)pubscreenlist->lh_Head;
		node->psn_Node.ln_Succ;
		node=(struct PubScreenNode *)node->psn_Node.ln_Succ)
	{
		// See if this node is what we're looking for
		if (node->psn_Screen==screen)
		{
			// Get name pointer
			name=node->psn_Node.ln_Name;

			// Lock screen?
			if (lock && !(LockPubScreen(name)))
				name=0;
			break;
		}
	}

	// Unlock list
	UnlockPubScreenList();

	// Return node pointer
	return (name)?node:0;
}


// Set flags in the library
ULONG __asm __saveds L_SetLibraryFlags(
	register __d0 ULONG flags,
	register __d1 ULONG mask,
	register __a6 struct MyLibrary *libbase)
{
	ULONG oldflags;
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Save old flags
	oldflags=data->flags;

	// Mask out bits in mask
	data->flags&=~mask;

	// Set new flags
	data->flags|=flags;
	return oldflags;
}


// Get flags in the library
ULONG __asm __saveds L_GetLibraryFlags(
	register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Return flags
	return data->flags;
}


// Compare two ListFormats
ULONG __asm __saveds L_CompareListFormat(
	register __a0 ListFormat *format1,
	register __a1 ListFormat *format2)
{
	ULONG res=0;
	short num;

	// Check format
	for (num=0;num<16;num++)
	{
		// Different?
		if (format1->display_pos[num]!=format2->display_pos[num])
		{
			res|=CONFIG_CHANGE_LIST_FORMAT;
			break;
		}

		// Finished?
		if (format1->display_pos[num]==-1) break;
	}

	// Sort method
	if (format1->sort.sort!=format2->sort.sort ||
		format1->sort.separation!=format2->sort.separation ||
		format1->sort.sort_flags!=format2->sort.sort_flags) res|=CONFIG_CHANGE_SORT;

	// Filters
	if ((format1->flags&(LFORMATF_REJECT_ICONS|LFORMATF_HIDDEN_BIT))!=
		(format2->flags&(LFORMATF_REJECT_ICONS|LFORMATF_HIDDEN_BIT))) res|=CONFIG_CHANGE_FILTERS;

	// Patterns
	if (stricmp(format1->show_pattern,format2->show_pattern)!=0 ||
		stricmp(format1->hide_pattern,format2->hide_pattern)!=0) res|=CONFIG_CHANGE_FILTERS;

	// Gauge
	if ((format1->flags&(LFORMATF_GAUGE))!=
		(format2->flags&(LFORMATF_GAUGE))) res|=CONFIG_CHANGE_LIST_DISPLAY|CONFIG_CHANGE_LIST_FONT;	
	return res;
}


// Set an environment variable permanently
void __asm __saveds L_SetEnv(
	register __a0 char *name,
	register __a1 char *data,
	register __d0 BOOL save)
{
	// Set variable
	if ((SetVar(name,data,-1,GVF_GLOBAL_ONLY)) && save)
	{
		BPTR old,dir;

		// Switch to ENVARC:
		if (dir=Lock("envarc:",ACCESS_READ))
		{
			char *ptr,*start;
			BPTR file;

			// Change CD
			old=CurrentDir(dir);

			// Go through string
			for (ptr=name,start=name;*ptr;ptr++)
			{
				// Directory separator?
				if (*ptr=='/')
				{
					// Null it out temporarily
					*ptr=0;

					// Try to lock or create directory
					if ((dir=Lock(start,ACCESS_READ)) ||
						(dir=CreateDir(start)))
					{
						// CD into directory
						UnLock(CurrentDir(dir));

						// Restore character we destroyed
						*ptr='/';

						// Bump start pointer
						start=ptr+1;
					}

					// Failed
					else break;
				}
			}

			// Open file
			if (file=Open(start,MODE_NEWFILE))
			{
				// Write data
				Write(file,data,strlen(data));
				Close(file);
			}

			// Restore CD
			UnLock(CurrentDir(old));
		}
	}
}


// Set requester hook
void __asm __saveds L_SetReqBackFill(
	register __a0 struct Hook *hook,
	register __a1 struct Screen **screen,
	register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// If we already have a hook, don't install a new one
	if (data->backfill && hook)
		return;

	// Lock the requester hook
	L_GetSemaphore(&data->backfill_lock,SEMF_EXCLUSIVE,0);

	// Install new hook pointer
	data->backfill=hook;
	data->backfill_screen=screen;

	// Unlock requester hook
	L_FreeSemaphore(&data->backfill_lock);
}


// Get requester backfill
struct Hook *__asm __saveds L_LockReqBackFill(
	register __a0 struct Screen *screen,
	register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Lock the requester hook
	L_GetSemaphore(&data->backfill_lock,SEMF_SHARED,0);

	// No hook, or different screen?
	if (!data->backfill || (screen && data->backfill_screen && screen!=*data->backfill_screen))
	{
		// Unlock the requester hook
		L_FreeSemaphore(&data->backfill_lock);
		return 0;
	}

	// Return hook pointer
	return data->backfill;
}


// Release requester backfill
void __asm __saveds L_UnlockReqBackFill(
	register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Unlock the requester hook
	L_FreeSemaphore(&data->backfill_lock);
}


// Case-insensitive strstr() function
char *strstri(char *string,char *substring)
{
	int a,len,sublen;

	// See if substring is longer than main string
	len=(strlen(string)-(sublen=strlen(substring)))+1;
	if (len<1) return 0;

	// Go through string (for the difference in length between the two)
	for (a=0;a<len;a++)
	{
		// See if substring matches at this point
		if (strnicmp(&string[a],substring,sublen)==0)
			return string+a;
	}

	// No match
	return 0;
}


// Statistics
long __asm L_GetStatistics(register __d0 long id,register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;

	// Get data pointer
	data=(struct LibData *)libbase->ml_UserData;

	// Task count?
	if (id==STATID_TASKCOUNT)
		return data->task_count;
	else
	if (id==STATID_CPU_USAGE)
		return GetCPUUsage();

	return 0;
}
