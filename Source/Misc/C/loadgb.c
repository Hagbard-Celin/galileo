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

#include "//Library/galileofmbase.h"
#include "//Library/galileofmpragmas.h"

#if RESOURCE_TRACKING
#include <restrack_protos.h>
#include <restrack_pragmas.h>

struct Library *ResTrackBase;

char *callerid;
#endif

char *version="$VER: LoadDB 0.1 "__AMIGADATE__" ";

enum
{
	ARG_DEBUG,
	ARG_DELAY,
	ARG_CLEANUP,
	ARG_NEWPATH,
	ARG_COUNT
};

#if RESOURCE_TRACKING
#define CALLER_ID "LoadGB"
char *callerid=0; // "MainExe";

#endif

BOOL workbench_running(void);

void __stdargs __main(char *arg_string)
{
	BOOL ok=0,run_wb=0,wb_startup=1,wb_running;
	BPTR lock;
	struct Process *proc;
	APTR wsave;
	char buf[256];
	struct Library *InputBase;
	struct Library *GalileoFMBase;
	struct IOStdReq input_req;
	short a;
	char *arg_ptr=0;
	struct RDArgs *args;
	ULONG arg_array[ARG_COUNT];

#if RESOURCE_TRACKING
    if (ResTrackBase=REALL_OpenLibrary("restrack.library",0))
         StartResourceTracking (RTL_ALL);

    callerid=NRT_AllocVec(strlen(CALLER_ID)+1,MEMF_PUBLIC|MEMF_CLEAR);
    strcpy(callerid,CALLER_ID);
    //*callerid=CALLER_ID;
#endif

	// Initialise arguments
	for (a=0;a<ARG_COUNT;a++)
		arg_array[a]=0;

	// Parse arguments
	if (args=ReadArgs("-DEBUG/S,DELAY/S,CLEANUP/S,NEWPATH/S",(long *)arg_array,0))
	{
		// Free arguments
		FreeArgs(args);
	}

	// See if Workbench is running
	wb_running=workbench_running();

	// New path?
	if (arg_array[ARG_NEWPATH])
	{
		struct Library *GalileoFMBase;
		BOOL ok=0;
	
		// See if Galileo is running
		Forbid();
        if (FindPort("Galileo"))
		{
			// Open library
            Permit();
			if (GalileoFMBase=OpenLibrary("Galileo:libs/galileofm.library",0))
			{
				// Update the path list from current process
				UpdatePathList();

				// Close library
				CloseLibrary(GalileoFMBase);

				// Set flag to say we got a new path
				ok=1;
			}
		}
        else
        {
            Permit();
		}
		// Is Workbench also running?
		if (wb_running)
		{
			// Set flag to pass through to Workbench
			run_wb=1;
		}

		// Otherwise, exit now, unless Galileo has to be launched
		else
		if (ok) exit(0);
	}

	// Open input device
	if (!run_wb && !(OpenDevice("input.device",0,(struct IORequest *)&input_req,0)))
	{
		// Get input base
		InputBase=(struct Library *)input_req.io_Device;

		// See if shift is held down
		if (PeekQualifier()&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
		{
			// Run Workbench instead
			run_wb=1;
		}

		// Is control held down?
		else
		if (PeekQualifier()&IEQUALIFIER_CONTROL)
		{
			// No WBStartup
			wb_startup=0;
		}

		// Close input device
		CloseDevice((struct IORequest *)&input_req);
	}

	// Find argument pointer
	for (a=0;arg_string[a] && arg_string[a]!=' ';a++);
	if (arg_string[a]) arg_ptr=arg_string+a;

	// Try to run Galileo?
	if (!run_wb)
	{
		// See if Workbench is already running; if so, we don't do WBStartup again
		if (wb_running) wb_startup=0;

		// Get this process, turn off requesters
		proc=(struct Process *)FindTask(0);
		wsave=proc->pr_WindowPtr;
		proc->pr_WindowPtr=(APTR)-1;

		// See if Galileo is there
		if (lock=Lock("Galileo:Galileo",ACCESS_READ))
		{
			// Yup
			UnLock(lock);

			// Open library
			if (GalileoFMBase=OpenLibrary("Galileo:libs/galileofm.library",0))
			{
				CloseLibrary(GalileoFMBase);

				// Build command string
				strcpy(buf,"Galileo:Galileo ENVIRONMENT=workbench ");
				if (wb_startup) strcat(buf," WBSTARTUP=YES ");
				if (arg_ptr) strcat(buf,arg_ptr);

				// Strip LF
				for (arg_ptr=buf;*arg_ptr;arg_ptr++)
					if (*arg_ptr=='\n') *arg_ptr=0;

				// Try launch
				ok=Execute(buf,0,0);
			}
		}

		// Restore requesters
		proc->pr_WindowPtr=wsave;
	}

	// Did we fail?
	if (!ok)
	{
		// Failed for some reason; start old-fashioned Workbench
		strcpy(buf,"c:LoadWB");

		// Add Workbench arguments
		if (arg_array[ARG_DEBUG]) strcat(buf," -DEBUG");
		if (arg_array[ARG_DELAY]) strcat(buf," DELAY");
		if (arg_array[ARG_CLEANUP]) strcat(buf," CLEANUP");
		if (arg_array[ARG_NEWPATH]) strcat(buf," NEWPATH");

		// Run it
		Execute(buf,0,0);
	}

	exit(0);
}


// See if Workbench is running
BOOL workbench_running(void)
{
	struct Screen *screen;
	struct List *list;
	BOOL wb=0;

	// Lock public screen list
	if (list=LockPubScreenList())
	{
		struct PubScreenNode *pub;
		BOOL ok=0;

		// Go through screens
		for (pub=(struct PubScreenNode *)list->lh_Head;
			pub->psn_Node.ln_Succ;
			pub=(struct PubScreenNode *)pub->psn_Node.ln_Succ)
		{
			// Workbench screen?
			if (stricmp(pub->psn_Node.ln_Name,"Workbench")==0)
			{
				ok=1;
				break;
			}
		}

		// Unlock list
		UnlockPubScreenList();

		// If Workbench screen isn't open, don't try to lock it
		if (!ok) return 0;
	}

	// Lock Workbench screen
	if (screen=LockPubScreen("Workbench"))
	{
		struct Window *window;
		ULONG lock;

		// Lock Intuition
		lock=LockIBase(0);

		// Go through windows on screen
		for (window=screen->FirstWindow;
			window;
			window=window->NextWindow)
		{
			// Look for Workbench windows
			if (window->Flags&WFLG_WBENCHWINDOW)
			{
				wb=1;
				break;
			}
		}

		// Unlock Intuition, and screen
		UnlockIBase(lock);
		UnlockPubScreen(0,screen);
	}

	return wb;
}
