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

#include <stdio.h>
#include <proto/exec.h>
#include "galileofmbase.h"
#include "galileofmpragmas.h"
#ifdef _DEBUG
#include <clib/debug_protos.h>
#endif

#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>
#endif

struct Library *ResTrackBase;

struct Library *GalileoFMBase;

void main(void);

#ifdef RESOURCE_TRACKING
    ULONG callerid=(ULONG)&main;
#endif

void main(void)
{
    ResTrackBase=0;

#ifdef RESOURCE_TRACKING

    if (ResTrackBase=REALL_OpenLibrary("g_restrack.library",0))
         StartResourceTracking (RTL_ALL);
#endif

	if (GalileoFMBase=OpenLibrary("galileofm.library",0))
	{
		ULONG original;
		APTR handle;
		void **mem;
		int a;

		mem=AllocMemH(0,512*4);

		printf("\navail mem : %ld\n\n",(original=AvailMem(MEMF_ANY)));

		handle=NewMemHandle(4096,256,MEMF_CLEAR);
		printf("handle %lx allocating 128x4096 using pools (puddle 4096)...\n",handle);
		for (a=0;a<4096;a++)
			if (!(AllocMemH(handle,128)))
			{
				printf("failed at %ld\n",a);
				break;
			}
		printf("allocating 128x512\n");
		for (a=0;a<512;a++)
			if (!(mem[a]=AllocMemH(handle,128)))
			{
				printf("failed at %ld\n",a);
				break;
			}
		printf("used : %ld\n",original-AvailMem(MEMF_ANY));
		printf("freeing 128x512\n");
		for (a=0;a<512;a++)
			if (mem[a])
			{
				FreeMemH(mem[a]);
				mem[a]=0;
			}
		printf("freeing memory\n");
		ClearMemHandle(handle);
		printf("lost: %ld\n",original-AvailMem(MEMF_ANY));
		printf("allocate 1024 : %lx\n",AllocMemH(handle,1024));
		FreeMemHandle(handle);
		printf("lost: %ld\n",original-AvailMem(MEMF_ANY));

		printf("\navail mem : %ld\n\n",(original=AvailMem(MEMF_ANY)));

		handle=NewMemHandle(0,0,MEMF_CLEAR);
		printf("handle %lx allocating 128x4096 without pools...\n",handle);
		for (a=0;a<4096;a++)
			if (!(AllocMemH(handle,128)))
			{
				printf("failed at %ld\n",a);
				break;
			}
		printf("allocating 128x512\n");
		for (a=0;a<512;a++)
			if (!(mem[a]=AllocMemH(handle,128)))
			{
				printf("failed at %ld\n",a);
				break;
			}
		printf("used : %ld\n",original-AvailMem(MEMF_ANY));
		printf("freeing 128x512\n");
		for (a=0;a<512;a++)
			if (mem[a])
			{
				FreeMemH(mem[a]);
				mem[a]=0;
			}
		printf("freeing memory\n");
		ClearMemHandle(handle);
		printf("lost: %ld\n",original-AvailMem(MEMF_ANY));
		printf("allocate 1024 : %lx\n",AllocMemH(handle,1024));
		FreeMemHandle(handle);
		printf("lost: %ld\n",original-AvailMem(MEMF_ANY));

		printf("\navail mem : %ld\n\n",(original=AvailMem(MEMF_ANY)));

		printf("allocate 131072 bytes without a handle : %lx\n",
			(handle=AllocMemH(0,128*1024)));
		printf("used : %ld\n",original-AvailMem(MEMF_ANY));
		printf("freeing memory\n");
		FreeMemH(handle);

		printf("allocating 128x512\n");
		for (a=0;a<512;a++)
			if (!(mem[a]=AllocMemH(0,128)))
			{
				printf("failed at %ld\n",a);
				break;
			}
		printf("used : %ld\n",original-AvailMem(MEMF_ANY));
		printf("freeing 128x512\n");
		for (a=0;a<512;a++)
			if (mem[a])
			{
				FreeMemH(mem[a]);
				mem[a]=0;
			}
		printf("lost: %ld\n",original-AvailMem(MEMF_ANY));

		printf("\navail mem : %ld\n\n",(original=AvailMem(MEMF_ANY)));

		FreeMemH(mem);

		CloseLibrary(GalileoFMBase);
	}
#ifdef RESOURCE_TRACKING
    KPrintF("Quitting......\n");
    PrintTrackedResources();
    //EndResourceTracking(); /* Generate a memory usage report */
#endif
}
