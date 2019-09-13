/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SIGNAL] = frequency[currpid][SIGNAL] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SIGNAL] = time[currpid][SIGNAL] + (b-a);
		}
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SIGNAL] = time[currpid][SIGNAL] + (b-a);
	}
	return(OK);
}
