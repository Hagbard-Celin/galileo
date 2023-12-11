#include "example.h"

// Definition of the module
ModuleInfo
	module_info={
		1,					// Version
		"example.module",	// Module name
		"example.catalog",	// Catalog name
		0,					// Flags
		1,					// Number of functions
		{0,"Example",MSG_EXAMPLE_DESC,FUNCF_SYNC,0}};
