/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][GETPID] = frequency[currpid][GETPID] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	if (syscall_record == 1){
		b = ctr1000;
		time[currpid][GETPID] = time[currpid][GETPID] + (b-a);
	}
	return(currpid);
}
