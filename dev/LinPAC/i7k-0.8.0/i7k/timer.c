/**********************************************************************
 *
 *  timer.c
 *
 *  v 0.00 1998.10.20 by Reed Lai
 *
 *  modules of timer
 *
 *  History:
 *
 *  v 0.00 1998.10.20 by Reed Lai
 *    create
 *
 **********************************************************************/
/*
 * unmark following line to enable debug message 
 */
// #define I7K_DEBUG

#include "timer.h"

/**********************************************************************
 *  _time_get_tod
 *
 *  get time of day
 *
 *  Arguments:  none
 *
 *  Returned:   unsigned long long integer value of time of today
 **********************************************************************/
__tod_t _time_get_tod()
{
	struct timeval now;
	struct timezone tz;

	gettimeofday(&now, &tz);

	return ((__tod_t) now.tv_sec * 1000000 + now.tv_usec);
}


/**********************************************************************
 *  _time_set_deadline
 *
 *  give a deadline, determines it is time-out or not
 *
 *  Arguments:
 *    usec      microseconds before deadline
 *    deadline  structure timeval for storing deadline
 *
 *  Returned:   none
 **********************************************************************/
void _time_set_deadline(unsigned long usec, struct timeval *deadline)
{
	struct timeval now;
	struct timezone tz;

	gettimeofday(&now, &tz);

	now.tv_sec += usec / 1000000;
	now.tv_usec += usec % 1000000;

	deadline->tv_sec = now.tv_sec + (now.tv_usec / 1000000);
	deadline->tv_usec = now.tv_usec % 1000000;
}


/**********************************************************************
 *  _time_out
 *
 *  give a deadline, determines it is time-out or not
 *
 *  Arguments:
 *    deadline  deadline time value
 *
 *  Returned:
 *    0 time is ok, -1 time is up(time-out)
 **********************************************************************/
int _time_out(struct timeval *deadline)
{
	struct timeval now;
	struct timezone tz;

	gettimeofday(&now, &tz);
	if (now.tv_sec > deadline->tv_sec) {
		return -1;				/* time is up */
	}
	if ((now.tv_sec == deadline->tv_sec)
		&& (now.tv_usec >= deadline->tv_usec)) {
		return -1;				/* time is up */
	}
	return 0;					/* time is ok, go on */
}


/**********************************************************************
 *  _set_timeout
 *
 *  set flag when timeout
 *
 *  Arguments:
 *    usec         micro seconds
 *    h_sigalrm    signal handler of SIGALRM
 **********************************************************************/
// int _set_timeout(long usec, __sighandler_t h_sigalrm)
// {
// struct itimerval itval;
// struct itimerval oitval;

// itval.it_interval.tv_sec = 0;
// itval.it_interval.tv_usec = 0;

// itval.it_value.tv_sec = usec / 1000000;
// itval.it_value.tv_usec = usec % 1000000;

// if (signal(SIGALRM, h_sigalrm) == SIG_ERR) {
// return -1;
// }

// getitimer(ITIMER_REAL, &oitval);

  /*
   * return 0, success 
   */
  /*
   * return -1, error 
   */
// return (setitimer(ITIMER_REAL, &itval, &oitval));
// }
