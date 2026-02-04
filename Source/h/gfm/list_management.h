/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#define _GALILEOFM_LIST_MANAGEMENT_H

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
#ifndef	EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#define ADDNODE_SORT		1		// Sort names
#define ADDNODE_EXCLUSIVE	2		// Exclusive entry
#define ADDNODE_NUMSORT		4		// Numerical name sort
#define ADDNODE_PRI		8		// Priority insertion
#define ADDNODE_LOCKNODE	16		// Create Att_LockNode
#define ADDNODE_DISKCHANGENODE	32		// Create Att_DiskChangeNode

typedef struct _Att_List
{
    struct List		    list;	    // List structure
    struct SignalSemaphore  lock;	    // Semaphore for locking
    ULONG		    flags;	    // Flags
    APTR		    memory;	    // Memory pool
    struct _Att_Node	    *current;	    // Current node (application use)
} Att_List;

// List management stuff
typedef struct _Att_Node
{
    struct Node		    node;	    // Node structure
    Att_List		    *list;	    // Pointer to list (inefficient!)
    ULONG		    att_data;	    // User data
} Att_Node;

typedef struct _Att_LockNode
{
    struct Node		    node;	    // Node structure
    Att_List		    *list;	    // Pointer to list (inefficient!)
    ULONG		    att_data;	    // User data
    BPTR		    att_lock;	    // Parent lock
} Att_LockNode;

#define REMLIST_FREEDATA	1		// FreeVec data when freeing list
#define REMLIST_SAVELIST	2		// Don't free list itself
#define REMLIST_FREEMEMH	4		// FreeMemH data when freeing list
#define REMLIST_UNLOCK		8		// UnLock locks when freeing list

#define LISTF_LOCK	(1<<0)
#define LISTF_POOL	(1<<1)

Att_List *Att_NewList(ULONG);
Att_Node *Att_NewNode(Att_List *list,char *name,ULONG data,ULONG flags);
void AddSorted(struct List *list,struct Node *node);
Att_Node *Att_FindNode(Att_List *list,long number);
Att_Node *Att_FindNodeData(Att_List *list,ULONG data);
void Att_PosNode(Att_List *,Att_Node *,Att_Node *);
Att_FindNodeNumber(Att_List *list,Att_Node *node);
struct Node *FindNameI(struct List *,char *name);
void Att_RemList(Att_List *list,long);
void Att_RemNode(Att_Node *node);
Att_NodeCount(Att_List *list);
void SwapListNodes(struct List *,struct Node *,struct Node *);
void Att_ChangeNodeName(Att_Node *node,char *name);

Att_NodeNumber(Att_List *list,char *name);
void InitListLock(struct ListLock *,char *);
BOOL IsListLockEmpty(struct ListLock *);
Att_NodeDataNumber(Att_List *list,ULONG data);

#endif

