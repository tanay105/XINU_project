/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SIGNALN] = frequency[currpid][SIGNALN] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SIGNALN] = time[currpid][SIGNALN] + (b-a);
		}
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SIGNALN] = time[currpid][SIGNALN] + (b-a);
	}
	return(OK);
}
