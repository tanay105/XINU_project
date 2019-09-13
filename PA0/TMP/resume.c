/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][RESUME] = frequency[currpid][RESUME] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][RESUME] = time[currpid][RESUME] + (b-a);
		}
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][RESUME] = time[currpid][RESUME] + (b-a);
	}
	return(prio);
}
