/* lab.c - miscellaneous functions for project 2 */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

#define F10 0xFFC00000
#define M2112 0x003FF000

node_t *age_head;
node_t *head_queue;
node_t *sc_evic_ptr;

int init_global_pages()
{
	int i = 4;
	int j = 0;
	for(i=0;i<4;i++) {
		int pt_avail;
		if (get_frm(&pt_avail) == 0) {
			return 0;
		}
		else {
			//modify the frame values
			frm_tab[pt_avail].fr_status = FRM_MAPPED;
			frm_tab[pt_avail].fr_pid = -1;
			frm_tab[pt_avail].fr_vpno = -1;
			frm_tab[pt_avail].fr_refcnt = 0;
			frm_tab[pt_avail].fr_type = FR_TBL;
			frm_tab[pt_avail].fr_dirty = -1;
		}

		//pointer if type pt_t;
		pt_t *pte;
		
		//address of the pointer should be the first byte
		//of the page table
		pte = NBPG*(FRAME0 + pt_avail);
		
		int count=0;
		while(count<1024) {
			pte->pt_pres = 1;
			pte->pt_write = 1;
			pte->pt_user = 0;
			pte->pt_pwt = 0;
			pte->pt_pcd = 0;
			pte->pt_acc = 0;
			pte->pt_dirty = 0;
			pte->pt_mbz = 0;
			pte->pt_global = 0;
			pte->pt_avail = 0;
			pte->pt_base = j;
			pte = pte + 1;
			frm_tab[pt_avail].fr_refcnt++;
			count++;
			j++;
		}
	}
	return 1;
}

int init_pd_for_null() {
	int pd_avail;
	if (get_frm(&pd_avail) == SYSERR) {
		return SYSERR;
	}
	else {
		//modify the frame values
		frm_tab[pd_avail].fr_status = FRM_MAPPED;
		frm_tab[pd_avail].fr_pid = -1;
		frm_tab[pd_avail].fr_vpno = -1;
		frm_tab[pd_avail].fr_refcnt = 0;
		frm_tab[pd_avail].fr_type = FR_DIR;
		frm_tab[pd_avail].fr_dirty = -1;
	}

	//pointer if type pt_t;
	pd_t *pde;
	
	//address of the pointer should be the first byte
	//of the page directory
	pde = NBPG*(FRAME0 + pd_avail);

	init_default_pde(pde, pd_avail);

	proctab[currpid].pdbr = pde;

	write_cr3(pde);

}

int init_default_pde(pd_t *pde, int pd_avail) {
	int i=0;
	for(i=0;i<4;i++) {
		pde->pd_pres = 1;
		pde->pd_write = 1;
		pde->pd_user = 0;
		pde->pd_pwt = 0;
		pde->pd_pcd = 0;
		pde->pd_acc = 0;
		pde->pd_mbz = 0;
		pde->pd_fmb = 0;
		pde->pd_global = 0;
		pde->pd_avail = 0;
		pde->pd_base = (FRAME0 + i);
		frm_tab[pd_avail].fr_refcnt++;
		pde = pde + 1;
	}
	return OK;
}

int init_pd_for_proc(int proc) {
	int pd_avail;
	if (get_frm(&pd_avail) == SYSERR) {
		//call swap module to get new page
		//swap the page and then use the given id
		pd_avail = global_page_algo(NULL);
		//perform swapping here
		swapping_page_id(pd_avail);
		
		//delete page from queue as using for Page Table now
		global_page_delete(pd_avail);
		print_age();
	}
	//modify the frame values
	frm_tab[pd_avail].fr_status = FRM_MAPPED;
	frm_tab[pd_avail].fr_pid = proc;
	frm_tab[pd_avail].fr_vpno = -1;
	frm_tab[pd_avail].fr_refcnt = 0;
	frm_tab[pd_avail].fr_type = FR_DIR;
	frm_tab[pd_avail].fr_dirty = -1;

	//pointer if type pt_t;
	pd_t *pde;
	
	//address of the pointer should be the first byte
	//of the page directory
	pde = NBPG*(FRAME0 + pd_avail);

	init_default_pde(pde, pd_avail);

	proctab[proc].pdbr = pde;
	return OK;
}

int create_page_table(int *pt_avail) {
	if (get_frm(pt_avail) == SYSERR) {
		//call swap module to get new page
		//swap the page and then use the given id
		*pt_avail = global_page_algo(NULL);
		//perform swapping here
		swapping_page_id(*pt_avail);
		
		//delete page from queue as using for Page Table now
		global_page_delete(*pt_avail);
		print_age();
	}
	//modify the frame values
	frm_tab[*pt_avail].fr_status = FRM_MAPPED;
	frm_tab[*pt_avail].fr_pid = currpid;
	frm_tab[*pt_avail].fr_vpno = -1;
	frm_tab[*pt_avail].fr_refcnt = 0;
	frm_tab[*pt_avail].fr_type = FR_TBL;
	frm_tab[*pt_avail].fr_dirty = -1;
	return OK;
}

int create_page(int *pg_avail, int vp, pt_t *pt) {
	int add = 1;
	if (get_frm(pg_avail) == SYSERR) {
		//call swap module to get new page
		//swap the page and then use the given id
		//kprintf("entering sw_page algo\n");
		*pg_avail = global_page_algo(pt);
		//perform swapping here
		swapping_page_id(*pg_avail);
		print_age();

		//setting the flag for adding to the circular queue
		add = 0;
	}
	//modify the frame values
	frm_tab[*pg_avail].fr_status = FRM_MAPPED;
	frm_tab[*pg_avail].fr_pid = currpid;
	frm_tab[*pg_avail].fr_vpno = vp;
	frm_tab[*pg_avail].fr_refcnt = 0;
	frm_tab[*pg_avail].fr_type = FR_PAGE;
	frm_tab[*pg_avail].fr_dirty = -1;
	if (add == 1)
		return 1;
	else
		return 0;
}

int get_bs_id(bsd_t *id) {
	int i=0;
	for(i=0;i<8;i++) {
		if (bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_pid == currpid) {
			*id = i;
			return OK;
		} 
	}
	return SYSERR;
}

int get_page_offset(int vp, int bs_id) {
	if (bsm_tab[bs_id].bs_status != BSM_MAPPED || bsm_tab[bs_id].bs_pid != currpid) {
		return SYSERR;
	}
	return (vp - bsm_tab[bs_id].bs_vpno);
}

int init_sc() {
	head_queue = NULL;
	sc_evic_ptr = NULL;
	return OK;
}

int init_age() {
	age_head = NULL;
}

node_t *createNode(int a, pt_t *pt) {
	node_t *newNode;
	newNode = (node_t *)getmem(sizeof(node_t));
	if (newNode == NULL) {
		return SYSERR;
	}
	(newNode->page_id) = a;
	(newNode->ptr) = pt;
	(newNode->age) = 255;
	(newNode->next) = NULL;
	return newNode;
}

int add_to_age(int page_id, pt_t *pt) {
	//kprintf("adding page %d to age\n", page_id);
	if (age_head == NULL) {
		node_t *temp = createNode(page_id, pt);
		age_head = temp;
		age_head->next = NULL;
		return OK;
	}
	else {
		node_t *temp = age_head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		node_t *temp2 = createNode(page_id, pt);
		temp->next = temp2;
		temp2->next = NULL;
		return OK;
	}
}

int add_to_sc(int page_id, pt_t *pt) {
	//kprintf("adding %d page to queue\n", page_id);
	if (head_queue == NULL) {
		//add the first node to the queue
		node_t *temp = createNode(page_id, pt);
		head_queue = temp;
		sc_evic_ptr = temp;
		head_queue->next = head_queue;
		return OK;
	}
	else if (head_queue->next == head_queue) {
		node_t *temp = createNode(page_id, pt);
		head_queue->next = temp;
		temp->next = head_queue;
		return OK;
	}
	else {
		node_t *temp = head_queue;
		while (temp->next != head_queue) {
			temp = temp->next;
		}
		node_t *temp2 = createNode(page_id, pt);
		temp->next = temp2;
		temp2->next = head_queue;
		return OK;
	}
}

int delete_from_age(int page_id) {
	//kprintf("delete page %d from age\n", page_id);
	if (age_head == NULL) {
		return SYSERR;
	}
	else if (age_head->next == NULL) {
		if (age_head->page_id == page_id) {
			freemem(age_head, sizeof(node_t));
			age_head = NULL;
			return OK;
		}
		else {
			return SYSERR;
		}
	}
	else {
		//kprintf("age_head is pointing to %d page\n", age_head->page_id);
		node_t *prev, *curr, *new_next;
		if (age_head->page_id == page_id) {
			curr = age_head;
			//kprintf("curr is pointing to %d page\n", curr->page_id);
			new_next = age_head->next;
			//kprintf("new_next is pointing to %d page\n", new_next->page_id);
			//point new head
			age_head = new_next;
			//kprintf("New age_head is pointing to %d page\n", age_head->page_id);
			freemem(curr, sizeof(node_t));
			return OK;
		}
		else {
			int end = 1;
			prev = age_head;
			curr = prev->next;
			while (curr != NULL) {
				if (curr->page_id == page_id) {
					end = 0;
					break;
				}
				prev = curr;
				curr = curr->next;
			}
			if (end == 1) {
				return SYSERR;
			}
			new_next = curr->next;
			prev->next = new_next;
			freemem(curr, sizeof(node_t));
			return OK;
		}
	}
}

int delete_from_sc(int page_id) {
	if (head_queue == NULL) {
		return SYSERR;
	}
	else if (head_queue->next == head_queue) {
		if (head_queue->page_id == page_id) {
			freemem(head_queue, sizeof(node_t));
			head_queue = NULL;
			sc_evic_ptr = NULL;
			return OK;
		}
		else {
			return SYSERR;
		}
	}
	else {
		node_t *prev, *curr, *new_next;
		if (head_queue->page_id == page_id) {
			curr = head_queue;
			prev = head_queue;
			while(prev->next != head_queue) {
				prev = prev->next;
			}
			new_next = curr->next;
			//update pointers now
			prev->next = new_next;
			if (sc_evic_ptr == curr) {
				sc_evic_ptr = new_next;
			}
			freemem(curr, sizeof(node_t));
			head_queue = new_next;
			return OK;
		}
		else {
			prev = head_queue;
			curr = prev->next;
			//***** potential point of failure *****
			//could be an infinite loop
			while(curr->page_id != page_id) {
				prev = curr;
				curr = curr->next;
			}
			new_next = curr->next;
			prev->next = new_next;
			if (sc_evic_ptr == curr) {
				sc_evic_ptr = new_next;
			}
			freemem(curr, sizeof(node_t));
			return OK;
		}
	}
	return SYSERR;
}

int print_age() {
	node_t *t = age_head;
	while(t->next != NULL) {
		//kprintf("%d %d %d\n", t->page_id, t->age, t->ptr->pt_acc);
		t = t->next;
	}
	//kprintf("%d %d %d \n", t->page_id, t->age, t->ptr->pt_acc);
}

int age_swap_algo(pt_t *pt) {
	if (age_head == NULL) {
		return SYSERR;
	}
	node_t *temp = age_head;
	int min_id = -1;
	int min_age = 1000;
	while (temp != NULL) {
		temp->age = temp->age >> 1;
		if (temp->ptr->pt_acc == 1) {
			temp->age = temp->age + 128;
			temp->ptr->pt_acc = 0;
		}
		if (temp->age < min_age) {
			min_age = temp->age;
			min_id = temp->page_id;
		}
		temp = temp->next;
	}
	//kprintf("printing FIFO queue inside head again\n");
	if (min_id != -1) {
		delete_from_age(min_id);
		add_to_age(min_id, pt);
		return min_id;
	}
	return SYSERR;
}

int sc_swap_page(pt_t *pt) {
	node_t *t = sc_evic_ptr;
	while(t->next != sc_evic_ptr) {
		//kprintf("%d %d,", t->page_id, t->ptr->pt_acc);
		t = t->next;
	}
	//kprintf("%d %d\n", t->page_id, t->ptr->pt_acc);
	if (sc_evic_ptr == NULL) {
		return SYSERR;
	}
	else {
		node_t *temp = sc_evic_ptr;
		while (1) {
			if (temp->ptr->pt_acc == 1) {
				temp->ptr->pt_acc = 0;
				temp = temp->next;
			}
			else {
				sc_evic_ptr = temp->next;
				//kprintf("returning %d page from algo\n", temp->page_id);
				temp->ptr = pt;
				return temp->page_id;	
			}
		}
	}
}

int vpno_lookup(int page_id) {
	if (frm_tab[page_id].fr_status == FRM_MAPPED){
		return frm_tab[page_id].fr_vpno;
	}
	else {
		return SYSERR;
	}
}

int pid_lookup(int page_id) {
	if (frm_tab[page_id].fr_status == FRM_MAPPED){
		return frm_tab[page_id].fr_pid;
	}
	else {
		return SYSERR;
	}
}

void invlpg(unsigned long addr){
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

int write_to_bs(int page_id, unsigned long addr) {
	int bs_id;
	int page_offset;
	if (bsm_lookup(currpid, addr, &bs_id, &page_offset) != SYSERR){
		write_bs((FRAME0 + page_id)*NBPG, bs_id, page_offset);
		return OK;
	}
	else {
		kprintf("something is wrong!!!\n");
		kill(currpid);
		return SYSERR;
	}
}

int swapping_page_id(int page_id) {
	int vp = vpno_lookup(page_id);
	unsigned long addr = vp*4096;
	int pid = pid_lookup(page_id);
	
	virt_addr_t fault_page;
	fault_page.pd_offset = (addr & F10) >> 22;
	fault_page.pt_offset = (addr & M2112) >> 12;

	pd_t *pd;
	pt_t *pt;
	pd = proctab[pid].pdbr;
	pd = pd + fault_page.pd_offset;
	if (pd->pd_pres == 1) {
		pt = NBPG*(pd->pd_base);
		pt = pt + fault_page.pt_offset;
		pt->pt_pres = 0;

		if (frm_tab[page_id].fr_pid == currpid) {
			invlpg(addr);
		}

		//***** Add decrement ref count *****
		frm_tab[(pd->pd_base) - FRAME0].fr_refcnt -= 1;

		if (frm_tab[(pd->pd_base) - FRAME0].fr_refcnt == 0) {
			pd->pd_pres = 0;
			free_frm(pd->pd_base);
		}
		//kprintf("planning to replace page %d\n", page_id);
		//write the page to backing store if page is dirty
		if (pt->pt_dirty == 1) {
			write_to_bs(page_id, addr);
		}
	}
	else {
		kprintf("ERROR: page table entry is not present\n");
		return SYSERR;
	}
}

int global_page_algo(pt_t *pt) {
	int page_id;
	if (page_replace_policy == SC) {
		page_id = sc_swap_page(pt);
	}
	else {
		page_id = age_swap_algo(pt);
	}
	kprintf("Swapping frame %d\n", page_id);
	return page_id;
}

int global_page_add(int page_id, pt_t *pt) {
	if (page_replace_policy == SC) {
		add_to_sc(page_id, pt);
	}
	else {
		add_to_age(page_id, pt);
	}
	return page_id;
}

int global_page_delete(int page_id) {
	if (page_replace_policy == SC) {
		delete_from_sc(page_id);
	}
	else if (page_replace_policy == AGING ){
		delete_from_age(page_id);
	}
	return page_id;
}

int delete_page_RAM(int pid) {
	int i=0;
	for(i=0;i<NFRAMES;i++) {
		if (frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE) {
			free_frm(i);
			global_page_delete(i);
		}
	}
	return OK;
}

int delete_page_table_RAM(int pid) {
	pd_t *pd;
	pd = proctab[pid].pdbr;

	pd_t *temp;
	int i=4;
	while (i<1024) {
		temp = pd + i;
		if (temp->pd_pres == 1) {
			free_frm((temp->pd_base) - FRAME0);
		}
		i++;
	}
	return OK;
}
