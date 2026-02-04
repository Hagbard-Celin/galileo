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

#include "galileofm.h"
#include <gfm/cli_launch.h>
#include <gfm/function_flags.h>
#include "function_launch_protos.h"
#include "misc_protos.h"
#include "function_protos.h"
#include "rexx_protos.h"
#include "commands.h"
#include "lsprintf_protos.h"
#include "eliza.h"

typedef struct
{
	BPTR			old_output;
	BPTR			old_input;
	struct MsgPort	*old_console;
	BPTR			output;
	BPTR			input;
	char			*name;
	struct MsgPort	*reply_port;
} CLIData;

void print_string(char *);
BOOL cli_open(CLIData *);
void cli_close(CLIData *);
void cli_free(CLIData *);

// Internal command line interpreter
GALILEOFM_FUNC(function_cli)
{
	CLIData data;
	short pos=0;
	char ch;
	CommandList *last_cmd=0;
    struct eliza_data *edata;
    short eliza_state=0;

	// Output filename
	lsprintf(handle->temp_buffer,
		"%s0/%ld/512/150/Galileo CLI/CLOSE/SCREEN %s",
		environment->env->output_device,
		(GUI->screen_pointer)?GUI->screen_pointer->BarHeight+1:20,
		get_our_pubscreen());

	// Allocate name copy
	if (!(data.name=AllocVec(strlen(handle->temp_buffer)+1,0)))
		return 0;

	// Copy name
	strcpy(data.name,handle->temp_buffer);

	// Create reply port for functions
	if (!(data.reply_port=CreateMsgPort()))
	{
		cli_free(&data);
		return 0;
	}

	// Open CLI file
	if (!(cli_open(&data)))
	{
		cli_free(&data);
		return 0;
	}

	// Output information
	print_string(
		"\nGalileo Command Line Interpreter v0.04\n"
		"© Copyright 1998 by Jonathan Potter\n\n");
	print_string(GetString(&locale,MSG_CLI_TYPE_HELP));

	// Output prompt string
	print_string("> ");

	// Initialise Eliza
	if ((edata=AllocVec(sizeof(struct eliza_data),MEMF_CLEAR)) &&
		init_eliza(edata)) eliza_state=1;

	// Wait for characters
	while (1)
	{
		BOOL ok=1;
		IPCMessage *msg;

		// Any messages?
		while (msg=(IPCMessage *)GetMsg(handle->ipc->command_port))
		{
			// Quit?
			if (msg->command==IPC_ABORT || msg->command==IPC_QUIT) ok=0;

			// Hide?
			else
			if (msg->command==IPC_HIDE && data.input) cli_close(&data);

			// Show?
			else
			if (msg->command==IPC_SHOW && !data.input)
			{
				if (!(cli_open(&data))) ok=0;
				else print_string("\n> ");
			}

			// Reply to message
			IPC_Reply(msg);
		}

		// Quit?
		if (!ok) break;

		// Input handle?
		if (data.input)
		{
			// Wait for characters
			ok=WaitForChar(data.input,500000);

			// Pending quit?
			if (GUI->flags&GUIF_PENDING_QUIT)
				break;

			// If no characters waiting, loop around
			if (!ok) continue;
		}

		// Otherwise
		else
		{
			// Wait for a message
			WaitPort(handle->ipc->command_port);
			continue;
		}

		// Read character
		if (Read(data.input,&ch,1)!=1)
		{
			// End of file
			break;
		}

		// End of line?
		if (ch=='\n')
		{
			// Null-terminate buffer
			handle->func_work_buf[pos]=0;

			// Anything in buffer?
			if (pos>0 || last_cmd)
			{
				BOOL wait_reply=1;
				char *ptr;

				// Response to a command?
				if (last_cmd)
				{
					// Copy input string to temporary area
					strcpy(handle->func_work_buf+512,handle->func_work_buf);

					// Build command string
					lsprintf(handle->func_work_buf,"%s %s",last_cmd->name,handle->func_work_buf+512);
				}

				// Quit?
				if (stricmp(handle->func_work_buf,"quit")==0)
					break;

				// Fun
				if (stricmp(handle->func_work_buf,"joshua")==0)
				{
					print_string("Greetings Professor Falken.\n");
				}

				// In eliza mode?
				else
				if (eliza_state==2)
				{
					// Pass the line to Eliza
					if (!eliza_line(edata,handle->func_work_buf))
					{
						// Not in eliza any more
						eliza_state=1;
					}
				}

				// Otherwise, go into Eliza?
				else
				if (eliza_state==1 &&
					stricmp(handle->func_work_buf,"help me eliza!")==0)
				{
					print_string("How can I help you?\n\n");
					eliza_state=2;
				}

				// Help?
				else
				if (strnicmp(handle->func_work_buf,"help",4)==0)
				{
					CommandList *command;

					// Get pointer to word after "help"
					ptr=handle->func_work_buf+4;
					rexx_skip_space(&ptr);

					// Lock command list
					lock_listlock(&GUI->command_list,FALSE);

					// Match internal command?
					if (command=function_find_internal(&ptr,0))
					{
						// Template?
						if (command->template && *command->template)
							print_string(command->template);
						else print_string(GetString(&locale,MSG_CLI_NO_TEMPLATE));
						print_string("\n");
					}

					// Otherwise, show list of commands?
					else
					if (stricmp(ptr,"list")==0)
					{
						for (command=(CommandList *)GUI->command_list.list.lh_Head;
							command->node.mln_Succ;
							command=(CommandList *)command->node.mln_Succ)
						{
							// Not private?
							if (!(command->flags&FUNCF_PRIVATE))
							{
								lsprintf(handle->func_work_buf,"%s",command->name);
								if (command->flags&FUNCF_EXTERNAL_FUNCTION)
								{
									strcat(handle->func_work_buf," (");
									if (strcmp(command->stuff.module_name,"!")==0)
										strcat(handle->func_work_buf,"temp");
									else
										strcat(handle->func_work_buf,command->stuff.module_name);
									strcat(handle->func_work_buf,")");
								}
								strcat(handle->func_work_buf,"\n");
								print_string(handle->func_work_buf);
							}
						}
					}

					// General help
					else
					if (!*ptr)
					{
						print_string(GetString(&locale,MSG_CLI_HELP));
					}

					// Unknown command
					else
					{
						print_string(GetString(&locale,MSG_CLI_BAD_CMD));
					}

					// Unlock command list
					unlock_listlock(&GUI->command_list);

					// Output a blank line
					print_string("\n");
				}

				// Otherwise
				else
				{
					CommandList *cmd;

					// Asynchronous function?
					if (handle->func_work_buf[pos-1]=='&')
					{
						// Yep
						handle->func_work_buf[pos-1]=0;
						wait_reply=0;
					}

					// Get pointer to start of string, bump past whitespaces
					ptr=handle->func_work_buf;
					rexx_skip_space(&ptr);

					// Rexx message?
					if (handle->func_work_buf[0]=='+')
					{
						// Got ARexx?
						if (RexxSysBase)
						{
							struct RexxMsg *msg;
							struct MsgPort *rexx_port;
							BOOL free=1;

							// Allocate message
							if (msg=BuildRexxMsgExTags(
								data.reply_port,0,0,
								RexxTag_Arg0,handle->func_work_buf+1,
								TAG_END))
							{
								// Asynchronous?
								if (!wait_reply) msg->rm_Node.mn_ReplyPort=0;
								else msg->rm_Action|=RXFF_RESULT;

								// Find rexx port
								Forbid();
								if (rexx_port=FindPort(GUI->rexx_port_name))
								{
									// Send message
									PutMsg(rexx_port,(struct Message *)msg);
									Permit();

									// Wait for reply?
									if (wait_reply)
									{
										// Get reply
										WaitPort(data.reply_port);
										GetMsg(data.reply_port);

										// Got result string?
										if (msg->rm_Result2)
										{
											// Output result
											print_string("--> ");
											print_string((char *)msg->rm_Result2);

											// Free result
											DeleteArgstring((STRPTR)msg->rm_Result2);
										}

										// Output result code
										else
										{
											char buf[16];

											// Build result string
											lsprintf(buf,"RC: %ld",msg->rm_Result1);
											print_string(buf);
										}

										// Add a blank line
										print_string("\n\n");
									}

									// Otherwise don't free message
									else free=0;
								}
								else Permit();

								// Free the message
								if (free) FreeRexxMsgEx(msg);
							}
						}
					}

					// Otherwise, internal function?
					else
					if (cmd=function_find_internal(&ptr,0))
					{
						Cfg_Function *function;

						// Display template?
						if (ptr && *ptr=='?' && *(ptr+1)==0 && !last_cmd)
						{
							// Print template if valid
							if (cmd->template) print_string(cmd->template);
							print_string(": ");

							// Save command
							last_cmd=cmd;
						}

						// Create dummy function
						else
						if (function=new_default_function(handle->func_work_buf,handle->memory))
						{
							struct Message reply_msg;

							// Clear 'last command' pointer
							last_cmd=0;

							// Set flag to free function
							function->function.flags2|=FUNCF2_FREE_FUNCTION;

							// Initialise reply message
							reply_msg.mn_ReplyPort=data.reply_port;

							// Execute function
							function_launch(
								FUNCTION_RUN_FUNCTION,
								function,
								0,0,
								0,0,
								0,0,
								0,0,
								0,
								(wait_reply)?&reply_msg:0,0);

							// Wait for reply?
							if (wait_reply)
							{
								// Get reply
								WaitPort(data.reply_port);
								GetMsg(data.reply_port);
							}
						}

						// Clear 'last command' pointer
						else last_cmd=0;
					}

					// Otherwise pass to dos
					else
					{
						CLI_Launch(
							handle->func_work_buf,
							(struct Screen *)-1,
							0,
#ifdef RESOURCE_TRACKING
							NRT_Open("console:",MODE_OLDFILE),
#else
							Open("console:",MODE_OLDFILE),
#endif
							0,
							wait_reply|LAUNCHF_USE_STACK,
							environment->env->default_stack, NULL);
					}
				}

				// Restart buffer
				pos=0;
			}

			// Output prompt string
			if (!last_cmd) print_string("> ");
		}

		// Store in buffer (unless full)
		else
		if (pos<255) handle->func_work_buf[pos++]=(char)ch;
	}

	// Clean up
	cli_close(&data);
	cli_free(&data);
	if (edata)
	{
		free_eliza(edata);
		FreeVec(edata);
	}

	return 0;
}

void print_string(char *str)
{
	PutStr(str);
	Flush(Output());
}


// Open CLI
BOOL cli_open(CLIData *data)
{
	// Initialise
	data->old_output=Output();
	data->old_input=Input();
	data->old_console=GetConsoleTask();
	data->input=0;

	// Try and open console window
	if (!(data->output=Open(data->name,MODE_NEWFILE)))
		return 0;

	// Make this the output channel
	SelectOutput(data->output);

	// Make this the console handler
	SetConsoleTask(((struct FileHandle *)BADDR(data->output))->fh_Type);

	// Duplicate output channel for input
	if (!(data->input=Open("console:",MODE_OLDFILE)))
	{
		Close(data->output);
		data->output=0;
		return 0;
	}

	// Set input channel
	SelectInput(data->input);

	return 1;
}


// Close CLI
void cli_close(CLIData *data)
{
	// Set things back the way they were
	SelectOutput(data->old_output);
	SelectInput(data->old_input);
	SetConsoleTask(data->old_console);

	// Close input/output channels
	Close(data->input);
	Close(data->output);

	// Clear handles
	data->input=0;
	data->output=0;
}


// Free CLI
void cli_free(CLIData *data)
{
	DeleteMsgPort(data->reply_port);
	FreeVec(data->name);
}
