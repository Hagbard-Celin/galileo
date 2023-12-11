#include "example.h"
#include <string.h>

// Define the version string here
static char *version="$VER: example.module 1.0 (01.08.96)";

// Main entry point to the module. The L_ is to identify this as a library
// function (specified by the "libprefix" option in the makefile)
int __asm __saveds L_Module_Entry(
	register __a0 char *args,				// User-supplied arguments
	register __a1 struct Screen *screen,	// Screen to open on
	register __a2 IPCData *ipc,				// Our IPC pointer
	register __a3 IPCData *main_ipc,		// Main Galileo IPC pointer
    register __a4 APTR *memhandlep,         // Async? See Documentation/Module-Changes.txt
	register __d0 ULONG mod_id,				// ID of module function
	register __d1 EXT_FUNC(func_callback))	// Galileo callback function
{
	char buf[256];

	// Build requester text
	strcpy(buf,GalileoGetString(locale,MSG_MESSAGE));

	// If arguments were supplied, tack them on the end
	if (args && *args)
	{
		// Add args to end of buffer
		strcat(buf,"\nArgs: ");
		strcat(buf,args);

		// The argument string is supplied with a newline character for use
		// with ReadArgs().. we strip it for the requester
		buf[strlen(buf)-1]=0;
	}

	// Display requester
	AsyncRequestTags(
		ipc,
		REQTYPE_SIMPLE,
		0,
		0,
		0,
		AR_Screen,screen,
		AR_Message,buf,
		AR_Title,GalileoGetString(locale,MSG_TITLE),
		AR_Button,GalileoGetString(locale,MSG_OK),
		TAG_END);

	return 1;
}
