#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>
#include <lab.h>
#include <mem.h>

#define PROC1_VADDR	0x40000000
#define PROC1_VPNO      0x40000
#define PROC2_VADDR     0x80000000
#define PROC2_VPNO      0x80000
#define PROC3_VADDR     0x50000000
#define PROC3_VPNO      0x50000
#define TEST1_BS	1
#define TEST2_BS	2

void proc1_test1(char *msg, int lck) {
	char *addr;
	int i;

	get_bs(TEST1_BS, 100);
	//get_bs(TEST2_BS, 100);

	if (xmmap(PROC1_VPNO, TEST1_BS, 100) == SYSERR) {
		kprintf("xmmap call failed\n");
		sleep(3);
		return;
	}

	//if (xmmap(PROC2_VPNO, TEST2_BS, 100) == SYSERR) {
	//	kprintf("xmmap call failed\n");
	//	sleep(3);
	//	return;
	//}

	addr = (char*) PROC1_VADDR;
	for (i = 0; i < 26; i++) {
		*(addr + i * NBPG) =  'A' + i;
		//kprintf("Addr is %d, i is %d\n", (int)(addr + i*NBPG)/NBPG, i);
	}
	//kprintf("assigning done \n");

	sleep(6);

	for (i = 0; i < 26; i++) {
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	xmunmap(PROC1_VPNO);
	return;
}

void proc1_test2(char *msg, int lck) {
	int *x;

	kprintf("ready to allocate heap space\n");
	x = vgetmem(1024);
	kprintf("heap allocated at %x\n", x);
	//kprintf("---------------------------------------------------\n");
	//print_age();
	*x = 100;
	*(x + 1) = 200;
	//print_age();
	//kprintf("---------------------------------------------------\n");
	//sleep(3);
	kprintf("heap variable: %d %d\n", *x, *(x + 1));
	vfreemem(x, 1024);
}

void proc1_test3(char *msg, int lck) {

	char *addr;
	int i;

	addr = (char*) 0x0;

	for (i = 0; i < 1024; i++) {
		*(addr + i * NBPG) = 'B';
	}

	for (i = 0; i < 1024; i++) {
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	return;
}

int main() {
	int pid1;
	int pid2;

	//srpolicy(4);

	kprintf("\n1: shared memory\n");
	pid1 = create(proc1_test1, 2000, 20, "proc1_test1", 0, NULL);
	resume(pid1);
	sleep(10);
	//print_age();

	kprintf("\n2: vgetmem/vfreemem\n");
	pid1 = vcreate(proc1_test2, 2000, 100, 20, "proc1_test2", 0, NULL);
	kprintf("pid %d has private heap\n", pid1);
	resume(pid1);
	sleep(3);
	//print_age();

	kprintf("\n3: Frame test\n");
	pid1 = create(proc1_test3, 2000, 20, "proc1_test3", 0, NULL);
	resume(pid1);
	sleep(3);
	//add_to_sc(100, 123456);
	//add_to_sc(200, 123456);
	//add_to_sc(300, 123456);
	//add_to_sc(400, 123456);
	//kprintf("sc ptr frame %d, with pt_acc %d with addr %d\n", sc_evic_ptr->page_id, sc_evic_ptr->ptr->pt_acc, sc_evic_ptr);
	//delete_from_sc(100);
	//add_to_sc(450, 123456);
	////head_queue->next->ptr->pt_acc = 0;
	////kprintf("%d has to be swapped \n", sc_swap_page());

	//node_t *temp = head_queue;
	//while(temp->next != head_queue) {
	//	kprintf("frame %d, with pt_acc %d with address %d\n", temp->page_id, temp->ptr->pt_acc, temp);
	//	temp = temp->next;
	//}
	//kprintf("frame %d, with pt_acc %d with addr %d\n", temp->page_id, temp->ptr->pt_acc, temp);
	//kprintf("sc ptr frame %d, with pt_acc %d with addr %d\n", sc_evic_ptr->page_id, sc_evic_ptr->ptr->pt_acc, sc_evic_ptr);
}
