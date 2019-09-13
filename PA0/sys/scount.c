/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SCOUNT] = frequency[currpid][SCOUNT] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SCOUNT] = time[currpid][SCOUNT] + (b-a);
		}
		return(SYSERR);
	}
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SCOUNT] = time[currpid][SCOUNT] + (b-a);
	}
	return(semaph[sem].semcnt);
}
