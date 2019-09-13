/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep1000(int n)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SLEEP1000] = frequency[currpid][SLEEP1000] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    

	if (n < 0  || clkruns==0){
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SLEEP1000] = time[currpid][SLEEP1000] + (b-a);
		}
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SLEEP1000] = time[currpid][SLEEP1000] + (b-a);
	}
	return(OK);
}
