#ifndef _GALILEOFM_LISTS
#define _GALILEOFM_LISTS

/*****************************************************************************

 List management

 *****************************************************************************/

typedef struct _Att_List
{
	struct List		list;		// List structure
	struct SignalSemaphore	lock;		// Semaphore for locking
	ULONG			flags;		// Flags
	APTR			memory;		// Memory pool
	struct _Att_Node	*current;	// Current node (application use)
} Att_List;

#define LISTF_LOCK		(1<<0)		// List requires locking
#define LISTF_POOL		(1<<1)		// Use memory pooling

typedef struct _Att_Node
{
	struct Node		node;		// Node structure
	Att_List		*list;		// Pointer back to list
	ULONG			att_data;	    // User data
} Att_Node;

#define ADDNODEF_SORT		1		// Sort names
#define ADDNODEF_EXCLUSIVE	2		// Exclusive entry
#define ADDNODEF_NUMSORT	4		// Numerical name sort
#define ADDNODEF_PRI		8		// Priority insertion

#define REMLISTF_FREEDATA	1		// FreeVec data when freeing list
#define REMLISTF_SAVELIST	2		// Don't free list itself
#define REMLISTF_FREEMEMH	4		// Use FreeMemH to free data

void AddSorted(struct List *,struct Node *node);
void Att_ChangeNodeName(Att_Node *,char *);
Att_Node *Att_FindNode(Att_List *,long);
Att_Node *Att_FindNodeData(Att_List *,ULONG);
long Att_FindNodeNumber(Att_List *,Att_Node *);
Att_List *Att_NewList(ULONG);
Att_Node *Att_NewNode(Att_List *,char *,ULONG,ULONG);
long Att_NodeCount(Att_List *);
long Att_NodeDataNumber(Att_List *,ULONG);
char *Att_NodeName(Att_List *,long);
long Att_NodeNumber(Att_List *,char *);
void Att_PosNode(Att_List *,Att_Node *,Att_Node *);
void Att_RemList(Att_List *,long);
void Att_RemNode(Att_Node *);
struct Node *FindNameI(struct List *,char *);
void LockAttList(Att_List *,BOOL);
void SwapListNodes(struct List *,struct Node *,struct Node *);
void UnlockAttList(Att_List *);


/*****************************************************************************

 Semaphore management

 *****************************************************************************/

struct ListLock {
	struct List		list;
	struct SignalSemaphore	lock;
};

#define SEMF_SHARED		0
#define SEMF_EXCLUSIVE		(1<<0)
#define SEMF_ATTEMPT		(1<<1)

long GetSemaphore(struct SignalSemaphore *,long,APTR);
void InitListLock(struct ListLock *,APTR);
BOOL IsListLockEmpty(struct ListLock *);

#endif
