/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][WAIT] = frequency[currpid][WAIT] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][WAIT] = time[currpid][WAIT] + (b-a);
		}
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][WAIT] = time[currpid][WAIT] + (b-a);
		}
		return pptr->pwaitret;
	}
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][WAIT] = time[currpid][WAIT] + (b-a);
	}
	return(OK);
}
