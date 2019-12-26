/* releaseall.c - realease locks */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * releaseall --  to release the specified number of locks
 *------------------------------------------------------------------------
 */

SYSCALL releaseall(int numlocks, int ldes1, ...)
{
	int ld = 0;
	int flag = 0;
	unsigned long *a = (unsigned long *) (&ldes1);
	while(numlocks--) {
		ld = *a++;
		//kprintf("lock desc is %d\n", ld);
		if (SYSERR == release(ld)) {
			flag = 1;
		}
	}
	return flag==1?SYSERR:OK;
}

int release(int lck)
{
	STATWORD ps;
	struct lentry *lptr = &lockarray[lck];
	struct pentry *pptr;
	int pid;
	
	disable(ps);
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}
	
	//unmark from the acquired list
	if (lockarray[lck].acquired[currpid] == -1) {
		//kprintf("ERROR: process does not hold the lock\n");
		restore(ps);
		return(SYSERR);
	}
	lockarray[lck].acquired[currpid] = -1;
	//marking the lock in proctab as 0
	if (proctab[currpid].locking[lck] == 0) {
		//kprintf("ERROR: process is not locking in proctab\n");
		restore(ps);
		return(SYSERR);
	}
	proctab[currpid].locking[lck] = 0;
	if (isacquiredempty(lck) == 1) {
		if (nonempty(lptr->lqhead)) {
			//printqueue(lptr->lqhead, lptr->lqtail);
			//schedule a process from wait queue as holders is empty
			//kprintf("scheduling a process from the wait list\n");
			release_from_wait(lck);
		}
		else {
			//kprintf("wait list is also empty\n");
			//mark ltype of the lock as not READ or WRITE
			lptr->ltype = 0;
		}
	}
	restore(ps);
	return(OK);
}

int isacquiredempty(int lck) {
	STATWORD ps;
	int i = 0;
	disable(ps);
	if ( (isbadlck(lck))) {
		restore(ps);
		return(SYSERR);
	}
	for (i=0 ; i<NPROC; i++) {
		if (lockarray[lck].acquired[i] != -1 )	{
			restore(ps);
			return 0;
		}
	}
	restore(ps);
	return 1;
}

int get_suitable_process(int lck) {
	//1. schdule the one with the highest priority
	//2. if two have the max priority then schdule the one with longest wait time
	//3. if the time difference is less than 1 sec, schdule the writer in that case
	STATWORD ps;
	struct lentry *lptr = &lockarray[lck];
	
	int pid;
	disable(ps);
	pid = getlast(lptr->lqtail);
	//kprintf("DEBUG: last pid is %d\n", pid);

	restore(ps);
	return pid;
}

int release_from_wait(int lck) {
	//1. schdule the one with the highest priority
	//2. if two have the max priority then schdule the one with longest wait time
	//3. if the time difference is less than 1 sec, schdule the writer in that case
	STATWORD ps;
	struct lentry *lptr = &lockarray[lck];
	struct pentry *pptr;
	
	int highest;
	int pid;
	unsigned long max_time;
	int temp_node;
	disable(ps);
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}
	highest = lastkey(lptr->lqtail);
	//kprintf("highest key in the wait list is %d\n", highest);
	max_time = 0;
	pid = 0;

	//printqueue(lptr->lqhead, lptr->lqtail);
	if (check_multiple(lptr->lqhead, lptr->lqtail, highest) == 1) {
		//kprintf("DEBUG: there is only one highest in the queue\n");
		pid = q[lptr->lqtail].qprev;
		//kprintf("the highest priority process in the queue is %d\n", pid);
		//check if highest if writer
		if (proctab[pid].locking[lck] == WRITE) {
			//it is a writer
			//kprintf("DEBUG: the highest priority is a writer\n");
			getlast(lptr->lqtail);
			pptr = &proctab[pid];
			pptr->pstate = PRREADY;
			pptr->lock_timer = 0;
			lptr->acquired[pid] = 1;
			lptr->ltype = pptr->locking[lck];
			insert(pid, rdyhead, pptr->pprio);
			resched();
			restore(ps);
			return(OK);
		}
		else if (proctab[pid].locking[lck] == READ) {
			//it is a reader
			//schdule all with reader with priority more than that of the writer if any waiting
			//current implementation only for one reader
			//kprintf("DEBUG: the highest priority is a reader\n");
			//printqueue(lptr->lqhead, lptr->lqtail);

			lptr->ltype = READ;
			while(pid != lptr->lqhead) {
				pptr = &proctab[pid];
				if (pptr->locking[lck] == READ) {
					//kprintf("making process %d ready\n", pid);
					getlast(lptr->lqtail);
					pptr->pstate = PRREADY;
					pptr->lock_timer = 0;
					lptr->acquired[pid] = 1;
					insert(pid, rdyhead, pptr->pprio);
				}
				else if (pptr->locking[lck] == WRITE) {
					break;
				}
				else {
					//kprintf("ERROR: in waitlist but the proctab does not have record\n");
					restore(ps);
					return(SYSERR);
				}
				pid = q[lptr->lqtail].qprev;
			}
			resched();
			restore(ps);
			return(OK);
		}
		else {
			//check this logic
			//kprintf("ERROR: I think the process does not have info about lock type\n");
		}
	}
	else {
		//kprintf("DEBUG: there are multiple highest in the queue\n");
		max_time = MAXINT;
		temp_node = q[lptr->lqhead].qnext;

		//pid with longest wait time is the one at the last always
		pid = longest_wait(lck, lptr->lqhead, lptr->lqtail, highest);
		//kprintf("DEBUG: the pid of the highest priority process with longest wait time is %d\n", pid);
		//check if highest is writer
		if (proctab[pid].locking[lck] == WRITE) {
			//it is a writer
			//kprintf("DEBUG: the highest priority is a writer\n");
			//getlast(lptr->lqtail);
			dequeue(pid);
			pptr = &proctab[pid];
			pptr->pstate = PRREADY;
			pptr->lock_timer = 0;
			lptr->acquired[pid] = 1;
			lptr->ltype = pptr->locking[lck];
			insert(pid, rdyhead, pptr->pprio);
			//printqueue(lptr->lqhead, lptr->lqtail);
			resched();
			restore(ps);
			return(OK);
		}
		else if (proctab[pid].locking[lck] == READ) {
			//it is a reader
			//schdule all with reader with priority more than that of the writer if any waiting
			//current implementation only for one reader
			//kprintf("DEBUG: the highest priority is a reader\n");
			//printqueue(lptr->lqhead, lptr->lqtail);

			lptr->ltype = READ;
			while(pid != lptr->lqhead) {
				pptr = &proctab[pid];
				if (pptr->locking[lck] == READ) {
					//kprintf("making process %d ready\n", pid);
					getlast(lptr->lqtail);
					pptr->pstate = PRREADY;
					pptr->lock_timer = 0;
					lptr->acquired[pid] = 1;
					insert(pid, rdyhead, pptr->pprio);
				}
				else if (pptr->locking[lck] == WRITE) {
					break;
				}
				else {
					//kprintf("ERROR: in waitlist but the proctab does not have record\n");
					restore(ps);
					return(SYSERR);
				}
				pid = q[lptr->lqtail].qprev;
			}
			resched();
			restore(ps);
			return(OK);
		}
		else {
			//check this logic
			//kprintf("ERROR: I think the process does not have info about lock type\n");
		}
	}
	restore(ps);
	return(OK);
}

int longest_wait(int lck, int head, int tail, int highest) {
	STATWORD ps;
	int pid;
	int max_time;
	int temp;
	disable(ps);
	if ( (isbadlck(lck))) {
		restore(ps);
		return(SYSERR);
	}

	pid = q[tail].qprev;
	max_time = proctab[pid].lock_timer;
	//kprintf("ctr of longest is %ld\n", max_time);
	temp = q[tail].qprev;

	//kprintf("DEBUG: the longest wait time process is %d\n", pid);
	while(temp != head) {
		//kprintf("ctr of temp is %ld\n", proctab[temp].lock_timer);
		//kprintf("lock priority is %d\n", q[temp].qkey);
		if ( q[temp].qkey == highest && proctab[temp].lock_timer - max_time < 1500 && proctab[temp].locking[lck] == WRITE) {
			pid = temp;
			//kprintf("DEBUG: found a writer with wait time within 1s is %d\n", pid);
		}
		temp = q[temp].qprev;
	}

	restore(ps);
	return pid;
}

int check_multiple(int head, int tail, int highest) {
	STATWORD ps;
	int count = 0;
	int temp;
	disable(ps);
	
	temp = q[head].qnext;
	//kprintf("DEBUG: now finding highest count\n");
	while (temp != tail) {
		if (q[temp].qkey == highest) {
			count++;
		}
		temp = q[temp].qnext;
	}
	restore(ps);
	//kprintf("DEBUG: the highest count is %d\n", count);
	return count;
}

int printqueue(int head, int tail)
{
	STATWORD ps;
	disable(ps);
	
	int temp = q[head].qnext;
	while (temp != tail) {
		//kprintf("DEBUG: key of temp is %d\n", q[temp].qkey);
		temp = q[temp].qnext;
	}
	restore(ps);
	return(OK);
}

int find_highest_writer(int head, int tail) {
	STATWORD ps;
	struct qent *hptr;
	int highest_writer = 0;

	disable(ps);
	hptr = &q[head];

	struct qent *temp = hptr->qnext;
	//while (temp->key != INT_MAX) {
	//	if 
	//}
	
	restore(ps);
	return highest_writer;
}

void release_all_locks(int pid) {
	STATWORD ps;
	int i=0;
	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		return(SYSERR);
	}
	for(i=0;i<NLOCKS;i++) {
		if (lockarray[i].acquired[pid] == 1) {
			release(i);
		}
	}
	restore(ps);
	return(OK);
}

void decrement_max_priority(int lck) {
	STATWORD ps;
	struct lentry *lptr;
	int head;
	int tail;
	int temp;
	int max = 0;
	disable(ps);
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}

	lptr = &lockarray[lck];
	head = lptr->lqhead;
	tail = lptr->lqtail;
	temp = q[head].qnext;
	//kprintf("lock id is %d, with currpid is %d\n",lck, currpid);
	while(temp != tail) {
		//kprintf("node in wait list is %d\n", temp);
		if (proctab[temp].pprio > max) {
			max = proctab[temp].pprio;
		}
		temp = q[temp].qnext;
	}
	//kprintf("max priority is now %d\n", max);
	lptr->lmax = max;
	
	restore(ps);
	return(OK);
}

void remove_from_wait_list(int pid, int lck) {
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;
	int i =0;
	disable(ps);
	if ( (isbadlck(lck)) || lptr->lstate == LFREE ) {
		restore(ps);
		return(SYSERR);
	}
	if (isbadpid(pid)) {
		restore(ps);
		return(SYSERR);
	}

	lptr = &lockarray[lck];
	
	dequeue(pid);

	decrement_max_priority(lck);

	invert_priorities_for_acquired_process(lck, 1);
	restore(ps);
}
