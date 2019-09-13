/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep100(int n)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SLEEP100] = frequency[currpid][SLEEP100] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    

	if (n < 0  || clkruns==0){
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SLEEP100] = time[currpid][SLEEP100] + (b-a);
		}
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SLEEP100] = time[currpid][SLEEP100] + (b-a);
	}
	return(OK);
}
