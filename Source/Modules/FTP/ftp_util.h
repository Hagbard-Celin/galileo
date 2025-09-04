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

#ifndef _OF_UTIL_H
#define _OF_UTIL_H

// Flags for display_msg
#define DSPMSG_RETRYABORT	(1<<0)

// Display a string, with optional Retry/Abort buttons
long display_msg(struct galileoftp_globals *, IPCData *, struct Window *, ULONG flags, char *);

// Get the name of the logged-in user on the Amiga
char *getlogname(char *);

// Get the hostname of the user's Amiga (for anonymous FTPs)
char *getuseraddress(char *);

// Initialize a ListLock and counter
void ad_InitListLock(struct ListLock *, int *count);

// Remove a node from a ListLock
void ListLockRemove(struct ListLock *, struct Node *, int *count);

// Add a node to a ListLock
void ListLockAddHead(struct ListLock *, struct Node *, int *count);

BOOL IsListLockEmpty(struct ListLock *l);

// Copy a string which is locked with a semaphore
void copy_locked_string(struct SignalSemaphore *, char *dest,  char *src,  int len);

// Convert a DateStamp to a number of seconds
ULONG datestamp_to_seconds(struct DateStamp *);

// Convert a number of seconds to a DateStamp
void seconds_to_datestamp(struct DateStamp *, ULONG secs);

// convert the number of seconds since Jan 1 1978 to an AmigaDOS time string
void seconds_to_datestring(ULONG seconds, char *buf);

int setfiledate_secs(char *path, ULONG secs);

ULONG getfiledate_secs(char *path);

ULONG get_curr_secs(void);

char **alloc_name_array(char *initstring);

// Convert value to a string in bytes, K, or megs
void cat_bytes(char *dest, unsigned long value);

// Count the number of quoted entries as passed via ARexx
int count_entries(char *);

// Convert a Unix line output by ls to an entry_info structure
int unix_line_to_entryinfo(struct entry_info *entry, const char *line, ULONG flags);

// Convert a Windows NT line output by ls to an entry_info structure
int nt_line_to_entryinfo(struct entry_info *entry, const char *line, ULONG flags);

// Convert an Amiga fileinfoblock to an entry_info structure
void fileinfoblock_to_entryinfo(struct entry_info *ei, struct FileInfoBlock *fib);

// Get final path from a pathname
void final_path(char *path,char *buf);

// Find an entry in a list
struct entry_info *rec_find_entry(struct rec_entry_list *, struct entry_info *, ULONG cmpflags);

// Convert site details to a standard text URL (not including the ftp:// prefix)
void build_url(char *buffer, const char *user, const char *pass, const char *host, int port, const char *path, const char *entry);

// Split a URL into its component parts
int split_url(const char *url, char *user, char *pass, char *host, long *port, char *path);

// Create a desktop function file
BOOL CreateFunctionFile (char *name, short type, char *instruction, char *icon);

// Generic requester
int ftpmod_request(struct galileoftp_globals *og, IPCData *ipc, Tag tag, ...);

// Tags for ftpmod_request
#define FR_IPC		(TAG_USER + 1000)	// Mainly to override lister IPC
#define FR_FormatString	(TAG_USER + 1001)	// printf-like formatting
#define FR_MsgNum	(TAG_USER + 1002)	// String number from catalog
#define FR_ButtonNum	(TAG_USER + 1003)	// As above for button text

// Check certain Galileo options
unsigned long ftpmod_options(struct galileoftp_globals *, int type);

// Open a temporary file
BPTR open_temp_file(char *filename, IPCData *ipc);

// Count the number of components in a path
int count_path_parts(const char *path);

// Forward an IPCMessage
int ipc_forward(IPCData *, IPCMessage *, ULONG flags);

// Convert Amiga protection flags to Unix style
ULONG prot_amiga_to_unix(LONG bits);

// Convert Unix protection flags to Amiga style
LONG prot_unix_to_amiga(ULONG mode);

// Get protection value from a string
long prot_from_string(char *buf);

#endif
