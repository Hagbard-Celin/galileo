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

/*
 *	27-08-95	Increased password length from 32 to 64 characters
 *	22-09-95	Increased filename length to 256 for Unix
 *	22-09-95	Increased password length to be equal to the address length
 *	20-03-96	Added semaphore to update_info so it can be used asynchronously
 *	07-06-96	Changed FTPQuit template from NOW/S to FORCE/S
 *
 *	08-07-97	added template_old for backward compatibility for systems
 *			which did not specify the PORT in template
 *
 */

#ifndef _GALILEOFTP_H
#define _GALILEOFTP_H

#include "modules.h"
#include "module.h"


#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef _GALILEOFM_GUI_ELEMENT_H
#include "gui_element.h"
#endif
#ifndef _FTP_GLOBALS_H
#include "ftp_globals.h"
#endif

/********************************/

#ifdef DEBUG
#define err(s)		(kprintf(s),kprintf("\n"))
#define errf(s,a)	(kprintf(s,a))
#define errf2(s,a,b)	(kprintf(s,a,b))
#else
#define err(s)
#define errf(s,a)
#define errf2(s,a,b)
#endif

// Not actually defined in dos.h
#define FIBF_HIDDEN    (1<<7)

/* AS225 requires a maximum # of sockets to be set (currently 128 is maximum)*/
#define MAX_AS225_SOCKETS	128

/*
 * the socket libraries need to be opened by each process that uses them
 * SocketBase is stored in each process's userdata field
 */

#define GETGLOBAL(t,v) ((struct globals *)((IPCData*)(t->tc_UserData))->userdata)->v

#define PORTNAME	"_GALILEOFTP_"

#define AMIFILENAMELEN	29				// 30 including null terminator
#define FIBFILENAMELEN	107				// 108 including null terminator
#define HANDLELEN	12				// lister handle

#define SCREENNAMELEN	(MAXPUBSCREENNAME+1)		// Galileo public screen name eg. 'Workbench'
#define TRAPTEMPCMDLEN	16				// Trappable Galileo command eg 'smartread'

#define CONFIG_TEMPLATE		"LIBRARY=LIB/K,LOG/K,LOGOFF/S,\
DEBUG/K/N,TIMEOUT/K/N,LISTUPDATE/K/N,ANONPASS/K,\
SHOWINDEX/S,INDEXSIZE/K/N,AUTOINDEX/S,NOOP/S,\
CONNECTRETRY/S,CONNECTTRIES/K/N,CONNECTRETRYTIMEOUT/K/N,\
NODIRPROGRESS/S"

enum							// offsets for parse template
{
	C_OPT_LIB,
	C_OPT_LOG,
	C_OPT_LOGOFF,
	C_OPT_DBUG,
	C_OPT_TIME,
	C_OPT_UPDT,
	C_OPT_ANONPASS,
	C_OPT_SHOWINDEX,
	C_OPT_INDEXSIZE,
	C_OPT_AUTOINDEX,
	C_OPT_NOOP,
	C_OPT_CONNECT_RETRY,
	C_OPT_CONNECT_TRIES,
	C_OPT_CONNECT_RETRY_TIMEOUT,
	C_OPT_DIRSCAN_PROGRESS,
};

#define ADDR_TEMPLATE		"ANONYMOUS=ANON/S,USERACCOUNT=ACCT/S,ALIAS=NAME/K,HOST,ADDRESS=ADDR/K,PORT/N,PATH=DIR,USER/K,PASSWORD=PASS/K"


enum	// offsets for parse template
{
	A_OPT_ANON,
	A_OPT_ACCT,
	A_OPT_ALIS,
	A_OPT_HOST,
	A_OPT_ADDR,
	A_OPT_PORT,
	A_OPT_PATH,
	A_OPT_USER,
	A_OPT_PASS,
};

#define CONNECT_TEMPLATE	"HOST,PORT/N/K,USER,PASSWORD=PASS,DIR/K,LISTER/N/K,SITE/K,GUI/S,NOSCAN/S,RECON/S"
#define ADDRBOOK_TEMPLATE	0
#if 0
#define CD_TEMPLATE		"DIR/F"
#define COPY_TEMPLATE		0
#define RENAME_TEMPLATE		0
#define DELETE_TEMPLATE		"FILE/F"
#endif
#define COMMAND_TEMPLATE	"QUIET/S,LONG/S,COMMAND/F"
#define SETVAR_TEMPLATE		"VAR,VALUE/F"
#define QUIT_TEMPLATE		"FORCE/S"
#define ADD_TEMPLATE		0
#define OPTIONS_TEMPLATE	"DEFAULT/S"

enum
{
	D_OPT_HOST,
	D_OPT_PORT,
	D_OPT_USER,
	D_OPT_PASS,
	D_OPT_PATH,
	D_OPT_HANDLE,
	D_OPT_SITE,
	D_OPT_GUI,
	D_OPT_NOSCAN,
	D_OPT_RECON,
};

// Module function IDs
enum
{
	ID_ADDRESSBOOK,	// <no template>
	ID_CONNECT,	// HOST,PORT,USER,PASS,DIR/K,LISTER/K/N
	ID_COMMAND,	// COMMAND/F	FTPCommand SYST HELP
	ID_SETVAR,	// VAR,VALUE/F	FTPSetVar LIST NLST -al
	ID_OPTIONS,
	ID_ADD,		// <no template>
	ID_QUIT,	// FORCE/S
};


// msgs flags sent from addressbook to main for changes to logfile

#define	ID_LOGNAME_F	(1 << 0)
#define	ID_LOGON_F	(1 << 1)


//cycle gadget values for copy_type
enum
{
	COPY_TYPE_DEFAULT,
	COPY_TYPE_UPDATE,
	COPY_TYPE_NEWER,
};

//cycle gadget values for replace
enum
{
	COPY_REPLACE_ALWAYS,
	COPY_REPLACE_NEVER,
	COPY_REPLACE_ASK,
};

/******** stuff for gui adressbook */

extern ConfigWindow ftp_main_window,ftp_edit_window,ftp_options_window;
extern ObjectDef ftp_main_objects[],ftp_edit_objects[],ftp_options_objects[];

enum
{
	GAD_BASE,


	GAD_FTP_MAIN_LAYOUT,
	GAD_FTP_SITES,

	GAD_FTP_NEW,
	GAD_FTP_EDIT,
	GAD_FTP_DELETE,
	GAD_FTP_SORT,
	GAD_FTP_SITE_LAYOUT,
	GAD_FTP_SITE_NAME,
	GAD_FTP_HOST_NAME,
	GAD_FTP_PORT,
	GAD_FTP_ANON,
	GAD_FTP_CUSTOM_LAYOUT,
	GAD_FTP_CUSTOM_OPTIONS,
	GAD_FTP_SET_CUSTOM_OPTIONS,
	GAD_FTP_SAVE,
	GAD_FTP_CONNECT,
	GAD_FTP_SET_DEFAULT_OPTIONS,
	GAD_FTP_CANCEL,


	GAD_ENV_ITEM_LISTER,
	GAD_ENV_EDIT_AREA,
	GAD_ENV_SAVE,
	GAD_ENV_RESET_DEFAULT,
	GAD_ENV_CANCEL,


	GAD_ENV_START_UPDATE,	// start marker for gadgets reqd to updating the gui dissplay
				// gadgets between here ane end automatically call update
				// fn to check status of default/reset etc

	GAD_ENV_ENABLE_RETRY,
	GAD_ENV_RETRY_COUNT,
	GAD_ENV_RETRY_DELAY,
	GAD_ENV_ENABLE_RETRY_LOST,
	GAD_ENV_NOOPS,
	GAD_ENV_PASSIVE,


	GAD_ENV_TIMEOUT,
	GAD_ENV_LIST_UPDATE,

	GAD_ENV_KEEP_LAST_DIR,
	GAD_ENV_SPECIAL_DIR,
	GAD_ENV_SAFE_LINKS,
	GAD_ENV_UNK_LINKS,

	GAD_ENV_SHOW_STARTUP_MSG,
	GAD_ENV_SHOW_DIR_MSG,
	GAD_ENV_PROGRESS_WINDOW,
	GAD_ENV_TRANSFER_DETAILS,

	GAD_ENV_SCRIPT_CONNECT_SUCCESS,
	GAD_ENV_SCRIPT_CONNECT_FAIL,
	GAD_ENV_SCRIPT_COPY_SUCCESS,
	GAD_ENV_SCRIPT_COPY_FAIL,
	GAD_ENV_SCRIPT_ERROR,
	GAD_ENV_SCRIPT_CLOSE,
	GAD_ENV_SCRIPT_TIME,

	GAD_ENV_INDEX_ENABLE,

	GAD_ENV_AUTO_DOWNLOAD,
	GAD_ENV_AUTO_DOWNLOAD_SIZE,
	GAD_ENV_LOG_ENABLE,
	GAD_ENV_DEBUG,
	GAD_ENV_LOG_FILE,
	GAD_ENV_ANON_PASSWORD,

	GAD_ENV_TOOLBAR_GLASS,
	GAD_ENV_TOOLBAR,

	GAD_ENV_CUST_FORMAT,
	GAD_ENV_SET_FORMAT,

	GAD_ENV_COPY_TYPE,

	GAD_ENV_COPY_SET_ARCHIVE,
	GAD_ENV_COPY_URL_COMMENT,
	GAD_ENV_COPY_RESCAN,
	GAD_ENV_COPY_RECURSIVE_SPECIAL,

	GAD_ENV_REPLACE,
	GAD_ENV_COPY_GALILEO_DEFAULT,
	GAD_ENV_COPY_DATESTAMP,
	GAD_ENV_COPY_PROTECTION,
	GAD_ENV_COPY_COMMENT,
	GAD_ENV_ADDR_DC,	// addressbook double click
	GAD_ENV_ADDR_AUTO,	// close after DC or connect

	GAD_ENV_END_UPDATE,	// end marker for gadgets reqd to updating the gui dissplay


	GAD_CONNECT_LAYOUT,
	GAD_CONNECT_GLASS,
	GAD_CONNECT_NAME,
	GAD_CONNECT_HOST,
	GAD_CONNECT_ANON,
	GAD_CONNECT_PORT,
	GAD_CONNECT_USER,
	GAD_CONNECT_PASSWORD,
	GAD_CONNECT_DIR,
	GAD_CONNECT_CONNECT,
	GAD_CONNECT_LAST,
	GAD_CONNECT_CANCEL,

	GAD_EDIT_NAME,
	GAD_EDIT_HOST,
	GAD_EDIT_ANON,
	GAD_EDIT_PORT,
	GAD_EDIT_USER,
	GAD_EDIT_PASSWORD,
	GAD_EDIT_DIR,
	GAD_EDIT_OK,
	GAD_EDIT_OPTIONS,
	GAD_EDIT_CANCEL,
	GAD_EDIT_CUSTOM_LAYOUT,
	GAD_EDIT_CUSTOM_OPTIONS,
	GAD_EDIT_SET_CUSTOM_OPTIONS,

	MENU_ENV_OPEN,
	MENU_ENV_SAVE,
	MENU_ENV_SAVEAS,
	MENU_ENV_CANCEL,
	MENU_ENV_RESET_DEFAULTS,
	MENU_ENV_LAST_SAVED,
	MENU_ENV_RESTORE,


	MENU_FTP_OPEN,
	MENU_FTP_IMPORT,
	MENU_FTP_IMPORT_AMFTP,
	MENU_FTP_SAVE,
	MENU_FTP_SAVEAS,
	MENU_FTP_EXPORT,
	MENU_FTP_CANCEL,

	GAD_LIST_SITES,
	GAD_LIST_OK,
	GAD_LIST_CANCEL,

};

enum
{
	// for main lister titles
	ENV_SUB_COPYFLAGS,
	ENV_SUB_COPYATTR,
	ENV_SUB_LISTER,
	ENV_SUB_DISPLAY,
	ENV_SUB_MISC,
	ENV_SUB_SCRIPTS,
	ENV_SUB_INDEX,
	ENV_SUB_CONNECTION,
	ENV_SUB_GLOBAL,
};


/*void kprintf(  char*, ... );*/

#endif
