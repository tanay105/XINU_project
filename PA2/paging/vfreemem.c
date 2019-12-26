/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	struct	mblock	*p, *q;
	unsigned top;

	//modify this for accomodating the new maxaddr
	//if (size==0 || (unsigned)block>(unsigned)maxaddr
	//    || ((unsigned)block)<((unsigned) &end))
	//	return(SYSERR);
	size = (unsigned)roundmb(size);
	for( p=proctab[currpid].vmemlist->mnext,q= &proctab[currpid].vmemlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &proctab[currpid].vmemlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		return(SYSERR);
	}
	if ( q!= &proctab[currpid].vmemlist && top == (unsigned)block )
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
	return(OK);
}
