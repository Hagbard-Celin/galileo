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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "misc.h"

char *version="$VER: misc.gfmmodule 0.2 "__AMIGADATE__" ";

int __asm __saveds L_Module_Entry_Internal(
	register __a0 struct List *files,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __d0 ULONG mod_id,
	register __d1 ULONG mod_data)
{
	struct IOAudio audio;
	struct MsgPort *reply_port;

	// Flash only?
	if (mod_id==FLASH)
	{
		DisplayBeep(screen);
		return 1;
	}

	// Create port
	if (!(reply_port=CreateMsgPort()))
		return 0;

	// Fill out audio request
	audio.ioa_Request.io_Message.mn_ReplyPort=reply_port;
	audio.ioa_Request.io_Message.mn_Node.ln_Pri=90;
	audio.ioa_Data=achannels;
	audio.ioa_Length=sizeof(achannels);

	// Open audio device
	if (!(OpenDevice("audio.device",0,(struct IORequest *)&audio,0)))
	{
		// Initialise write command
		audio.ioa_Request.io_Command=CMD_WRITE;
		audio.ioa_Request.io_Flags=ADIOF_PERVOL;
		audio.ioa_Volume=64;
		audio.ioa_Data=(UBYTE *)beepwave;

		// It's an alarm?
		if (mod_id==ALARM)
		{
			short a;

			// Waveform size
			audio.ioa_Length=8;

			// Standard length
			audio.ioa_Cycles=60;

			// Five cycles
			for (a=0;a<5;a++)
			{
				// High tone
				audio.ioa_Period=800;

				// Play sound
				BeginIO((struct IORequest *)&audio);
				WaitIO((struct IORequest *)&audio);

				// Low tone
				audio.ioa_Period=1200;

				// Play sound
				BeginIO((struct IORequest *)&audio);
				WaitIO((struct IORequest *)&audio);
			}
		}

		// Waveform size
		else audio.ioa_Length=sizeof(beepwave);

		// Set note information
		audio.ioa_Period=(mod_id==BEEP)?300:800;
		audio.ioa_Cycles=(mod_id==BEEP)?100:60;

		// Play sound
		BeginIO((struct IORequest *)&audio);
		WaitIO((struct IORequest *)&audio);

		// Close audio device
		CloseDevice((struct IORequest *)&audio);
	}

	// Delete message port
	DeleteMsgPort(reply_port);
	return 1;
}
