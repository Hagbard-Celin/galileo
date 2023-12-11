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

enum
{
	PROG_NAME,
	PROG_SIZE,
	PROG_INFO,
	PROG_INFO2,
	PROG_INFO3,
	PROG_GRAPH,
	PROG_LAST,
};

typedef struct _ProgressWindow
{
	struct Window		*pw_Window;		// Window pointer
	IPCData			*pw_IPC;		// IPC pointer

	struct Window		*pw_OwnerWindow;	// Owner window
	struct Screen		*pw_Screen;		// Screen we're on

	char			pw_Title[40];		// Window title

	unsigned long		pw_Flags;		// Flags

	char			pw_FileName[80];	// Current filename
	long			pw_FileSize;		// Current filesize
	long			pw_FileDone;		// Amount of file done

	long			pw_FileCount;		// Total number of files
	long			pw_FileNum;		// Current file number

	char			pw_Information[80];	// Current information line

	struct Task		*pw_SigTask;		// Task to signal for abort
	long			pw_SigBit;		// Signal bit to use

	long			pw_A4;			// A4 register
	struct MyLibrary	*pw_Lib;		// Library pointer

	Point			pw_Offset;		// Coordinate offset
	struct Gadget		*pw_Abort;		// Abort gadget
	struct DrawInfo		*pw_DrawInfo;		// DrawInfo
	char			*pw_SizeString;		// Cache for size string

	Point			pw_WindowPos;		// Window position
	BOOL			pw_PosValid;		// Position is valid?

	short			pw_ProgWidth;		// Width of progress bar filled

	WindowID		pw_ID;			// Window ID
	struct Hook		*pw_Backfill;		// Backfill hook

	char			pw_TaskName[40];	// Task name

	char			pw_Information2[80];	// Current information line
	char			pw_Information3[80];	// Current information line

	struct IBox		pw_Coords[PROG_LAST+1];	// Coordinates
} ProgressWindow;


#define PWF_ALL			(PWF_FILENAME|PWF_FILESIZE|PWF_INFO|PWF_GRAPH)

#define PWF_ABORTED		(1<<16)

#define PROGRESS_SET		1000
#define PROGRESS_GET		1001
