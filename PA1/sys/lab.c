/* lab.c */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab.h>

int global_sched_flag;

int global_init_flag = 0;

int get_epoch_value() {
	//loop over proctab to get what is the epoch length
	int epoch;

	struct pentry *temp;

	int i=0;
	for(i=0;i<NPROC;i++) {
		temp = &proctab[i];
		if (temp->pstate != PRFREE) {
			epoch = epoch + temp->pprio;
			temp->time_quantum = temp->pprio;
		}
	}
	return epoch;
}

void initialize_quantum_goodness()
{
	int i=0;
	struct pentry *temp;
	for(i=0;i<NPROC;i++) {
		temp = &proctab[i];
		if (temp->pstate != PRFREE) {
			//update the quantum with counter and priority;
			//no point of updating prev_counter
			temp->time_quantum = temp->pprio + (int)(temp->prev_counter/2);
			temp->goody = temp->pprio + (int)(temp->prev_counter);
		}
	}

}

void global_initialize_quantum_goodness()
{
	int i=0;
	struct pentry *temp;
	for(i=0;i<NPROC;i++) {
		temp = &proctab[i];
		if (temp->pstate != PRFREE) {
			temp->time_quantum = temp->pprio;
			temp->goody = temp->pprio;
			temp->prev_counter = 0;
		}
	}

}

int dequeue_goody_process()
{
	int temp = get_next_goody_process_from_queue();
	if (temp < NPROC) {
		return dequeue(temp);
	}
	else {
		return EMPTY;
	}
}

int total_goodness()
{
	int next;
	next = q[rdyhead].qnext;
	int total = 0;

	struct pentry *q_i;
	while(q[next].qkey != MAXINT) {
		q_i = &proctab[next];
		total = total + q_i->goody;
		next = q[next].qnext;
	}
	return total;
}

int find_max_goodness()
{
	int next;
	next = q[rdyhead].qnext;
	int max = 0;

	int ret = 0;

	struct pentry *q_i;
	while(q[next].qkey != MAXINT) {
		//kprintf("%d\n", next);
		q_i = &proctab[next];
		if (q_i->goody > max) {
			max = q_i->goody;
			ret = next;
		}
		next = q[next].qnext;
	}
	//printf("%d\n", ret);
	return ret;
}

int get_next_goody_process_from_queue()
{
	int next;
	next = q[rdyhead].qnext;
	int max = 0;
	int ret = 0;

	struct pentry *q_i;

	while(q[next].qkey != MAXINT){
		q_i = &proctab[next];
		//printf("%d\n", next);
		if (q_i->goody > max) {
			max = q_i->goody;
			ret = next;
		}
		next = q[next].qnext;
	}
	//printf("%d\n", ret);
	return ret;
}

int get_queue_count(int head){
	int next;
	int count = 0;

	next = q[head].qnext;
	while(q[next].qkey != MAXINT) {
		count++;
		next = q[next].qnext;
	}
	return count;
}

void setschedclass(int sched_class)
{
	if (sched_class == EXPDISTSCHED) {
		global_sched_flag = 1;
		global_init_flag = 1;
	}
	else if (sched_class == LINUXSCHED) {
		global_sched_flag = 2;
		global_init_flag = 1;
	}
	else {
		global_sched_flag = 0;
	}
}

int getschedclass()
{
	if (global_sched_flag == 1) {
		return EXPDISTSCHED;
	}
	else if (global_sched_flag == 2) {
		return LINUXSCHED;
	}
}

int getexpdistnextproc(int random, int head)
{
	int next;
	int prev;

	next = q[head].qnext;
	while(q[next].qkey != MAXINT) {
		if (q[next].qkey > random){
			break;
		}
		next = q[next].qnext;
	}
	if (next == 251) {
		next = q[next].qprev;
	}

	//abc(head);

	kprintf("random is %d\n", random);
	kprintf("process is %d\n", next);

	if (next < NPROC){
		return ( dequeue(next) );
	}
	else {
		return (EMPTY);
	}
}

int get_next_expdist_proc(int random, int head)
{
	int next;
	int h1, h2;
	next = q[head].qnext;

	while(next != rdytail) {
		//printf("%d,%d\n", next, q[next].qkey);
		if (q[next].qkey > random) {
			h1 = next;
			h2 = q[next].qnext;
			while (next != rdyhead && next != rdytail && (q[h2].qkey == q[h1].qkey)) {
				h1 = h2;
				h2 = q[h1].qnext;
			}
			next = h1;
			break;
		}
		next = q[next].qnext;
	}

	if (next == rdytail) {
		next = q[next].qprev;
	}
	
	return next;
	
}

void abc(int head)
{
	int temp = head;
	while(q[temp].qkey != MAXINT) {
		kprintf ("queue is %d \n", temp);
		temp = q[temp].qnext;
	}
}

int printq(int head) 
{
	kprintf("printing queue now\n");
	int next;
	int prev;
		kprintf("head is %d\n", head);
		kprintf("head key is %d\n", q[head].qkey);
		kprintf("head next is %d\n", q[head].qnext);
		kprintf("head prev is %d\n", q[head].qprev);
	next = q[head].qnext;
		kprintf("1 is %d\n", next);
		kprintf("1 key is %d\n", q[next].qkey);
		kprintf("1 next is %d\n", q[next].qnext);
		kprintf("1 prev is %d\n", q[next].qprev);
	next = q[next].qnext;
		kprintf("2 is %d\n", next);
		kprintf("2 key is %d\n", q[next].qkey);
		kprintf("2 next is %d\n", q[next].qnext);
		kprintf("2 prev is %d\n", q[next].qprev);
	next = q[next].qnext;
		kprintf("3 is %d\n", next);
		kprintf("3 key is %d\n", q[next].qkey);
		kprintf("3 next is %d\n", q[next].qnext);
		kprintf("3 prev is %d\n", q[next].qprev);
	next = q[next].qnext;
		kprintf("4 is %d\n", next);
		kprintf("4 key is %d\n", q[next].qkey);
		kprintf("4 next is %d\n", q[next].qnext);
		kprintf("4 prev is %d\n", q[next].qprev);
	next = q[next].qnext;
		kprintf("5 is %d\n", next);
		kprintf("5 key is %d\n", q[next].qkey);
		kprintf("5 next is %d\n", q[next].qnext);
		kprintf("5 prev is %d\n", q[next].qprev);
	int key = 10;
	next = q[head].qnext;
	while(q[next].qkey != MAXINT) {
		if (q[next].qkey > key){
			break;
		}
		next = q[next].qnext;
	}
	if (next == 251) {
		next = q[next].qprev;
	}
	kprintf("\n\ncaught next : %d\n\n", next);
	return(OK);
}
