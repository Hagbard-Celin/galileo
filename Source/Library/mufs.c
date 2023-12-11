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

struct LoginPkt
{
	struct Window	*window;
	char			*name;			// not used
	char			*password;		// not used
};

// Logout (must be called from GALILEO_LAUNCHER process)
void do_mufs_logout(struct LoginPkt *pkt)
{
	struct muBase *muBase;
	struct Process *proc;
	APTR wsave;

	// Set window pointer
	proc=(struct Process *)FindTask(0);
	wsave=proc->pr_WindowPtr;
	proc->pr_WindowPtr=pkt->window;

	// Open MUFS library
	if (muBase=(struct muBase *)OpenLibrary("multiuser.library",39))
	{
		struct muUserInfo *info;

		// Allocate user info
		if (info=muAllocUserInfo())
		{
			struct TagItem tags[6];

			// Fill out tags to logout Galileo
			tags[0].ti_Tag=muT_Global;
			tags[0].ti_Data=TRUE;
			tags[1].ti_Tag=muT_Quiet;
			tags[1].ti_Data=TRUE;
			tags[2].ti_Tag=muT_All;
			tags[2].ti_Data=TRUE;
			tags[3].ti_Tag=muT_Task;
			tags[3].ti_Data=(ULONG)FindTask("galileo");
			tags[4].ti_Tag=TAG_END;

			// Log out
			muLogoutA(tags);

			// Fill out tags to logout GALILEO_LAUNCHER and log back in again
			tags[0].ti_Tag=muT_Graphical;
			tags[0].ti_Data=TRUE;
			tags[1].ti_Tag=muT_Global;
			tags[1].ti_Data=TRUE;
			tags[2].ti_Tag=TAG_END;

			// Log this task out
			info->uid=(muLogoutA(tags)>>16)&0xffff;
			muGetUserInfo(info,muKeyType_uid);

			// Log Galileo task in
			tags[0].ti_Tag=muT_Graphical;
			tags[0].ti_Data=TRUE;
			tags[1].ti_Tag=muT_Task;
			tags[1].ti_Data=(ULONG)FindTask("galileo");
			tags[2].ti_Tag=muT_Own;
			tags[2].ti_Data=TRUE;
			tags[3].ti_Tag=muT_Global;
			tags[3].ti_Data=TRUE;
			tags[4].ti_Tag=muT_UserID;
			tags[4].ti_Data=(ULONG)info->UserID;
			tags[5].ti_Tag=TAG_END;

			// Log in
			muLoginA(tags);

			// Free user info
			muFreeUserInfo(info);
		}

		// Close MUFS
		CloseLibrary((struct Library * )muBase);
	}

	// Restore window pointer
	proc->pr_WindowPtr=wsave;
}
