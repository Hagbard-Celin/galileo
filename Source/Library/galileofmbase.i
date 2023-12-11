
	IFND EXEC_TYPES_I
	INCLUDE "exec/types.i"
	ENDC

	IFND EXEC_LISTS_I
	INCLUDE "exec/lists.i"
	ENDC

	IFND EXEC_LIBRARIES_I
	INCLUDE "exec/libraries.i"
	ENDC

	STRUCTURE GalileoFMBase,LIB_SIZE
	UBYTE db_Flags
	UBYTE db_pad
	ULONG db_ExecBase
	ULONG db_DOSBase
	ULONG db_IntuitionBase
	ULONG db_GfxBase
	ULONG db_LayersBase
	ULONG pdb_cycletop
	ULONG pdb_cyclebot
	ULONG pdb_check
	ULONG pad1
	ULONG pad2
	ULONG pad3
	ULONG pad4
	ULONG pdb_Flags
	ULONG db_SegList
	LABEL GalileoFMBase_SIZEOF

GALILEOFMNAME MACRO
	DC.B 'galileofm.library',0
	ENDM
