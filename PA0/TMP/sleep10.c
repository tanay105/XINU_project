/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SLEEP10] = frequency[currpid][SLEEP10] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	if (n < 0  || clkruns==0){
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SLEEP10] = time[currpid][SLEEP10] + (b-a);
		}
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SLEEP10] = time[currpid][SLEEP10] + (b-a);
	}
	return(OK);
}
