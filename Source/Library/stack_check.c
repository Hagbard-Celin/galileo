/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993 Günther Röhrich
CopyRight 2025 Hagbard Celine

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
#include "stack_check.h"
#ifdef _DEBUG_STACK


#define WHITE  "\x1B[32m"
#define NORMAL "\x1B[39m"
#define MAXSTRSIZE 80         /* maximum string size */
#define FILLED_UNKNOWN 0
#define FILLED_PATTERN 1
#define FILLED_ZERO    2
#define STATUS_OK              0
#define STATUS_OTHER_STACK     1
#define STATUS_STACK_OVERFLOW  2


void  __asm __saveds L_StackCheckBegin(register __a0 struct Task *check_task, register __a1 StackData *stack_data)
{
    /* local variables to get maximum speed                        */
    ULONG   i;
    ULONG   Free;     /* amount of free stack (in words) */
    UWORD   *SPLower;  /* lower bound of the stack we are checking */
    UWORD   ConstFill = 0xBEEF; /* value to fill the unused stack area */

    if (!check_task || !stack_data)
	return;
    stack_data->Filled = FILLED_UNKNOWN;
    stack_data->StackStatus = STATUS_OK;

    stack_data->SPUpper = (UWORD *)check_task->tc_SPUpper;
    stack_data->GlobalSPLower = (UWORD *)check_task->tc_SPLower;
    stack_data->SPUpperFirst = stack_data->SPUpper;

   /* ensure we still have the same stack area */
   if(stack_data->SPUpper == stack_data->SPUpperFirst)
   {
	stack_data->SPReg = check_task->tc_SPReg;
	SPLower = stack_data->GlobalSPLower;
	if(stack_data->SPReg < SPLower || stack_data->SPReg > stack_data->SPUpper)
	{
	    Forbid();
	    KPrintF("Taskname: %s (0x%08.lx)\n", check_task->tc_Node.ln_Name, check_task);
	    KPrintF("Could not fill unused stack area.\n");
	    KPrintF("Task uses alternative stack or stack already overflowed.\n");
	    Permit();
	    return;
	}
	Free = ((ULONG)stack_data->SPReg - (ULONG)SPLower)/2L;

	for(i=0; i < 8; i++) stack_data->StackBase[i] = SPLower[i];
	/* Let's fill up the unused stack area (if it's not already done) */
	/* we assume that the unused area contains 0 bytes                */
	if((SPLower[0] == 0) && (SPLower[1] == 0) && (Free > 2))
	{
	    i=0;
	    stack_data->Filled = FILLED_ZERO;
	    while((SPLower[i] == 0) && (i < Free)) SPLower[i++] = ConstFill;
	}

	/* if the unused area is filled with an unknown pattern,      */
	/* let's fill it with our own pattern                         */
	/* WARNING: by doing that UsedMax becomes inaccurate          */
	else if((SPLower[0] != ConstFill) || (SPLower[1] != ConstFill))
	{
	    for(i = 0; i < Free; i++) SPLower[i] = ConstFill;
	}
	else stack_data->Filled = FILLED_PATTERN;
    }
}

void  __asm __saveds L_StackCheckEnd(register __a0 struct Task *check_task, register __a1 StackData *stack_data)
{
    /* local variables to get maximum speed                        */
    /* this is needed to increase performance because Aztec C does */
    /* not put global variables and constants into registers       */
    /* NOTE: GNU C does it when using -O2 option                   */
    ULONG   i;
    ULONG   Free;     /* amount of free stack (in words) */
    UWORD   *SPLower;  /* lower bound of the stack we are checking */
    UWORD   ConstFill = 0xBEEF; /* value to fill the unused stack area */

    if (!check_task || !stack_data)
	return;

    stack_data->SPUpper = (UWORD *)check_task->tc_SPUpper;
    stack_data->GlobalSPLower = (UWORD *)check_task->tc_SPLower;

    /* ensure we still have the same stack area */
    if(stack_data->SPUpper == stack_data->SPUpperFirst)
    {
	stack_data->SPReg = (UWORD *)check_task->tc_SPReg;
	SPLower = stack_data->GlobalSPLower;
	Free = ((ULONG)stack_data->SPUpper - (ULONG)SPLower) / 2;

	for(i=0; i < 4; i++) stack_data->LongStackBase[i] = *(ULONG *)&SPLower[i*2];
	/* count the undamaged area from SPLower to SPUpper   */
	for(i = 0; i < Free; i++) if(SPLower[i] != ConstFill) break;
	stack_data->UsedMax = (ULONG)stack_data->SPUpper - (ULONG)SPLower - i * 2;

	if((stack_data->SPReg < SPLower || stack_data->SPReg > stack_data->SPUpper) && stack_data->StackStatus == STATUS_OK)
	       stack_data->StackStatus = STATUS_OTHER_STACK;
	if(stack_data->UsedMax == Free * 2) stack_data->StackStatus = STATUS_STACK_OVERFLOW;

	Forbid();
	KPrintF("\n\n             **** STACK CHECK ***\n");
	KPrintF("Taskname: %s (0x%08.lx)\n", check_task->tc_Node.ln_Name, check_task);
	switch(stack_data->Filled)
	{
	  case FILLED_UNKNOWN: KPrintF("Free stack area contained unknown pattern:\n");
						       break;
	  case FILLED_PATTERN: KPrintF("Free stack area contained known pattern:\n");
						       break;
	  case FILLED_ZERO   : KPrintF("Free stack area was cleared:\n");
	}
	KPrintF("0x%08.lx: ", stack_data->GlobalSPLower);
	for(i=0; i < 8; i = i + 2)
	    KPrintF("0x%04.lx%04.lx ", stack_data->StackBase[i], stack_data->StackBase[i+1]);
	KPrintF("\n");
	switch(stack_data->StackStatus)
	{
		case STATUS_OK:
		            KPrintF("Stack OK:\n");
		            break;

		case STATUS_OTHER_STACK:
			    KPrintF(WHITE "Task used alternative stack!\n" NORMAL);
		            KPrintF("Status of normal stack area:\n");
		            break;

		case STATUS_STACK_OVERFLOW:
			    KPrintF(WHITE  "Task stack overflowed:\n" NORMAL);
	}
	KPrintF("0x%08.lx: ", stack_data->GlobalSPLower);
	for(i=0; i < 4; i++)
	KPrintF("0x%08.lx ", stack_data->LongStackBase[i]);
	KPrintF("\n");

	#ifdef DEBUG
	KPrintF("SPReg:   %lX\n", stack_data->SPReg);
	KPrintF("SPUpper: %lX\n", stack_data->SPUpper);
	KPrintF("SPLower: %lX\n", SPLower);
	#endif
	KPrintF("Stacksize: %lu\n", (ULONG)stack_data->SPUpper - (ULONG)SPLower);
	if(stack_data->StackStatus != STATUS_STACK_OVERFLOW) KPrintF("Used max:  %lu\n", stack_data->UsedMax);
	KPrintF("                 ********************\n\n");
	Permit();
    }
    else
    {
	Forbid();
	KPrintF("\n\n             **** STACK CHECK ***");
	KPrintF("Taskname: %s (0x%08.lx)\n Stack not found??????\n", check_task->tc_Node.ln_Name, check_task);
	KPrintF("                 ********************\n\n");
	Permit();
    }
}

#endif
