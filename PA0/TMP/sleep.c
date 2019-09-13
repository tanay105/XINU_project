/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */

SYSCALL	sleep(int n)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SLEEP] = frequency[currpid][SLEEP] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	if (n<0 || clkruns==0) {
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SLEEP] = time[currpid][SLEEP] + (b-a);
		}
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SLEEP] = time[currpid][SLEEP] + (b-a);
		}
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);

	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SLEEP] = time[currpid][SLEEP] + (b-a);
	}
	return(OK);
}
