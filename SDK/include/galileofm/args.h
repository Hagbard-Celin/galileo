#ifndef _GALILEOFM_ARGS
#define _GALILEOFM_ARGS

/*****************************************************************************

 ReadArgs support

 *****************************************************************************/

typedef struct
{
	struct RDArgs	*FA_RDArgs;	// RDArgs structure
	struct RDArgs	*FA_RDArgsRes;	// Return from ReadArgs()
	char		*FA_ArgString;	// Copy of argument string (with newline)
	long		*FA_ArgArray;	// Argument array pointer
	long		*FA_Arguments;	// Argument array you should use
	short		FA_Count;	// Number of arguments
	short		FA_DoneArgs;	// Galileo uses this flag for its own purposes
} FuncArgs;

FuncArgs *ParseArgs(char *,char *);
void DisposeArgs(FuncArgs *);

#endif
