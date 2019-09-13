/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <date.h>
#include <lab0.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][GETTIME] = frequency[currpid][GETTIME] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
    /* long	now; */

	/* FIXME -- no getutim */

	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][GETTIME] = time[currpid][GETTIME] + (b-a);
	}
    return OK;
}
