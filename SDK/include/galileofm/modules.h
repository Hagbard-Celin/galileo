#ifndef _GALILEOFM_MODULES
#define _GALILEOFM_MODULES

/****************************************************************************

 Support file for Galileo modules

 ****************************************************************************/

#ifndef _GALILEOFMBASE
#include <galileofm/galileofmbase.h>
#endif


// Defines a function in a module
typedef struct
{
	ULONG	id;		// Function ID code
	char	*name;          // Function name
	ULONG	desc;           // Locale string ID for function description
	ULONG	flags;		// Function flags
	char	*template;	// Command template
} ModuleFunction;

// Function flags
#define FUNCF_NEED_SOURCE		(1<<0)	// Needs a source directory
#define FUNCF_NEED_DEST			(1<<1)	// Needs a destination directory
#define FUNCF_NEED_FILES		(1<<2)	// Needs some files to work with
#define FUNCF_NEED_DIRS			(1<<3)	// Needs some files to work with
#define FUNCF_NEED_ENTRIES	(FUNCF_NEED_FILES|FUNCF_NEED_DIRS)
#define FUNCF_CAN_DO_ICONS		(1<<6)	// Function can do icons
#define FUNCF_SINGLE_SOURCE		(1<<8)	// Only a single source needed
#define FUNCF_SINGLE_DEST		(1<<9)	// Only a single destination needed
#define FUNCF_WANT_DEST			(1<<11)	// Want destinations, don't need them
#define FUNCF_WANT_SOURCE		(1<<12)	// Want source, don't need it
#define FUNCF_WANT_ENTRIES		(1<<19)	// Want entries
#define FUNCF_SYNC			(1<<24)	// Function wants to be synchronous
#define FUNCF_PRIVATE			(1<<26) // Function is private


// Defines all the functions in a module
typedef struct
{
	ULONG		ver;		// Module version
	char		*name;		// Module name
	char		*locale_name;	// Catalog name
	ULONG		flags;		// Module flags
	ULONG		function_count;	// Number of functions in module
	ModuleFunction	function[1];	// Definition of first function
} ModuleInfo;


/*** If the module has more than one function, the additional ModuleFunction
     structures MUST follow the ModuleInfo structure in memory. Eg,

     ModuleInfo module_info={....};
     ModuleFunction more_funcs[2]={{...},{...}};                           ***/


// Flags for ModuleInfo
#define MODULEF_CALL_STARTUP		(1<<0)	// Call ModuleEntry() on startup
#define MODULEF_STARTUP_SYNC		(1<<1)	// Run Synchronously on startup
#define MODULEF_CATALOG_VERSION		(1<<2)	// Use version for catalog
#define MODULEF_HELP_AVAILABLE		(1<<3)	// Command help is available

// ID passed to Module_Entry() if module is run on startup
#define FUNCID_STARTUP			0xffffffff

#define FUNC_IS_ASYNC			(1<<4) // running asynchronously

#define FUNCASYNC(ipc)			(ipc->flags&FUNC_IS_ASYNC)


/*****************************************************************************

 Galileo Callbacks

 *****************************************************************************/

// For .gfmmodules the address fo the callbacks is supplied to Module_Entry().
// If you are not a module, you can use this command to get the address of the
// hooks from the Galileo process. Send it to the main Galileo IPC, and supply
// a poiinter to a pointer where you will recive the address of the
// GalileoCallbackInfo structure.

#define HOOKCMD_GET_CALLBACKS	0x40100

#define GALILEOFM_CALLBACK_COUNT    56

// This structure will grow in the future, correct use of the gc_Count field
// ensures innocent memory won't get overwritten.

typedef struct
{
	// Set to the number of hooks you want
	ULONG		gc_Count;

	// Create a file entry
	APTR __asm	(*gc_CreateFileEntry)
				(register __a0 ULONG lister,
				 register __a1 struct FileInfoBlock *fib,
				 register __d0 BPTR lock);

	// Change parameters of a file entry
	VOID __asm	(*gc_FileSet)
				(register __a0 ULONG lister,
				 register __a1 APTR entry,
				 register __a2 struct TagItem *tags);

	// Sort list of entries into a lister
	VOID __asm	(*gc_SortFileList)
				(register __a0 ULONG lister,
				 register __a1 struct List *list,
				 register __d0 long file_count,
				 register __d1 long dir_count);

	// Add single entry to a lister
	APTR __asm	(*gc_AddFileEntry)
				(register __a0 ULONG lister,
				 register __a1 APTR entry,
				 register __d0 BOOL sort);

	// Resort a lister
	VOID __asm	(*gc_ResortLister)
				(register __a0 ULONG lister,
				 register __a1 struct ListFormat *format);

	// Refresh a lister
	VOID __asm	(*gc_RefreshLister)
				(register __a0 ULONG lister,
				 register __d0 ULONG flags);

	// Lock lister file list
	VOID __asm	(*gc_LockFileList)
				(register __a0 ULONG lister,
				 register __d0 BOOL exclusive);

	// Unlock file list
	VOID __asm	(*gc_UnlockFileList)
				(register __a0 ULONG lister);

	// Find entry in a lister by name
	APTR __asm	(*gc_FindFileEntry)
				(register __a0 ULONG lister,
				 register __a1 char *name);

	// Change comment of an entry
	BOOL __asm	(*gc_SetFileComment)
				(register __a0 ULONG lister,
				 register __a1 char *name,
				 register __a2 char *comment);

	// Remove file entry from a lister
	VOID __asm	(*gc_RemoveFileEntry)
				(register __a0 ULONG lister,
				 register __a1 APTR entry);

	// Query file entry
	BOOL __asm	(*gc_FileQuery)
				(register __a0 ULONG lister,
				 register __a1 APTR entry,
				 register __a2 struct TagItem *tags);

	// Show help
	void __asm	(*gc_ShowHelp)
				(register __a0 char *file_name,
				 register __a1 char *node_name);


	// Convert entry pointer from one type to another
	APTR __asm	(*gc_ConvertEntry)
				(register __a0 APTR entry);


	// Get lister pointer from a path handle
	ULONG __asm	(*gc_GetLister)
				(register __a0 APTR path);


	// Get first source lister
	APTR __asm 	(*gc_GetSource)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// Get next source lister
	APTR __asm	(*gc_NextSource)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// Unlock source listers
	void __asm	(*gc_UnlockSource)
				(register __a0 APTR handle);


	// Get next destination lister
	APTR __asm	(*gc_GetDest)
				(register __a0 APTR handle,
				 register __a1 char *path);


	// End use of source lister
	void __asm	(*gc_EndSource)
				(register __a0 APTR handle,
				 register __d0 long complete);


	// End use of destination lister
	void __asm	(*gc_EndDest)
				(register __a0 APTR handle,
				 register __d0 long complete);


	// Get next selected entry
	APTR __asm	(*gc_GetEntry)
				(register __a0 APTR handle);


	// Examine an entry
	ULONG __asm	(*gc_ExamineEntry)
				(register __a0 APTR entry,
				 register __d0 long type);


	// End use of an entry
	void __asm	(*gc_EndEntry)
				(register __a0 APTR handle,
				 register __a1 APTR entry,
				 register __d0 BOOL deselect);


	// Mark an entry for removal
	void __asm	(*gc_RemoveEntry)
				(register __a0 APTR entry);


	// Return count of selected entries
	long __asm	(*gc_EntryCount)
				(register __a0 APTR handle);


	// Mark an entry to be reloaded
	void __asm	(*gc_ReloadEntry)
				(register __a0 APTR handle,
				 register __a1 APTR entry);


	// Add a file to a lister
	void __asm	(*gc_AddFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 struct FileInfoBlock *fib,
				 register __a3 ULONG lister);


	// Delete a file from a lister
	void __asm	(*gc_DelFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 char *name,
				 register __a3 ULONG lister);

	// Load/reload a file in a lister
	void __asm	(*gc_LoadFile)
				(register __a0 APTR handle,
				 register __a1 char *path,
				 register __a2 char *name,
				 register __d0 long flags,
				 register __d1 BOOL reload);


	// Perform changes on a lister
	void __asm	(*gc_DoChanges)
				(register __a0 APTR handle);


	// Check for user abort
	BOOL __asm	(*gc_CheckAbort)
				(register __a0 APTR handle);


	// Get window pointer from a path handle
	struct Window *__asm	(*gc_GetWindow)
				(register __a0 APTR path);


	// Get Galileo ARexx port name
	struct MsgPort *__asm	(*gc_GetPort)
				(register __a0 char *ame);


	// Get Galileo public screen name
	struct Screen *__asm	(*gc_GetScreen)
				(register __a0 char *name);


	// Get information about the Galileo screen
	struct GalileoScreenData *__asm	(*gc_GetScreenData)(void);


	// Free screen data structure
	void __asm	(*gc_FreeScreenData)(void);


	// Open progress indicator in a lister
	void __asm	(*gc_OpenProgress)
				(register __a0 APTR path,
				 register __a1 char *operation,
				 register __d0 long total);


	// Update progress indicator
	void __asm	(*gc_UpdateProgress)
				(register __a0 APTR path,
				 register __a1 char *name,
				 register __d0 long count);


	// Close progress indicator
	void __asm	(*gc_CloseProgress)
				(register __a0 APTR path);


	// Show 'File exists - Replace?' requester
	long __asm	(*gc_ReplaceReq)
				(register __a0 struct Window *window,
				 register __a1 struct Screen *screen,
				 register __a2 IPCData *ipc,
				 register __a3 struct FileInfoBlock *file1,
				 register __a4 struct FileInfoBlock *file2,
				 register __d0 long flags);


	// Get pointer to Galileo internal things
	APTR __asm	(*gc_GetPointer)
				(register __a0 struct GetPointerPkt *);
	void __asm	(*gc_FreePointer)
				(register __a0 struct GetPointerPkt *);


	// Send an ARexx command direct to Galileo
	ULONG __asm	(*gc_SendCommand)
				(register __a0 APTR handle,
				 register __a1 char *command,
				 register __a2 char **result,
				 register __d0 ULONG flags);


	// Make Galileo check if the desktop needs updating
	void __asm	(*gc_CheckDesktop)
				(register __a0 char *path);


	// Get desktop path
	short __asm	(*gc_GetDesktop)
				(register __a0 char *buffer);

	// Run script
	short __asm	(*gc_Script)
				(register __a0 char *name,
				 register __a1 char *data);

	// Popup desktop popup
	short __asm	(*gc_DesktopPopup)
				(register __a0 ULONG flags);

	// Reset file entries
	void __asm	(*gc_FirstEntry)
				(register __a0 APTR handle);

	// Send rexx command direct to Galileo
	long __asm	(*gc_RexxCommand)
				(register __a0 char *command,
				 register __a1 char *result,
				 register __d0 long length,
				 register __a2 struct MsgPort *replyport,
				 register __d1 ULONG flags);

	// Show file requester
	long __asm	(*gc_FileRequest)
				(register __a0 struct Window *parent,
				 register __a1 char *title,
				 register __a2 char *initial_path,
				 register __a3 char *pathname,
				 register __d0 ULONG flags,
				 register __d1 char *pattern);

	// Get themes path
	void __asm	(*gc_GetThemes)
				(register __a0 char *buffer);

	// Free pointer without struct pointer_packet
	void __asm  (*gc_FreePointerDirect)
				(register __a0 APTR pointer,
	    			 register __d0 ULONG type,
	    			 register __d1 ULONG flags);

	// Check if lister source/dest status is locked
	BOOL __asm  (*gc_IsSourceDestLock)
				(register __a0 ULONG lister);

	void __asm  (*gc_FakeDir)
		       	        (register __a0 ULONG lister,
				 register __d0 BOOL fakedir);

	BOOL __asm  (*gc_IsFakeDir)
				(register __a0 ULONG lister);

} GalileoCallbackInfo;


// Values for gc_FileSet/gc_FileQuery

#define HFFS_NAME		( TAG_USER + 0x1 )	// char * 		- Entry name
#define HFFS_SIZE		( TAG_USER + 0x2 )	// ULONG 		- Entry size
#define HFFS_PROTECTION		( TAG_USER + 0x3 )	// ULONG 		- Protection flags
#define HFFS_DATE		( TAG_USER + 0x4 )	// struct Datestamp *	- Entry date
#define HFFS_COMMENT		( TAG_USER + 0x5 )	// char *		- Comment
#define HFFS_SELECTED		( TAG_USER + 0x6 )	// BOOL			- Selected state
#define HFFS_LINK		( TAG_USER + 0x7 )	// BOOL			- Set if a link
#define HFFS_COLOUR		( TAG_USER + 0x8 )	// ULONG		- 1 = device, 2 = assign
#define HFFS_USERDATA		( TAG_USER + 0x9 )	// ULONG		- Userdata
#define HFFS_FILETYPE		( TAG_USER + 0xa )	// char *		- Filetype description
#define HFFS_DISPLAY		( TAG_USER + 0xb )	// char *		- Custom display string
#define HFFS_VERSION		( TAG_USER + 0xc )	// VersionInfo *	- Version information
#define HFFS_MENU		( TAG_USER + 0xd )	// struct List *	- Custom menus for entry
#define HFFS_ICON		( TAG_USER + 0xe )	// struct DiskObject *	- not implemented


// Additional file requester flags
#define GCRFF_PATTERN		(1<<29)
#define GCRFF_SCREENPARENT	(1<<30)
#define GCRFF_REJECTICONS	(1<<31)

#ifndef _GALILEOFM_DIRLIST
// Holds version information (used by HFFS_VERSION)
typedef struct _VersionInfo {
	UWORD		vi_Version;		// Major version #
	UWORD		vi_Revision;		// Minor revision #
	long		vi_Days;		// Days of date
	ULONG		vi_Flags;		// Flags
	char		vi_Char;		// Version character
	char		vi_String[1];		// Version string
} VersionInfo;
#endif


// Flags for gc_RefreshLister()
#define HOOKREFRESH_DATE	(1<<0)
#define HOOKREFRESH_FULL	(1<<1)



// Type value for gc_ExamineEntry()
enum
{
	EE_NAME,		// Get pointer to name (READ ONLY!)
	EE_TYPE,		// Get type (<0 = file, >0 = dir)
};


// Flags for gc_ReplaceReq()
#define REPREQF_NOVERSION	(1<<16)		// No 'version' button


// Define this if you want to use gc_ResortLister
#ifdef	GALILEOFMHOOK_INCLUDE_FORMAT



// Sort format
struct SortFormat {
	BYTE	sf_Sort;			// Sort method
	BYTE	sf_SortFlags;			// Sort flags
	BYTE	sf_Separation;			// File separation
};

#define SORT_REVERSE			(1<<0)	// Sort in reverse order

// List format
struct ListFormat {

	// Colour fields, not used at present
	UBYTE			lf_FilesUnsel[2];	// Unselected files
	UBYTE			lf_FilesSel[2];		// Selected files
	UBYTE			lf_DirsUnsel[2];	// Unselected directories
	UBYTE			lf_DirsSel[2];		// Selected directories

	// Sort information
	struct SortFormat	lf_Sort;		// Sort method
	BYTE			lf_DisplayPos[16];	// Item display position
	BYTE			lf_DisplayLen[15];	// Display length (not used)

	UBYTE			lf_Flags;		// See LFORMATF_xxx below

	// Not used
	BYTE			lf_ShowFree;		// Show free space type

	// You must call ParsePattern() yourself
	char			lf_ShowPattern[40];	// Show pattern
	char			lf_HidePattern[40];	// Hide pattern
	char			lf_ShowPatternP[40];	// Show pattern parsed
	char			lf_HidePatternP[40];	// Hide pattern parsed
};

#define LFORMATF_REJECT_ICONS		(1<<0)	// Reject icons
#define LFORMATF_HIDDEN_BIT		(1<<1)	// Respect the H bit
#define LFORMATF_ICON_VIEW		(1<<2)	// Default to icon view
#define LFORMATF_SHOW_ALL		(1<<3)	// Show all

// Used for the sf_Sort and lf_DisplayPos fields
enum
{
	DISPLAY_NAME,
	DISPLAY_SIZE,
	DISPLAY_PROTECT,
	DISPLAY_DATE,
	DISPLAY_COMMENT,
	DISPLAY_FILETYPE,
	DISPLAY_OWNER,
	DISPLAY_GROUP,
	DISPLAY_NETPROT,
	DISPLAY_VERSION,

	DISPLAY_LAST
};

// Used for the sf_Separation field
enum
{
	SEPARATE_MIX,
	SEPARATE_DIRSFIRST,
	SEPARATE_FILESFIRST,
};

#endif


// Used for the gc_GetPointer/gc_FreePointer hooks
struct GetPointerPkt
{
	ULONG		gpp_Type;
	APTR		gpp_Ptr;
	ULONG		gpp_Flags;
};

#define GETPTR_HANDLE		2	// Get a function handle
#define GETPTR_COMMANDS		4	// Get internal command list

#define GETPTRF_DELPORT     (1<<1)

#ifndef DEF_GALILEOFMCOMMANDLIST
#define DEF_GALILEOFMCOMMANDLIST

struct GalileoCommandList
{
	struct Node	gcl_Node;		// Command name in ln_Name
	ULONG		gcl_Flags;		// Function flags
	char		*gcl_Description;	// Description string
	char		*gcl_Template;		// Command template
	char		*gcl_Module;		// Module command resides in
	char		*gcl_Help;		// Help file for command
};

#endif


// Structures used with callback commands
struct function_entry
{
	ULONG			pad[2];
	char			*name;	// File name
	APTR			entry;	// Entry pointer (don't touch!)
	short			type;	// Type of file
	short			flags;	// File flags
};

struct path_node
{
	ULONG			pad[2];
	char			buffer[512];	// Contains path string
	char			*path;		// Points to path string
	APTR			lister;		// Lister pointer
	ULONG			flags;		// Flags
};



#define LFF_ICON		(1<<0)


// Result code from EXTCMD_REPLACE_REQ
#define REPLACE_ABORT		-1
#define REPLACE_LEAVE		0
#define REPLACE_REPLACE		1
#define REPLACEF_ALL		(1<<1)


// EXTCMD_GET_SCREENDATA
struct GalileoScreenData
{
	struct Screen		*screen;	// Screen pointer
	struct DrawInfo		*draw_info;	// DrawInfo pointer
	unsigned short		depth;		// Screen depth
	unsigned short		pen_alloc;	// Mask of allocated pens
	unsigned short		pen_array[16];	// Pen array
	short			pen_count;	// Number of pens
};	


#define EXT_FUNC(name)	unsigned long __asm (*name)(register __d0 ULONG,register __a0 APTR,register __a1 APTR)
#define TYPE_EXT(var)	(unsigned long (*)())var

#define IDCMP_FUNC(name)	unsigned long __asm (*name)(register __d0 ULONG,register __a0 struct IntuiMessage *)


int __saveds __asm L_Module_Entry(register __a0 char *args,
				  register __a1 struct Screen *screen,
				  register __a2 IPCData *ipc,
				  register __a3 IPCData *main_ipc,
				  register __d0 ULONG mod_id,
				  register __d1 CONST GalileoCallbackInfo *gci);


/**** Define this if you are using the standard modinit.o code ****/
#ifdef _GALILEOFM_MODULE_DEF

// These library bases are defined and made available automatically
extern struct DosLibrary    *DOSBase;
extern struct Library	    *GalileoFMBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase	    *GfxBase;
extern struct Library	    *IconBase;
extern struct Library	    *UtilityBase;
extern struct Library	    *LayersBase;
extern struct Library	    *GadToolsBase;
extern struct Library	    *AslBase;
extern struct Library	    *DiskfontBase;
extern struct RxsLib	    *RexxSysBase;
extern struct Device	    *TimerBase;
extern struct LocaleBase    *LocaleBase;
extern struct Library	    *WorkbenchBase;

#ifdef RESOURCE_TRACKING
extern struct Library       *ResTrackBase;
#endif

// Global pointer to a module's locale information
extern struct GalileoLocale *locale;
void init_locale_data(struct GalileoLocale *);

// You MUST define the ModuleInfo structure yourself
extern ModuleInfo module_info;

#endif

#endif
