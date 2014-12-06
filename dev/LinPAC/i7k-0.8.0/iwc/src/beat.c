/**********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  beat.c
 *
 *  v 0.0.1 2000.05.03 by Reed Lai
 *
 *  time beats
 *
 *  History:
 *
 *  v 0.0.1 2000.05.03 by Reed Lai
 *    replace alarm() with setitimer(), beat resolution up to 1 microsecond
 *
 *  v 0.0.0 2000.04.28 by Reed Lai
 *    create, use alarm() and SIGALRM, 1 second time beat resolution
 *
 **********************************************************************/
#define BEAT_DEBUG		/* unmark to show beat debug message */

#include "callbacks.h"
#include "beat.h"

guint           time_signals[LAST_SIGNAL];	/* signal array for time
						 * beat */

// static int beat_nsecs; /* next beat for seconds */

struct itimerval itv,
                oitv;

/**********************************************************************
 * beat_next
 *
 * set the next time beat
 **********************************************************************/
int
beat_next(unsigned int t)
{
    itv.it_value.tv_sec = t / 1000000;
    itv.it_value.tv_usec = t % 1000000;

    itv.it_interval.tv_sec = itv.it_value.tv_sec;
    itv.it_interval.tv_usec = itv.it_value.tv_usec;

    return (setitimer(ITIMER_REAL, &itv, 0));

    // return( alarm( t ) );
}

/**********************************************************************
 * beat_close
 *
 * restore original itimerval
 **********************************************************************/
void
beat_close()
{
    setitimer(ITIMER_REAL, &oitv, &itv);
}

/**********************************************************************
 * beat_handler
 *
 * routine to handle time beat
 **********************************************************************/
void
beat_handler(int sig)
{
#define BELLS "\007\007\007"	/* ASCII bells */

    // gtk_signal_emit( gtk_obj[MAIN_WIN], time_signals[BEAT] );

    gtk_signal_emit(gtk_obj[FG_A], time_signals[BEAT]);
    gtk_signal_emit(gtk_obj[DRAW], time_signals[BEAT]);
    gtk_signal_emit(gtk_obj[FG_D], time_signals[BEAT]);
    gtk_signal_emit(gtk_obj[DI_BTN], time_signals[BEAT]);

    // beat_next( beat_nsecs );
}

/**********************************************************************
 * beat_init
 *
 * routine to handle time beat
 **********************************************************************/
int
beat_init()
{
    static struct sigaction act;

  /******************
   * clear old beat *
   ******************/
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;

    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &itv, &oitv);
    usleep(1000000);

  /****************************
   * create new signal "beat" *
   ****************************/
    time_signals[BEAT] = gtk_signal_new("beat",
					GTK_RUN_LAST | GTK_RUN_ACTION,
					GTK_TYPE_OBJECT,
					0,
					gtk_marshal_NONE__NONE,
					GTK_TYPE_NONE, 0);

  /************************
   * set action for alarm *
   ************************/
    act.sa_handler = beat_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGALRM);
    return (sigaction(SIGALRM, &act, NULL));

  /****************
   * rocking roll *
   ****************/
    // return( beat_next( beat_nsecs ) );
}
