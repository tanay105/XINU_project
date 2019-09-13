/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][SETDEV] = frequency[currpid][SETDEV] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	short	*nxtdev;

	if (isbadpid(pid)) {
		return(SYSERR);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][SETDEV] = time[currpid][SETDEV] + (b-a);
		}
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][SETDEV] = time[currpid][SETDEV] + (b-a);
	}
	return(OK);
}
