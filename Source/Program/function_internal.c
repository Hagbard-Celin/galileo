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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
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
#include "/Modules/modules_protos.h"

extern CONST GalileoCallbackInfo CallBackInfo;

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
	    if (handle && instruction && instruction->ipa_flags&FUNCF_RUN_ASYNC)
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
				 command->function,
				 &CallBackInfo);
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
