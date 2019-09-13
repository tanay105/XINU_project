/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][RECVCLR] = frequency[currpid][RECVCLR] + 1;
		to_print[currpid] = 1;
		a = ctr1000;
        }
	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][RECVCLR] = time[currpid][RECVCLR] + (b-a);
	}
	return(msg);
}
