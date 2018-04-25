#include "timeP.h"

static struct timeval start_up_time; 

int 
gettimeofday();

long unsigned int 
ckit_msec_timer(int start_timing) 
{
  struct timeval stv;

  if (gettimeofday(&stv, (struct timezone *) 0) < 0) {
    fprintf(stderr, "Error: failed in gettimeofday \n");
    return 0;
  }
  
  if (start_timing) {
    start_up_time.tv_sec  = stv.tv_sec;
    start_up_time.tv_usec = stv.tv_usec;
    return 0;
  } else 
    return  ((stv.tv_sec - start_up_time.tv_sec - 1) * 1000 +
	     (stv.tv_usec - start_up_time.tv_usec + 1000000) / 1000);
}




ckit_msec_time_t
ckit_clock(int start_timing)
{
  /* 
	  static clock_t msec_since_start = 0;
	  clock_t c;
	  
	  c = (double)clock() / CLOCKS_PER_SEC * 1000;
	  if (start_timing) {
	  msec_since_start = c;
	  }
	  return c - msec_since_start;
  */
  return 1;
}

