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

#include "/Library/galileofmbase.h"
#include "/Library/galileofmpragmas.h"

struct Library *GalileoFMBase;

#define SetFlag(v,f)		((v)|=(f))
#define ClearFlag(v,f)		((v)&=~(f))
#define ToggleFlag(v,f) 	((v)^=(f))
#define FlagIsSet(v,f)		(((v)&(f))!=0)
#define FlagIsClear(v,f)	(((v)&(f))==0)


void main(int argc,char **argv)
{
char *name;
struct DiskObject *icon;
LONG flags;

	if (GalileoFMBase=OpenLibrary("galileofm.library",0))
	{
	if (argc>1)
		{
		name = argv[1];
		printf("icon %s\n",name);

		if	(IconBase->lib_Version>=44)
			icon=GetIconTags(name,
				ICONGETA_FailIfUnavailable,TRUE,
				TAG_DONE);
		else
			icon=GetDiskObject(name);

		if	(icon)
			{
			ULONG mx,ud;

			ud=(ULONG)icon->do_Gadget.UserData;
			mx=icon->do_Gadget.MutualExclude; 

			printf("UserData %lx MutualExclude %lx\n",ud,mx);

			flags=GetIconFlags(icon);
			printf("%lx\n",flags);

			printf("ICONF_POSITION_OK %lx\n",flags&ICONF_POSITION_OK);
			printf("ICONF_ICON_VIEW %lx\n",flags&ICONF_ICON_VIEW);
			printf("ICONF_BORDER_OFF %lx\n",flags&ICONF_BORDER_OFF);
			printf("ICONF_NO_LABEL %lx\n",flags&ICONF_NO_LABEL);
			printf("ICONF_BORDER_ON %lx\n",flags&ICONF_BORDER_ON);


			if	(argc==3)
				{
				if	(!strcmp(argv[2],"on"))
					{
					printf("ON\n");

					ClearFlag(flags,ICONF_BORDER_OFF);
					SetFlag(flags,ICONF_BORDER_ON);

					}

				else
					{
					printf("OFF\n");

					SetFlag(flags,ICONF_BORDER_OFF);
					ClearFlag(flags,ICONF_BORDER_ON);
					}


			ud=(ULONG)icon->do_Gadget.UserData;
			mx=icon->do_Gadget.MutualExclude; 

			printf("UserData %lx MutualExclude %lx\n",ud,mx);

			printf("%lx\n",flags);

			printf("ICONF_BORDER_OFF %lx\n",flags&ICONF_BORDER_OFF);
			printf("ICONF_NO_LABEL %lx\n",flags&ICONF_NO_LABEL);
			printf("ICONF_BORDER_ON %lx\n",flags&ICONF_BORDER_ON);

		
				SetIconFlags(icon,flags);
				PutDiskObject(name,icon);



				}

			FreeDiskObject(icon);
			}
		}
	

	CloseLibrary(GalileoFMBase);
	}
}
