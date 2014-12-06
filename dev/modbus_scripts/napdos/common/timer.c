/* Example of high resolution timer handle LinCon8k slot.

   Author: Moki Matsushima

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* File level history (record changes for this file here.)

   v 0.0.0  1 Sep 2004 by Moki Matsushima
   
     create, blah blah... */
     
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include "msw.h"

static int dovalue=0;
void sig_handler(int sig)
{
	static unsigned sig_counter;
//	printf("\rGot single %d for %u times ", sig, ++sig_counter);
	if (dovalue){
		dovalue = 0;
		DO_32(SLOT1,dovalue);
	}else{
		dovalue = 1;
		DO_32(SLOT1,dovalue);
	}
	EnableTimer(1000);
}

int main()
{
	slot_signal_t sig;

	static struct sigaction act, act_old;

	/* open device file */
	if(Open_Slot(SLOT0)){
		printf("open Slot %d failed!\n",SLOT0);
		return FAILURE;
	}
	
	if(Open_Slot(SLOT1)){
		printf("open Slot %d failed!\n",SLOT1);
		return FAILURE;
	}

	/* set action for signal */
	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	
	if (sigaction(SIGALRM, &act, &act_old)) {
		Close_Slot(SLOT0);
		Close_Slot(SLOT1);
		puts("Failure of signal action.");
		return FAILURE;
	}

	/* signal condiction */
	if(setSignal(SLOT0,SIGALRM,getpid())){
		Close_Slot(SLOT0);
		Close_Slot(SLOT1);
		sigaction(SIGALRM, &act_old, NULL);
		puts("Failure of signal condiction.");
		return FAILURE;
	}

	EnableTimer(1000);

	/* wait for exit */
	puts("press <enter> to exit\n");
	while (getchar() != 10);
	DisableTimer();

	sigaction(SIGALRM, &act_old, NULL);
	puts("\nEnd of program.");
	Close_Slot(SLOT0);
	Close_Slot(SLOT1);
	return SUCCESS;
}
