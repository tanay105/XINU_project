/* lcreate.c - lcreate, newlck */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
	STATWORD ps;    
	int	lck;

	disable(ps);
	if ( (lck=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	lockarray[lck].lbirthtime = ctr1000;
	lockarray[lck].lprocessid = currpid;
	restore(ps);
	//kprintf("created a lock with id: %d \n", lck);
	return(lck);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lck;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lck=nextlck--;
		if (nextlck < 0)
			nextlck = NLOCKS-1;
		if (lockarray[lck].lstate==LFREE) {
			lockarray[lck].lstate = LUSED;
			return(lck);
		}
	}
	return(SYSERR);
}
