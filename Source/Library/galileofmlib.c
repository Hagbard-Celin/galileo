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

#include "galileofmbase.h"

extern struct Library *GalileoFMBase;

__stdargs __saveds SimpleRequestTags(
	struct Window *window,
	char *title,
	char *buttons,
	char *message,...)
{
	return SimpleRequest(window,title,buttons,message,0,(APTR)(((ULONG *)&message)+1),0,0);
}

__stdargs __saveds SimpleRequestScreenTags(
	struct Screen *screen,
	char *title,
	char *buttons,
	char *message,...)
{
	return SimpleRequest((struct Window *)screen,title,buttons,message,0,(APTR)(((ULONG *)&message)+1),0,SRF_SCREEN_PARENT);
}

APTR __stdargs __saveds OpenProgressWindowTags(Tag tag1,...)
{
	return OpenProgressWindow((struct TagItem *)&tag1);
}

void __stdargs __saveds SetProgressWindowTags(APTR prog,Tag tag1,...)
{
	SetProgressWindow(prog,(struct TagItem *)&tag1);
}


void __stdargs __saveds GetProgressWindowTags(APTR prog,Tag tag1,...)
{
	GetProgressWindow(prog,(struct TagItem *)&tag1);
}


long __stdargs __saveds AsyncRequestTags(
	IPCData *my_ipc,
	long type,
	struct Window *window,
	REF_CALLBACK(callback),
	APTR data,
	Tag tag,...)
{
	return AsyncRequest(my_ipc,type,window,callback,data,(struct TagItem *)&tag);
}


struct Hook *__stdargs __saveds GetEditHookTags(
	ULONG type,
	ULONG flags,
	Tag tag,...)
{
	return GetEditHook(type,flags,(struct TagItem *)&tag);
}


short __stdargs __saveds RenderImageTags(
	struct RastPort *rp,
	APTR image,
	USHORT left,
	USHORT top,
	Tag tag,...)
{
	return RenderImage(rp,image,left,top,(struct TagItem *)&tag);
}


struct RexxMsg *__stdargs __saveds BuildRexxMsgExTags(
	struct MsgPort *port,
	UBYTE *extension,
	UBYTE *host,
	Tag tag,...)
{
	return BuildRexxMsgEx(port,extension,host,(struct TagItem *)&tag);
}
