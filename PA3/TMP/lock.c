/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * lock  --  to acquire a lock
 *------------------------------------------------------------------------
 */
SYSCALL lock(int lck, int type, int lock_priority)
{
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;

	disable(ps);
	lptr = &lockarray[lck];
	pptr = &proctab[currpid];
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}

	if (lptr->lbirthtime > pptr->pbirthtime && lptr->lprocessid != currpid) {
		restore(ps);
		return(SYSERR);
	}

	if (lockarray[lck].ltype != READ && lockarray[lck].ltype != WRITE) {
		lockarray[lck].ltype = type;
		if (type == READ || type == WRITE) {
			if (lockarray[lck].acquired[currpid] == 1) {
				restore(ps);
				return(SYSERR);
			}
			else {
				lockarray[lck].acquired[currpid] = 1;  //add the PID to the locks acquired array
				proctab[currpid].locking[lck] = type;     //add the lock to the proctabs locking array
				////kprintf("adding %d pid to acquired list of %d at postion with flag %d\n", currpid, lck, lockarray[lck].acquired[currpid]);
				restore(ps);
				return(OK);
			}
		}
		else {
			////kprintf("invalid type of lock recognized\n");
			restore(ps);
			return(OK);
		}
	}

	if (lockarray[lck].ltype == READ) {
		if (type == READ) {
			if (lockarray[lck].acquired[currpid] == 1) {
				////kprintf("ERROR: lock requested has already been acquired\n");
				restore(ps);
				return(SYSERR);
			}
			else if ( lock_priority >= lastkey(lptr->lqtail)) {
				////kprintf("read type request for a read type lock having more priority than the max priority in wait list\n");
				lockarray[lck].acquired[currpid] = 1;  //add the PID to the locks acquired array
				proctab[currpid].locking[lck] = type;     //add the lock to the proctabs locking array
				////kprintf("adding %d pid to acquired list of %d at postion with flag %d\n", currpid, lck, lockarray[lck].acquired[currpid]);
				restore(ps);
				return(OK);
			}
			else {
				////kprintf("read type request for a read type lock having less priority than the max priority in wait list\n");
				pptr->pstate = PRWAIT;
				pptr->lid = lck;
				pptr->locking[lck] = type;
				pptr->lock_timer = ctr1000;
				insert(currpid, lptr->lqhead, lock_priority);
				update_max_priority(lck);
				invert_priorities_for_acquired_process(lck, 0);
				//printqueue(lptr->lqhead, lptr->lqtail);
				pptr->pwaitret = OK;
				resched();
				restore(ps);
				return pptr->pwaitret;
			}
		}
		else if (type == WRITE) {
			//check for the case when the pid is already in the queue before we put it inside the queue
			////kprintf("write type request for a read type lock\n");
			////kprintf("adding %d pid to wait state list of %d\n", currpid, lck);
			////kprintf("pname for process is %s\n", pptr->pname);
			pptr->pstate = PRWAIT;
			pptr->lid = lck;
			pptr->locking[lck] = type;
			pptr->lock_timer = ctr1000;
			insert(currpid, lptr->lqhead, lock_priority);
			update_max_priority(lck);
			invert_priorities_for_acquired_process(lck, 0);
			//printqueue(lptr->lqhead, lptr->lqtail);
			pptr->pwaitret = OK;
			resched();
			restore(ps);
			return pptr->pwaitret;
		}
	}
	else if (lockarray[lck].ltype == WRITE) {
		if (type == READ || type == WRITE) {
			////kprintf("read or write type request for a write type lock\n");
			////kprintf("adding %d pid to wait state list of %d\n", currpid, lck);
			////kprintf("pname for process is %s\n", pptr->pname);
			pptr->pstate = PRWAIT;
			pptr->lid = lck;
			pptr->locking[lck] = type;
			pptr->lock_timer = ctr1000;
			insert(currpid, lptr->lqhead, lock_priority);
			update_max_priority(lck);
			invert_priorities_for_acquired_process(lck, 0);
			//printqueue(lptr->lqhead, lptr->lqtail);
			pptr->pwaitret = OK;
			resched();
			restore(ps);
			return pptr->pwaitret;
		}
	}

	restore(ps);
	return(OK);
}

int update_max_priority(int lck) {
	STATWORD ps;
	struct lentry *lptr;
	int head;
	int tail;
	int temp;
	disable(ps);
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}

	lptr = &lockarray[lck];
	head = lptr->lqhead;
	tail = lptr->lqtail;
	temp = q[head].qnext;
	//kprintf("lock id is %d\n",lck);
	while(temp != tail) {
		//kprintf("node in wait list is %d\n", temp);
		if (proctab[temp].pprio > lptr->lmax) {
			lptr->lmax = proctab[temp].pprio;
		}
		temp = q[temp].qnext;
	}
	//kprintf("max priority is now %d\n", lptr->lmax);
	
	restore(ps);
	return(OK);
}

int invert_priorities_for_acquired_process(int lck, int kill) {
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;

	disable(ps);
	lptr = &lockarray[lck];
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}

	int i =0;
	//updating all the holders with new priorities
	for (i=0;i<NPROC;i++) {
		if (lptr->acquired[i] == 1) {
			update_process_priority(i, lck);
			//pptr = &proctab[i];
			//if (kill == 0) {
			//	if (pptr->pinh == 0) {
			//		pptr->pinh = pptr->pprio;
			//	}
			//	if (lptr->lmax > pptr->pprio) {
			//		pptr->pprio = lptr->lmax;
			//		//kprintf("updating %s process with priority %d\n", pptr->pname, pptr->pprio);
			//	}
			//}
			//else {
			//	if (lptr->lmax == 0) {
			//		pptr->pprio = pptr->pinh;
			//		pptr->pinh = 0;
			//		//kprintf("updating %s process with priority %d\n", pptr->pname, pptr->pprio);
			//	}
			//	else {
			//		pptr->pprio = lptr->lmax;
			//		//kprintf("updating %s process with priority %d\n", pptr->pname, pptr->pprio);
			//	}
			//}
		}
	}
	restore(ps);
	return(OK);
}

int update_process_priority(int pid, int lck) {
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;
	int i;
	int max;
	disable(ps);

	pptr = &proctab[pid];
	max = 0;
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}
	if (isbadpid(pid)) {
		restore(ps);
		return(SYSERR);
	}

	for (i=0;i<NLOCKS;i++) {
		if (pptr->locking[i] != 0 && i != pptr->lid) {
			lptr = &lockarray[i];
			//kprintf("lmax for %d lock is %d\n", i, lptr->lmax);
			if (max < lptr->lmax) {
				max = lptr->lmax;
			}
		}
	}
	
	if (pptr->pinh == 0 ) {
		pptr->pinh = pptr->pprio;
	}
	if (max >= pptr->pinh) {
		pptr->pprio = max;
		//kprintf("updating %s process with priority %d\n", pptr->pname, pptr->pprio);
	}
	else if (pptr->pinh > max){
		pptr->pprio = pptr->pinh;
		pptr->pinh = 0;
		//kprintf("updating %s process with priority %d\n", pptr->pname, pptr->pprio);
	}

	if (pptr->lid != -1) {
		invert_priorities_for_acquired_process(pptr->lid, 0);
	}
	restore(ps);
	return(OK);
}
