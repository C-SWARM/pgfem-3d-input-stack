#ifndef DID_TIMEP_H
#define DID_TIMEP_H

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#ifdef sgi
#define _BSD_TIME
#endif
#include <sys/time.h>

#ifndef CLOCKS_PER_SEC 
/* Sun's (sparc-sun-sunos4.1.3_U1) do not seem to have this */
#  define CLOCKS_PER_SEC 1000000
#endif

#define CKIT_START_TIMER 1
#define CKIT_LAP_TIMER   0

typedef long unsigned int  ckit_msec_time_t;

/* Timer:  returns the difference between the start time and the current */
/* time in milliseconds.  The timer can be started by passing /start_timing/ */
/* non-zero.  The argument /start_timing/ must be zero (0) if you don't want */
/* the timer (re)started. */
ckit_msec_time_t
ckit_msec_timer(int start_timing);


/* Timer:  returns the difference between the start time and the current */
/* time in milliseconds.  The timer can be started by passing /start_timing/ */
/* non-zero.  The argument /start_timing/ must be zero (0) if you don't want */
/* the timer (re)started. */
/* This is just a wrapper for the clock() routine (see manual page).  */
ckit_msec_time_t
ckit_clock(int start_timing);


#endif /* DID_TIMEP_H */
