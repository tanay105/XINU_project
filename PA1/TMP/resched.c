/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <lab.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);

int flag = 0;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int loop = 3;

int resched()
{
	if (global_sched_flag == 1 ) {
		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */
		
		int random = (int)expdev(0.1);
		int no_ctxsw_flag = 0;

		optr= &proctab[currpid];
	
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}
	
		/* remove highest priority process at end of ready list */

		int nextpid = get_next_expdist_proc(random, rdyhead);
		if (nextpid == NULLPROC){
			currpid = NULLPROC;
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
			return OK;
		}
	
		nptr = &proctab[nextpid];
		dequeue(nextpid);
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		currpid = nextpid;
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif
		
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;
	}
	else if (global_sched_flag == 2)
	{
		//if start of epoch
		//	calculate epoch
		//	calculate goodness value and assign
		//
		//find max of goodness value for ready queue processes
		//
		//dequeue the prcoess from ready queue
		//
		//context switch
		int temp_preempt = preempt;
		int flag_main_init = 0;

		/*let the main complete for the first run to create all process */
		if (temp_preempt > 0 && global_init_flag > 0) {
			return OK;
		}

		/*Main completes quantum and first linux sched init */
		if (global_init_flag > 0) {  /* first linux sched init */
			global_initialize_quantum_goodness();
			global_init_flag = 0;
			flag_main_init = 1;
		}

		/*find max goodness to check if the epoch end is reached*/
		int total_goody = find_max_goodness();

		if (total_goody == 0) {
			//printf("|");
			initialize_quantum_goodness();
		}

		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */
	
		optr= &proctab[currpid];
	
		//don't reset goodness and preempt values if new epoch
		if (total_goody != 0) {
			if (temp_preempt == 0 && flag_main_init == 1 ) {
				flag_main_init = 0;
			}
			else if (temp_preempt == 0 ) {
				optr->goody = temp_preempt;
				optr->prev_counter = temp_preempt;
			}
			else {
				optr->prev_counter = temp_preempt;
				optr->goody = optr->prev_counter + optr->pprio;
			}
		}

		if(optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		//nptr = &proctab[ (currpid = dequeue_goody_process()) ];
		int nextpid = get_next_goody_process_from_queue();
		if (nextpid < NPROC && nextpid > 0){
			currpid = nextpid;
			dequeue(currpid);
		}
		if (nextpid == NULLPROC){
			currpid = nextpid;
			//printf("%d", q[q[rdyhead].qnext].qkey);
			return EMPTY;
		}
		nptr = &proctab[currpid];

		//printf("%d", currpid);
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = nptr->time_quantum;		/* reset preemption counter by process quantum	*/
	#endif
		
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		
		/* The OLD process returns here when resumed. */
		return OK;
	}

	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
