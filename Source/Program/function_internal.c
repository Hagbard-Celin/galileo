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

#include "galileofm.h"
#include "/Modules/modules.h"

function_internal_command(CommandList *command,
	                  char *args,
	                  FunctionHandle *handle,
	                  InstructionParsed *instruction)
{
    short ret=1;
    Lister *lister;
    DirBuffer *cust_buffer=0;
    Lister *source_lister=0,*dest_lister=0;
    char custom_port[40];
    PathNode *source_n=0,*dest_n=0;

    // Clear instruction flags
    if (handle) handle->instruction_flags=0;

    // Valid command?
    if (!command || !command->name) return 1;

    // Initialise custom port
    custom_port[0]=0;

    // Got a handle?
    if (handle)
    {
	// Get source and dest nodes
	source_n=function_path_current(&handle->func_source_paths);
	dest_n=function_path_current(&handle->func_dest_paths);

	// Get current source lister
	if (lister=function_lister_current(&handle->func_source_paths))
	{
	    // Custom handler installed?
	    if (lister->cur_buffer->buf_CustomHandler[0])
	    {
		// If there's a NEW flag, we won't be using this lister
		if (instruction && !instruction->ipa_new_arg)
		{
		    // Look for trap handler
		    if (FindFunctionTrap(command->name,lister->cur_buffer->buf_CustomHandler,custom_port))
		    {
			source_lister=lister;
			dest_lister=function_lister_current(&handle->func_dest_paths);
			cust_buffer=lister->cur_buffer;
		    }
		}
	    }
	}

	// If no custom handler, try destination
	if (!cust_buffer &&
	    (command->flags&FUNCF_NEED_DEST) &&
	    (lister=function_lister_current(&handle->func_dest_paths)))
	{
	    // Custom handler installed?
	    if (lister->cur_buffer->buf_CustomHandler[0])
	    {
		// Look for trap handler
		if (FindFunctionTrap(command->name,lister->cur_buffer->buf_CustomHandler,custom_port))
		{
		    source_lister=function_lister_current(&handle->func_source_paths);
		    dest_lister=lister;
		    cust_buffer=lister->cur_buffer;
		}
	    }
	}
    }

    // Custom handler?
    if (cust_buffer)
    {
	char *files=0;
	ULONG flags;

	// Need files?
	if (command->flags&FUNCF_NEED_ENTRIES)
	{
	    // Build file string
	    files=function_build_file_string(handle,
		                             cust_buffer->cust_flags&CUSTF_WANT_QUOTES);
	}

	// Get flags
	flags=RXMF_WARN;
	if (cust_buffer->cust_flags&CUSTF_SYNCTRAPS) flags|=RXMF_SYNC;

	// Send command message
	ret=rexx_handler_msg(custom_port,
		             cust_buffer,
		             flags,
		             HA_String,0,command->name,
		             HA_Value,1,source_lister,
		             HA_String,2,files,
		             HA_Value,3,dest_lister,
		             HA_String,4,(source_n)?source_n->pn_path:0,
		             HA_String,5,args,
		             HA_String,7,(dest_n)?dest_n->pn_path:0,
		             HA_Value,8,(flags&RXMF_SYNC)?handle:0,
		             TAG_END);

	// Free file string
	if (files)
	    FreeVec(files);
	return ret;
    }

    // External command?
    if (command->flags&FUNCF_EXTERNAL_FUNCTION)
    {
	struct Library *ModuleBase;
	char *work_buf=0,*buffer;

	// Need to allocate buffer?
	if (!handle)
	{
	    work_buf=AllocVec(512,MEMF_CLEAR);
	    buffer=work_buf;
	}
	else buffer=handle->func_work_buf;

	// No buffer?
	if (!buffer || strcmp(command->stuff.module_name,"!")==0)
	{
	}

	// User command?
	else
	if (strnicmp(command->stuff.module_name,"PROGDIR:commands/",17)==0)
	{
	    // Shouldn't be able to get here!
	}

	// Rexx script?
	else
	if (sufcmp(command->stuff.module_name,".galileo"))
	{
#ifdef _DEBUG_AREXX_COMMAND
	    KPrintF("Running arexx command lister: %lx \n", (source_n)?source_n->lister:0);
#endif
	    // Get rexx function to run
	    lsprintf(buffer,
		     "PROGDIR:modules/%s %s %s %ld %ld %s",
		     command->stuff.module_name,
		     GUI->rexx_port_name,
		     command->name,
		     (source_n)?source_n->pn_lister:0,
		     (dest_n)?dest_n->pn_lister:0,
		     args);
#ifdef _DEBUG_AREXX_COMMAND
	    KPrintF("Running arexx command %s\n", buffer);
#endif
	    // Run rexx thing
	    rexx_send_command(buffer,TRUE);
	}

	// Open module
	else
	if (ModuleBase=OpenModule(command->stuff.module_name))
	{
	    // Copy arguments
	    strcpy(buffer,args);

	    // Add a newline
	    if (*buffer) strcat(buffer,"\n");
#ifdef _DEBUG
	    KPrintF("function_internal_command instruction->flags: %lx \n", instruction->ipa_flags);
#endif
	    // Async by '&'? Set appropriate flags
	    if ((handle) && (instruction->ipa_flags&FUNCF_RUN_ASYNC))
	    {
		handle->func_parameters.flags|=FUNCF_RUN_ASYNC;
		handle->func_parameters.flags&=~(FUNCF_RELOAD_FILES|FUNCF_RESCAN_SOURCE|FUNCF_RESCAN_DEST);

		// If output to file is on, change to window
		if (handle->func_parameters.flags&FUNCF_OUTPUT_FILE)
		{
		    handle->func_parameters.flags&=~FUNCF_OUTPUT_FILE;
		    handle->func_parameters.flags|=FUNCF_OUTPUT_WINDOW;
		}
	    }

	    // Do we want async
	    if ((handle) && (handle->func_parameters.flags&FUNCF_RUN_ASYNC) && (!(handle->func_flags&FUNCF_SYNC)))
	    {
		AsyncData 	*adata;
		APTR        *memhandlep;

		if (memhandlep=AllocVec(sizeof(APTR), MEMF_CLEAR))
		{
		    if (*memhandlep = NewMemHandle(NULL, NULL, MEMF_CLEAR))
    	            {
	        	if( (adata = AllocMemH(*memhandlep, sizeof(AsyncData))) )
	    	    	{
			    if ( (command->flags&FUNCF_NO_ARGS) || (adata->args=AllocMemH(*memhandlep, strlen(buffer)+1)) )
    	            	    {
				IPCData *myipc=handle->ipc;

				if (adata->args)
				    strcpy(adata->args, buffer);
		    	        else
		    	            adata->args=0;

				adata->screen = GUI->screen_pointer;
				adata->main_ipc	= &main_ipc;
				adata->command = command;
				adata->function_external_hook = (ULONG)function_external_hook;
				adata->module = ModuleBase;
				adata->library = GalileoFMBase;
				adata->memhandlep = memhandlep;
				adata->handle = handle;

	                    	// Call module asynchronous
	                	if (ret=function_internal_async(adata))
				{
				    myipc->flags|=IPCF_DETACHEDCHILD;
				}
				else
		                {
				    FreeMemHandle(*memhandlep);
				    FreeVec(memhandlep);
				}
		    	    }
		            else
			    {
			    	FreeMemHandle(*memhandlep);
				FreeVec(memhandlep);
			    }
			}
			else
			{
			    FreeMemHandle(*memhandlep);
			    FreeVec(memhandlep);
			}
		    }
		    else
	            FreeVec(memhandlep);
		}
	    }
	    else
	    {
		// Call module normally
		ret=Module_Entry((command->flags&FUNCF_NO_ARGS)?0:(struct List *)buffer,
				 GUI->screen_pointer,
				 (handle)?handle->ipc:0,
				 &main_ipc,
				 0,
				 command->function,
				 (ULONG)function_external_hook);
	    }
	    // Close module
	    CloseLibrary(ModuleBase);
	}

	// Free buffer
	if (work_buf)
	    FreeVec(work_buf);
    }

    // Valid code?
    else
    if (command->stuff.code)
	    ret=(int)((int (*)())(command->stuff.code)(command,handle,args,instruction));

    return ret;
}


// Hook for external commands
ULONG __asm __saveds function_external_hook(register __d0 ULONG command,
	                                    register __a0 FunctionHandle *handle,
	                                    register __a1 APTR packet)
{
    // Look at command
    switch (command)
    {
	// Get current source path
	case EXTCMD_GET_SOURCE:
		return (ULONG)HookGetSource(handle,(char *)packet);

	// Get next source path
	case EXTCMD_NEXT_SOURCE:
		return (ULONG)HookNextSource(handle,(char *)packet);

	// Unlock source paths
	case EXTCMD_UNLOCK_SOURCE:
		HookUnlockSource(handle);
		break;

	// Get destination path
	case EXTCMD_GET_DEST:
		return (ULONG)HookGetDest(handle,(char *)packet);

	// Finished with this source
	case EXTCMD_END_SOURCE:
		HookEndSource(handle,(long)packet);
		break;

	// Finished with this destination
	case EXTCMD_END_DEST:
		HookEndDest(handle,(long)packet);
		break;

	// Get next entry
	case EXTCMD_GET_ENTRY:
		return (ULONG)HookGetEntry(handle);

	// End this entry
	case EXTCMD_END_ENTRY:
		HookEndEntry(
			handle,
			((struct endentry_packet *)packet)->entry,
			((struct endentry_packet *)packet)->deselect);
		break;

	// Remove a file
	case EXTCMD_REMOVE_ENTRY:
		HookRemoveEntry((FunctionEntry *)packet);
		break;

	// Get entry count
	case EXTCMD_ENTRY_COUNT:
		return (ULONG)HookEntryCount(handle);

	// Reload a file
	case EXTCMD_RELOAD_ENTRY:
		HookReloadEntry(handle,(FunctionEntry *)packet);
		break;

	// Add a file to a path
	case EXTCMD_ADD_FILE:
		HookAddFile(
			handle,
			((struct addfile_packet *)packet)->path,
			((struct addfile_packet *)packet)->fib,
			((struct addfile_packet *)packet)->lister);
		break;

	// Delete a file in a path
	case EXTCMD_DEL_FILE:
		HookDelFile(
			handle,
			((struct delfile_packet *)packet)->path,
			((struct delfile_packet *)packet)->name,
			((struct delfile_packet *)packet)->lister);
		break;

	// Load a file in a path
	case EXTCMD_LOAD_FILE:
		HookLoadFile(
			handle,
			((struct loadfile_packet *)packet)->path,
			((struct loadfile_packet *)packet)->name,
			((struct loadfile_packet *)packet)->flags,
			((struct loadfile_packet *)packet)->reload);
		break;

	// Perform changes
	case EXTCMD_DO_CHANGES:
		HookDoChanges(handle);
		break;

	// Check abort status
	case EXTCMD_CHECK_ABORT:
		return (ULONG)HookCheckAbort(handle);

	// Get a window handle
	case EXTCMD_GET_WINDOW:
		return (ULONG)HookGetWindow((PathNode *)packet);
		break;

	// Get help on a topic
	case EXTCMD_GET_HELP:
		HookShowHelp(0,(char *)packet);
		break;

	// Get rexx portname
	case EXTCMD_GET_PORT:
		return (ULONG)HookGetPort((char *)packet);

	// Get screen name
	case EXTCMD_GET_SCREEN:
		return (ULONG)HookGetScreen((char *)packet);

	// Get screen data
	case EXTCMD_GET_SCREENDATA:
		return (ULONG)HookGetScreenData();

	// Free screen data
	case EXTCMD_FREE_SCREENDATA:
		HookFreeScreenData(packet);
		break;

	// Open progress indicator
	case EXTCMD_OPEN_PROGRESS:
		HookOpenProgress(
			((struct progress_packet *)packet)->path,
			((struct progress_packet *)packet)->name,
			((struct progress_packet *)packet)->count);
		break;

	// Update progress indicator
	case EXTCMD_UPDATE_PROGRESS:
		HookUpdateProgress(
			((struct progress_packet *)packet)->path,
			((struct progress_packet *)packet)->name,
			((struct progress_packet *)packet)->count);
		break;

	// Close progress indicator
	case EXTCMD_CLOSE_PROGRESS:
		HookCloseProgress(((struct progress_packet *)packet)->path);
		break;

	// Show exists/replace? requester
	case EXTCMD_REPLACE_REQ:
		return (ULONG)HookReplaceReq(
			((struct replacereq_packet *)packet)->window,
			((struct replacereq_packet *)packet)->screen,
			((struct replacereq_packet *)packet)->ipc,
			((struct replacereq_packet *)packet)->file1,
			((struct replacereq_packet *)packet)->file2,
			((struct replacereq_packet *)packet)->default_option);
		break;


	// Get a pointer
	case EXTCMD_GET_POINTER:
		return HookGetPointer((struct pointer_packet *)packet);

	// Free a pointer
	case EXTCMD_FREE_POINTER:
		HookFreePointer((struct pointer_packet *)packet);
		break;


	// Send an ARexx command to Galileo
	case EXTCMD_SEND_COMMAND:
		((struct command_packet *)packet)->rc=
			HookSendCommand(
				handle,
				((struct command_packet *)packet)->command,
				&((struct command_packet *)packet)->result,
				((struct command_packet *)packet)->flags);
		return 1;
    }

    // Unknown command
    return 0;
}
