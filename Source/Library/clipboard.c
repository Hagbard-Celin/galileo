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
#include "clipboard.h"

ClipHandle *__asm __saveds L_OpenClipBoard(register __d0 ULONG unit)
{
	ClipHandle *clip;
    
	// Allocate handle
	if (!(clip=AllocVec(sizeof(ClipHandle),MEMF_CLEAR)))
		return 0;

	// Open clipboard
	if (!(clip->clip_Port=CreateMsgPort()) ||
		!(clip->clip_Req=(struct IOClipReq *)
			CreateIORequest(clip->clip_Port,sizeof(struct IOClipReq))) ||
		(OpenDevice("clipboard.device",unit,(struct IORequest *)clip->clip_Req,0)))
	{
		L_CloseClipBoard(clip);
		return 0;
	}

	return clip;
}

void __asm __saveds L_CloseClipBoard(register __a0 ClipHandle *clip)
{
    struct Message *msg;

	if (clip)
	{
		// Got clip request?
		if (clip->clip_Req)
		{
			// Device open?
			if (clip->clip_Req->io_Device)
			{
				// Written to?
				if (clip->clip_Write)
				{
					// Update it
					clip->clip_Req->io_Command=CMD_UPDATE;
					DoIO((struct IORequest *)clip->clip_Req);
				}

				// Close it
				CloseDevice((struct IORequest *)clip->clip_Req);
			}

			// Delete request
			DeleteIORequest((struct IORequest *)clip->clip_Req);
		}

		// Delete port
        while (msg=GetMsg(clip->clip_Port))
			ReplyMsg(msg);
		DeleteMsgPort(clip->clip_Port);

		// Free clip handle
		FreeVec(clip);
	}
}

BOOL __asm __saveds L_WriteClipString(
	register __a0 ClipHandle *clip,
	register __a1 char *string,
	register __d0 long length)
{
	// Valid clip?
	if (clip)
	{
		// Get length if not supplied
		if (length==-1) length=strlen(string);

		// Fill out header
		clip->clip_Header[0]=ID_FORM;
		clip->clip_Header[1]=3*sizeof(ULONG)+length;
		clip->clip_Header[2]=ID_FTXT;
		clip->clip_Header[3]=ID_CHRS;
		clip->clip_Header[4]=length;

		// Odd-length string?
		if (length&1)
		{
			++clip->clip_Header[1];
			++length;
		}

		// Write header
		clip->clip_Req->io_Data=(APTR)clip->clip_Header;
		clip->clip_Req->io_Length=sizeof(ULONG)*5;
		clip->clip_Req->io_Offset=0;
		clip->clip_Req->io_Command=CMD_WRITE;
		DoIO((struct IORequest *)clip->clip_Req);

		// Write string
		if (length>0)
		{
			clip->clip_Req->io_Data=string;
			clip->clip_Req->io_Length=length;
			DoIO((struct IORequest *)clip->clip_Req);
		}

		// Set write flag
		clip->clip_Write=1;
		return 1;
	}

	return 0;
}

long __asm __saveds L_ReadClipString(
	register __a0 ClipHandle *clip,
	register __a1 char *string,
	register __d0 long length)
{
	long len=0;

	// Valid clip?
	if (clip)
	{
		// Clear header
		clip->clip_Header[0]=0;
		clip->clip_Header[2]=0;

		// Read header
		clip->clip_Req->io_Data=(APTR)clip->clip_Header;
		clip->clip_Req->io_Length=sizeof(ULONG)*5;
		clip->clip_Req->io_Offset=0;
		clip->clip_Req->io_Command=CMD_READ;
		if (DoIO((struct IORequest *)clip->clip_Req)) return 0;

		// Valid text?
		if (clip->clip_Header[0]==ID_FORM && clip->clip_Header[2]==ID_FTXT)
		{
			// Check maximum length
			if (clip->clip_Header[4]>=length) clip->clip_Header[4]=length-1;

			// Read string
			clip->clip_Req->io_Data=string;
			clip->clip_Req->io_Length=clip->clip_Header[4];
			if (DoIO((struct IORequest *)clip->clip_Req)) return 0;

			// Null-terminate
			string[clip->clip_Header[4]]=0;

			// Get length
			len=(long)clip->clip_Header[4];
		}

		// Flush clipboard
		clip->clip_Req->io_Data=clip->clip_Buffer;
		clip->clip_Req->io_Length=128;
		while (clip->clip_Req->io_Actual)
		{
			if (DoIO((struct IORequest *)clip->clip_Req))
				break;
		}
	}

	return len;
}
