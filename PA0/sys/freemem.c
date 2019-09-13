/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL	freemem(struct mblock *block, unsigned size)
{
	int a = 0, b=0;
	if (syscall_record == 1){
        	frequency[currpid][FREEMEM] = frequency[currpid][FREEMEM] + 1;
		a = ctr1000;
        }
	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end)) {
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][FREEMEM] = time[currpid][FREEMEM] + (b-a);
		}
		return(SYSERR);
	}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		if(syscall_record == 1)	{
			b = ctr1000;
			time[currpid][FREEMEM] = time[currpid][FREEMEM] + (b-a);
		}
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	if(syscall_record == 1)	{
		b = ctr1000;
		time[currpid][FREEMEM] = time[currpid][FREEMEM] + (b-a);
	}
	return(OK);
}
