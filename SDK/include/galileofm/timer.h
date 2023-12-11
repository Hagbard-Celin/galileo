#ifndef _GALILEOFM_TIMER
#define _GALILEOFM_TIMER

/*****************************************************************************

 Timer

 *****************************************************************************/

typedef struct TimerHandle
{
	struct MsgPort		*port;		// Port to wait on
	struct timerequest	req;		// Timer request
	struct MsgPort		*my_port;	// Supplied port
	short			active;		// Indicates active request
} TimerHandle;

TimerHandle *AllocTimer(ULONG,struct MsgPort *);
BOOL CheckTimer(TimerHandle *);
void FreeTimer(TimerHandle *);
struct Library *GetTimerBase(void);
void StartTimer(TimerHandle *,ULONG,ULONG);
void StopTimer(TimerHandle *);
BOOL TimerActive(TimerHandle *);

#endif
