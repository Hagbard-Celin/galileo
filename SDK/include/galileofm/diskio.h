#ifndef _GALILEOFM_DISKIO
#define _GALILEOFM_DISKIO

/*****************************************************************************

 Disk I/O

 *****************************************************************************/

typedef struct
{
	struct MsgPort			*dh_Port;	// Message port
	struct IOExtTD			*dh_IO;		// IO request
	struct FileSysStartupMsg	*dh_Startup;	// Startup message
	struct DosEnvec			*dh_Geo;	// Disk geometry
	char				dh_Name[32];	// Disk name
	char				dh_Device[32];	// Device name
	struct InfoData			dh_Info;	// Disk information
	unsigned long			dh_Result;	// dh_Info is valid
	unsigned long			dh_Root;	// Root block
	unsigned long			dh_BlockSize;	// Block size
	struct DateStamp		dh_Stamp;	// not used
} DiskHandle;

// Some third-party DOS types
#define ID_AFS_PRO		0x41465301
#define ID_AFS_USER		0x41465302
#define ID_AFS_MULTI		0x6D754146
#define ID_PFS_FLOPPY		0x50465300
#define ID_PFS_HARD		0x50465301
#define ID_SFS0_HARD    0x53455300
#define ID_SFS2_HARD    0x53455302
#define ID_ENVOY_DISK	0x444F5380

DiskHandle *OpenDisk(char *,struct MsgPort *);
void CloseDisk(DiskHandle *);

#endif
