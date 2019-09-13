/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][CHPRIO] = frequency[currpid][CHPRIO] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][CHPRIO] = time[currpid][CHPRIO] + (b-a);
		}
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][CHPRIO] = time[currpid][CHPRIO] + (b-a);
	}
	return(newprio);
}
