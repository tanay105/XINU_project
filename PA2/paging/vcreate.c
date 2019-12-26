/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	//kprintf("starting vcreate\n");
	//create process using normal create method
	int new_pid = create(procaddr, ssize, priority, name, nargs, args);
	if (new_pid <= 0) {
		return SYSERR;
	}
	//allocate vheap for the process
	int bs_id;
	if (get_bsm(&bs_id) == SYSERR) {
		//check if we have to kill the process here before 
		//returing error back
		return SYSERR;
	}
	if (bsm_map(new_pid, 4096, bs_id, hsize) == SYSERR) {
		//check if we have to kill the process here before 
		//returing error back
		return SYSERR;
	}
	bsm_tab[bs_id].bs_sem = 1;

	proctab[new_pid].store = bs_id;
	kprintf("store id is %d\n", proctab[new_pid].store);
	proctab[new_pid].vhpno = 4096;
	proctab[new_pid].vhpnpages = hsize;

	proctab[new_pid].vmemlist->mnext = 4096*NBPG;
	struct mblock *vmptr;

	vmptr = 2048*NBPG + bs_id*NBPG;
	vmptr->mlen = hsize*NBPG;
	vmptr->mnext = NULL;

	//kprintf("finishing vcreate\n");
	return new_pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
