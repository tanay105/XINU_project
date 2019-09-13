/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(int nok, int pid)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SETNOK] = frequency[currpid][SETNOK] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SETNOK] = time[currpid][SETNOK] + (b-a);
		}
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SETNOK] = time[currpid][SETNOK] + (b-a);
	}
	return(OK);
}
