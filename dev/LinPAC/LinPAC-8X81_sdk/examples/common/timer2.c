/* Timer test 

   Author: Moki Matsushima  (v 0.0.0  1 April 2004)

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

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define  PROMPT "5 sec pass\n\a"

char *prompt=PROMPT;
unsigned int len;

void prompt_info(int signo)
{
	write(STDERR_FILENO,prompt,len);
}

void init_sigaction(void)
{
	struct sigaction act;
	act.sa_handler=prompt_info;
  act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGPROF,&act,NULL);
}

void init_time()
{
  struct itimerval value;
	value.it_value.tv_sec=5;/* seconds */
	value.it_value.tv_usec=0;/* microseconds */
  value.it_interval=value.it_value;/* it_interval->next value; it_value->current value */

  setitimer(ITIMER_PROF,&value,NULL); //virtual time expired.
}

int main()
{
	len=strlen(prompt);
	init_sigaction();
	init_time();
	while(1);
	exit(0);
} 
