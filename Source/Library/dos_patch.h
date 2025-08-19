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


BPTR __asm PatchedCreateDir(register __d1 char *name);
BPTR __asm OriginalCreateDir(
	register __d1 char *name);
long __asm PatchedDeleteFile(register __d1 char *name);
long __asm OriginalDeleteFile(
	register __d1 char *name);
BOOL __asm PatchedSetFileDate(
	register __d1 char *name,
	register __d2 struct DateStamp *date);
BOOL __asm OriginalSetFileDate(
	register __d1 char *name,
	register __d2 struct DateStamp *date);
BOOL __asm PatchedSetComment(
	register __d1 char *name,
	register __d2 char *comment);
BOOL __asm OriginalSetComment(
	register __d1 char *name,
	register __d2 char *comment);
BOOL __asm PatchedSetProtection(
	register __d1 char *name,
	register __d2 ULONG mask);
BOOL __asm OriginalSetProtection(
	register __d1 char *name,
	register __d2 ULONG mask);
BOOL __asm PatchedRename(
	register __d1 char *oldname,
	register __d2 char *newname);
BOOL __asm OriginalRename(
	register __d1 char *oldname,
	register __d2 char *newname);
BPTR __asm PatchedOpen(
	register __d1 char *name,
	register __d2 LONG accessMode);
BPTR __asm OriginalOpen(
	register __d1 char *name,
	register __d2 LONG accessMode);
BOOL __asm PatchedClose(register __d1 BPTR file);
BOOL __asm OriginalClose(
	register __d1 BPTR file);
LONG __asm PatchedWrite(
	register __d1 BPTR file,
	register __d2 void *data,
	register __d3 LONG length);
LONG __asm OriginalWrite(
	register __d1 BPTR file,
	register __d2 void *data,
	register __d3 LONG length);
BOOL __asm PatchedRelabel(
	register __d1 char *volumename,
	register __d2 char *name);
BOOL __asm OriginalRelabel(
	register __d1 char *volumename,
	register __d2 char *name);

struct FileInfoBlock *dospatch_fib(BPTR,BOOL);

struct FileHandleWrapper
{
	struct MinNode		fhw_Node;
	BPTR			fhw_FileHandle;
	ULONG			fhw_Flags;
	BPTR			fhw_Parent;
};

#define FHWF_WRITTEN		(1<<0)		// File has been written to

struct FileHandleWrapper *find_filehandle(BPTR file);
