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

typedef struct _Att_List {
	struct List list;
} Att_List;

typedef struct _Att_Node {
	struct Node node;
	Att_List *list;
	ULONG data;
} Att_Node;

#define ADDNODE_SORT		1		// Sort names
#define ADDNODE_EXCLUSIVE	2		// Exclusive entry
#define ADDNODE_NUMSORT		4		// Numerical name sort

#define REMLIST_FREEDATA	1		// FreeVec data when freeing list

// prototypes
Att_List *Att_NewList(void);
Att_Node *Att_NewNode(Att_List *list,char *name,ULONG data,ULONG flags);
void Att_RemNode(Att_Node *node);
void Att_PosNode(Att_List *,Att_Node *,Att_Node *);
void Att_RemList(Att_List *list,int);
Att_Node *Att_FindNode(Att_List *list,int number);
Att_NodeNumber(Att_List *list,char *name);
Att_Node *Att_FindNodeData(Att_List *list,ULONG data);
Att_NodeDataNumber(Att_List *list,ULONG data);

#define VALIDLIST(l) ((l) && (l)->list.lh_Head->ln_Succ)
