/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][GETPRIO] = frequency[currpid][GETPRIO] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][GETPRIO] = time[currpid][GETPRIO] + (b-a);
		}
		return(SYSERR);
	}
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][GETPRIO] = time[currpid][GETPRIO] + (b-a);
	}
	return(pptr->pprio);
}
