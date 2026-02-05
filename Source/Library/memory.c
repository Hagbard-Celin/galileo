/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023 Hagbard Celine

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

#include "galileofmlib.h"
#ifdef RESOURCE_TRACKING
#ifdef L_NewMemHandle
#undef L_NewMemHandle
#endif
#ifdef L_FreeMemHandle
#undef L_FreeMemHandle
#endif
#ifdef L_ClearMemHandle
#undef L_ClearMemHandle
#endif
#ifdef L_AllocMemH
#undef L_AllocMemH
#endif
#ifdef L_FreeMemH
#undef L_FreeMemH
#endif
#endif
/*
	Create a memory handle

	puddle_size = size of puddles to use for pooling (0 for no pools)
	thresh_size = threshold of puddle allocations
	type        = type of memory

	              MEMF_CLEAR  = clear memory
	              MEMF_PUBLIC = use semaphore locking when accessing pool

		  Other MEMF flags as normal
*/

void *__asm __saveds L_NewMemHandle(
	register __d0 ULONG puddle_size,
	register __d1 ULONG thresh_size,
	register __d2 ULONG type)
{
	MemHandle *handle;

	// Allocate handle
#ifdef RESOURCE_TRACKING
	if (!(handle=NRT_AllocVec(sizeof(MemHandle),MEMF_CLEAR)))
#else
	if (!(handle=AllocVec(sizeof(MemHandle),MEMF_CLEAR)))
#endif
		return 0;

	// Initialise check value
	handle->check_val=(ULONG)handle;

	// Want pooling?
	if (puddle_size>0)
	{
		long thresh_count;

		// Calculate number of "thresholds" in the puddle size
		thresh_count=UDivMod32(puddle_size,thresh_size);

		// Increment thresh size by size of allocation headers
		thresh_size+=3*sizeof(ULONG);

		// Increment puddle size by size of allocation headers
		puddle_size+=3*sizeof(ULONG)*thresh_count;
	}

	// Initialise handle
	NewList((struct List *)&handle->memory_list);
	InitSemaphore(&handle->lock);
	handle->puddle_size=puddle_size;
	handle->thresh_size=thresh_size;

	// Want memory clearing?
	if (type&MEMF_CLEAR) handle->flags=MEMHF_CLEAR;

	// Want locking?
	if (type&MEMF_PUBLIC)
	{
		handle->flags|=MEMHF_LOCK;
		type&=~MEMF_PUBLIC;
	}

	// Save type of memory
	handle->type=type;

	// If puddle size is >0, try to create a pool
	if (puddle_size>0)
	{
	// Avoid redundant clearing of memory
	type&=~MEMF_CLEAR;

		handle->pool_header=
			(SysBase->LibNode.lib_Version>=39)?	CreatePool(type,puddle_size,thresh_size):
							AsmCreatePool(type,puddle_size,thresh_size,(struct Library *)SysBase);
	}

	// Return handle
	return handle;
}


// Free a memory handle and all associated allocations
void __asm __saveds L_FreeMemHandle(register __a0 MemHandle *handle)
{
	// Valid handle?
	if (handle)
	{
		// Did the handle have a memory pool?
		if (handle->pool_header)
		{
			// Under 39 use library routines
			if (SysBase->LibNode.lib_Version>=39)
				DeletePool(handle->pool_header);
			else
				AsmDeletePool(handle->pool_header,(struct Library *)SysBase);
		}

		// Otherwise, free all memory allocations
		else L_ClearMemHandle(handle);

		// Free the handle
#ifdef RESOURCE_TRACKING
		NRT_FreeVec(handle);
#else
		FreeVec(handle);
#endif
	}
}


// Clear all memory allocated with a handle (leave handle intact)
void __asm __saveds L_ClearMemHandle(register __a0 MemHandle *handle)
{
	BOOL realloc=1;

	// Valid handle?
	if (handle)
	{
		// Want locking?
		if (handle->flags&MEMHF_LOCK) ObtainSemaphore(&handle->lock);

		// Did the handle have a memory pool?
		if (handle->pool_header)
		{
			// Is there any memory allocated?
			if (handle->total>0)
			{
				// Delete pool
				if (SysBase->LibNode.lib_Version>=39)
					DeletePool(handle->pool_header);
				else
					AsmDeletePool(handle->pool_header,(struct Library *)SysBase);
				handle->pool_header=0;
			}

			// Otherwise, don't need to clear
			else realloc=0;
		}

		// Otherwise, we have a list of allocations
		else
		{
			struct MinNode *node,*next;

			// Go through list, free all allocations
			for (node=handle->memory_list.mlh_Head;node->mln_Succ;node=next)
			{
				ULONG *ptr;

				// Get next pointer
				next=node->mln_Succ;

				// Free this entry
				ptr=(ULONG *)node;
#ifdef RESOURCE_TRACKING
				NRT_FreeMem(node,ptr[3]);
#else
				FreeMem(node,ptr[3]);
#endif
			}
		}

		// Clear allocation total
		handle->total=0;

		// Did we need to clear it?
		if (realloc)
		{
			// Initialise memory list
			NewList((struct List *)&handle->memory_list);

			// Did we have pooling before?
			if (handle->puddle_size>0)
			{
				// Try to create a new pool. If this fails we'll be using allocation lists from now on
				handle->pool_header=
					(SysBase->LibNode.lib_Version>=39)?	CreatePool(handle->type&~MEMF_CLEAR,handle->puddle_size,handle->thresh_size):
										AsmCreatePool(handle->type&~MEMF_CLEAR,handle->puddle_size,handle->thresh_size,(struct Library *)SysBase);
			}
		}

		// Unlock if necessary
		if (handle->flags&MEMHF_LOCK) ReleaseSemaphore(&handle->lock);
	}
}


/*
	Allocate some memory, with size tracking

	Allocated as :    [<mln_Succ>]		-16
	                  [<mln_Pred>]		-12
	                  <handle>			-8
	                  <size>			-4
	                  <memory pointer>	0

	handle = handle to use for allocations
	     can be 0 in which case size will be tracked but memory must be
	     freed manually (ie can't FreeMemHandle())
	size   = size of allocation
*/

void *__asm __saveds L_AllocMemH(
	register __a0 MemHandle *handle,
	register __d0 ULONG size)
{
	register ULONG *mem;

	// Get size to allocate (size rounded up plus 2 longwords)
	size=(((size+3)>>2)+2)<<2;

	// If no handle supplied, use AllocMem()
	if (!handle)
	{
#ifdef RESOURCE_TRACKING
		mem=NRT_AllocMem(size,MEMF_CLEAR);
#else
		mem=AllocMem(size,MEMF_CLEAR);
#endif
	}
	// Allocate memory through handle
	else
	{
		// Lock if necessary
		if (handle->flags&MEMHF_LOCK) ObtainSemaphore(&handle->lock);

		// If there's a memory pool, use that
		if (handle->pool_header)
		{
			// Use exec routines under 39, lib routines before that
			mem=(SysBase->LibNode.lib_Version>=39)?	AllocPooled(handle->pool_header,size):
								AsmAllocPooled(handle->pool_header,size,(struct Library *)SysBase);

			// If successful
			if (mem)
			{
				// Increment allocation total
				handle->total+=size;

				// Need to clear?
				if (handle->flags&MEMHF_CLEAR)
				{
					register ULONG clr,clrsize;

					// Clear memory
					clrsize=size>>2;
					for (clr=0;clr<clrsize;clr++) mem[clr]=0;
				}
			}
		}

		// No memory pool
		else
		{
			// Need extra storage for a MinNode
			size+=sizeof(struct MinNode);

			// Allocate extra, for a node
#ifdef RESOURCE_TRACKING
			if (mem=NRT_AllocMem(size,handle->type))
#else
			if (mem=AllocMem(size,handle->type))
#endif
			{
				// Add to memory list
				AddTail((struct List *)&handle->memory_list,(struct Node *)mem);

				// Bump memory pointer past node
				mem+=2;

				// Add to allocation total
				handle->total+=size;
			}
		}

		// Unlock if necessary
		if (handle->flags&MEMHF_LOCK) ReleaseSemaphore(&handle->lock);
	}

	// Did we succeed?
	if (mem)
	{
		// Store header pointer and size
		mem[0]=(ULONG)handle;
		mem[1]=size;

		// Return bumped pointer
		return (void *)(mem+2);
	}

	return 0;
}


// Free some memory allocated with AllocMemH()
void __asm __saveds L_FreeMemH(register __a0 void *memory)
{
	// Check valid pointer
	if (memory)
	{
		register ULONG *mem;
		register MemHandle *handle;

		// Decrement pointer
		mem=((ULONG *)memory)-2;

		// Do we have a memory handle?
		if ((handle=(MemHandle *)mem[0]))
		{
			// See if handle is valid
			if (handle->check_val!=(ULONG)handle)
			{
/*
				KPrintF("** Invalid MemHandle : %lx %lx %ld\n",handle,memory,mem[1]);
*/
			}

			// Assume it is
			else
			{

				// Lock if necessary
				if (handle->flags&MEMHF_LOCK) ObtainSemaphore(&handle->lock);

				// Decrement from allocation total
				handle->total-=mem[1];

				// Is there a memory pool?
				if (handle->pool_header)
				{
					// Use exec routines under 39, lib routines before that
					if (SysBase->LibNode.lib_Version>=39)
						FreePooled(handle->pool_header,mem,mem[1]);
					else
						AsmFreePooled(handle->pool_header,mem,mem[1],(struct Library *)SysBase);
				}

				// Otherwise
				else
				{
					struct MinNode *node;

					// Get node pointer
					node=(struct MinNode *)(mem-2);

					// Remove from memory list
					Remove((struct Node *)node);

					// Free allocation
#ifdef RESOURCE_TRACKING
					NRT_FreeMem(node,mem[1]);
#else
					FreeMem(node,mem[1]);
#endif
				}

				// Unlock if necessary
				if (handle->flags&MEMHF_LOCK) ReleaseSemaphore(&handle->lock);
			}
		}

		// Free with FreeMem()
		else
		{
#ifdef RESOURCE_TRACKING
			NRT_FreeMem(mem,mem[1]);
#else
			FreeMem(mem,mem[1]);
#endif
		}
	}
}
