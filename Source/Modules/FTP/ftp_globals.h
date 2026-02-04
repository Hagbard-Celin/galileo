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

#ifndef _FTP_GLOBALS_H
#define _FTP_GLOBALS_H

#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFM_MODULE
#include "module.h"
#endif

#define HOSTNAMELEN	96				// eg, 'planet.earth'
#define USERNAMELEN	32
#define ADDRESSLEN	(USERNAMELEN+1+HOSTNAMELEN)	// full internet address eg. 'foo@planet.earth'
#define PASSWORDLEN	ADDRESSLEN			// anonymous password is your address
#define LOGNAMELEN	256				// for logfile CON:...
#define COMMENTLEN	79				// 80 including null terminator

#define PATHLEN		256
#define PORTNAMELEN	10				// Galileo Arexx port name eg. 'GALILEO.1'
#define LSCMDLEN	20				// LIST or NLST command + arguments

struct ftp_environment
{
	int	     e_retry_count;				// Number of times to retry a connection
	int	     e_retry_delay;				// Number of seconds to wait before retrying
	int	     e_list_update;				// How often is lister updated during LIST
	int	     e_timeout;
	int	     e_script_time;
	int	     e_indexsize;

	char	     e_toolbar[256];

	unsigned int e_custom_options	    :	    1;

	unsigned int e_retry		    :	    1;		// Retry failed attempt to connect?
	unsigned int e_retry_lost	    :	    1;

	unsigned int e_noops		    :	    1;		// Send noops

	unsigned int e_passive		    :	    1;

	unsigned int e_keep_last_dir	    :	    1;
	unsigned int e_safe_links	    :	    1;
	unsigned int e_unk_links_file	    :	    1;

	unsigned int e_show_startup	    :	    1;
	unsigned int e_show_dir		    :	    1;
	unsigned int e_progress_window	    :	    1;		//Display progress bars while scanning dirs


	unsigned int oldpad		    :	    2;

	unsigned int e_script_connect_ok    :	   1;
	unsigned int e_script_connect_fail  :	 1;
	unsigned int e_script_copy_ok	    :	    1;
	unsigned int e_script_copy_fail	    :	     1;
	unsigned int e_script_error	    :	    1;
	unsigned int e_script_close	    :	    1;

	unsigned int e_index_enable	    :	    1;
	unsigned int e_index_auto	    :	    1;		// If exceeds the limit then skip

	unsigned int e_custom_format	    :	    1;

	unsigned int e_copy_type	    :	    2;		// default/update/newer
	unsigned int e_copy_set_archive	    :	    1;
	unsigned int e_copy_replace	    :	    2;

	unsigned int e_copy_galileo_default :	1;
	unsigned int e_copy_datestamp	    :	    1;
	unsigned int e_copy_protection	    :	    1;
	unsigned int e_copy_comment	    :	    1;

	unsigned int e_url_comment	    :	    1;

	unsigned int e_transfer_details	    :	    1;		//(-ve flag 0=TRUE)
	unsigned int e_rescan		    :	    1;
	unsigned int e_recursive_special    :	   1;		//(recursive copy names with space)
	unsigned int e_special_dir	    :	    1;		// old fred hack env var
	unsigned int pad7		    :	    1;
	unsigned int pad8		    :	    1;

	// IMPORTANT for memcmp comparisions of different structures
	// the ListFormat MUST BE LAST IN STRUCTURE

	ListFormat   e_listformat;
};


/****************************************************************************
 *
 *	NOTE: COPY OF SITE ENTRY STRUCTURES
 *
 *	you cannot memcpy site_entry structures because of internal ptr
 *
 *	ALWAYS Use fn copy_site_entry(og,new,old)
 *
 *****************************************************************************/

struct site_entry
{
	BOOL		       se_anon;
	char		       se_name[HOSTNAMELEN+1];
	char		       se_host[HOSTNAMELEN+1];
	char		       se_user[USERNAMELEN+1];
	char		       se_pass[PASSWORDLEN+1];
	char		       se_path[PATHLEN+1];
	LONG		       se_port;
	BOOL		       se_has_custom_env;	// Only Greg may touch
	struct ftp_environment se_env_private;		// Only Greg may touch
	struct ftp_environment *se_env;			// back -> local or -> global env



	/***************************************************************/
	/********* ^^ do not change size or order above ^^ 	********/
	/********* 	see ftp_addressbook.h			********/
	/***************************************************************/

	UWORD		       se_active_gadget;	// contains 0 or GADGET ID for
							// gadget to be activated on gui
							// from ftp_opuftp.h GAD_CONNECT...

	UBYTE		       se_preserve_format;	// do not call get_site_format when copying
							// site_entry so preserve se_listformat when
							// pasing structure

	UBYTE		       se_pad;

	ListFormat	       se_listformat;		// listerformat to be used as tmp
							// by a lister during connection
};

struct	ftp_config
{
	char		       oc_logname[LOGNAMELEN+1];	/* Buffer for name of log file */
	char		       oc_anonpass[PASSWORDLEN+1];

	unsigned int	       pad1		:	1;	//
	unsigned int	       oc_enable_log	:	1;	// user wants log open on start
	unsigned int	       oc_log_debug	:	1;	// User level debugging (print FTP commands)
	unsigned int	       oc_user_password	:	1;	// user has defined user password

	unsigned int	       oc_addr_auto	:	1;	// auto close addressbook on connect
	unsigned int	       oc_addr_dc	:	2;	// what to do on double click
	unsigned int	       pad8		:	1;

	// IMPORTANT for memcmp comparisions of different structures
	// the environment MUST BE LAST IN STRUCTURE
	struct ftp_environment oc_env;				// default global environment
};

/**
**	Global variables
**/

// 28.1.97 GP og_galileo_ipc NOW IS the REAL IPC for the main Galileo process

struct galileoftp_globals
{
	// Global for all Galileos
	IPCData			  *og_galileo_ipc;	// IPC of Galileo itself
	IPCData			  *og_main_ipc;		// IPC of main FTP process
	int			  og_ftp_launched;	// flag to stop duplicate running of module
	int			  og_noreq;		// TRUE to suppress ALL listers
	char			  *og_socketlibname[2];
	int			  og_socketlibver[2];
	int			  og_socketlib;		// Which socket library are we using?

	struct ListLock		  og_listerlist;	// List of connected FTP listers
	int			  og_listercount;	// Number of connected FTP listers

	IPCData			  *og_addrproc;
	BOOL			  og_addrbook_open;	// So we don't have to queue address book messages

	// Should be seperate for each lister, addressbook (multiple Galileos)
	char			  og_galileoname[PORTNAMELEN + 1];
	struct Screen		  *og_screen;

	CONST GalileoCallbackInfo *og_gci;		// gp

	struct ftp_config	  og_oc;

	Att_List		  *og_SiteList;		// globally accesable addrress book sitelist
	struct SignalSemaphore	  og_SiteList_semaphore;
	int			  og_log_open;		// log file is open

	ListFormat		  og_galileo_format;	// holder for current galileo default lister format

	struct site_entry	  og_last_site;		//last site for connect msg
	BOOL			  og_valid_site;
};


extern struct galileoftp_globals og;

#endif
