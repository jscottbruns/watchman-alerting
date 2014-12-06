/**********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  beat.h
 *
 *  v 0.0.0 2000.04.28 by Reed Lai
 *
 *  head of beat.c
 *
 *  History:
 *
 *  v 0.0.0 2000.04.28 by Reed Lai
 *    create, yeh
 *
 **********************************************************************/

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include <gnome.h>

enum {
    BEAT,			/* signal for time beat */
    LAST_SIGNAL
};

extern guint    time_signals[LAST_SIGNAL];	/* signal array for time
						 * beat */
extern struct itimerval itv,
                oitv;

int             beat_next(unsigned int);
void            beat_handler(int);
int             beat_init();
void            beat_close();
