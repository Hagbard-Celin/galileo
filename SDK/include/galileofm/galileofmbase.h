/*
 *
 * galileofm.library header files
 *
 * Support library for Galileo
 *
 * (c) Copyright 1998 Jonathan Potter
 *
 */


// Include this file to conveniently include all the standard Galileo headers

#ifndef _GALILEOFMBASE
#define _GALILEOFMBASE

#include <galileofm/args.h>
#include <galileofm/bufferedio.h>
#include <galileofm/gui.h>
#include <galileofm/clipboard.h>
#include <galileofm/images.h>
#include <galileofm/iff.h>
#include <galileofm/misc.h>
#include <galileofm/progress.h>
#include <galileofm/appicon.h>
#include <galileofm/drag.h>
#include <galileofm/locale.h>
#include <galileofm/lists.h>
#include <galileofm/timer.h>
#include <galileofm/notify.h>
#include <galileofm/ipc.h>
#include <galileofm/requesters.h>
#include <galileofm/layout.h>
#include <galileofm/popup.h>
#include <galileofm/diskio.h>
#include <galileofm/edithook.h>
#include <galileofm/icon.h>
#include <galileofm/memory.h>
#include <galileofm/dos.h>
#include <galileofm/rexx.h>
#include <galileofm/hooks.h>

#ifndef GALILEOFM_PRAG
#include <pragmas/galileofm_pragmas.h>
#endif

// Galileo Public Semaphore
struct GalileoPublicSemaphore {
	struct SignalSemaphore	gs_Sem;
	APTR			gs_MainIPC;
	struct ListLock		gs_Modules;
};

#endif
