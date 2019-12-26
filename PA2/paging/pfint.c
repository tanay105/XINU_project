/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
int l;

#define F10 0xFFC00000
#define M2112 0x003FF000

SYSCALL pfint()
{
	unsigned long addr = read_cr2();
	unsigned long vp = addr/NBPG;
	//kprintf("VA is %ld and VP is %ld\n", addr, vp);
	pd_t *pd = proctab[currpid].pdbr;
	//kprintf("PDBR is at %ld\n", proctab[currpid].pdbr);
	pt_t *pt;
	//kprintf("vp is %d\n", vp);

	if (vp == l) {
		shutdown();
	}
	l = vp;

	//check if legal or not
	int i=0;
	for (i=0;i<8;i++) {
		if (bsm_tab[i].bs_pid == currpid) {
			if (vp < bsm_tab[i].bs_vpno || vp > bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages) {
				//kprintf("vp is %d, bs_vpno is %d, bs_npages is %d\n", vp, bsm_tab[i].bs_vpno, bsm_tab[i].bs_npages);
				//kprintf("address out of range\n");
				kill(currpid);
			}
		}
	}
	
	virt_addr_t fault_page;
	fault_page.pd_offset = (addr & F10) >> 22;
	fault_page.pt_offset = (addr & M2112) >> 12;
	//kprintf("Page dir offset is %ld\n", fault_page.pd_offset);
	//kprintf("Page table offset is %ld\n", fault_page.pt_offset);

	pd = pd + fault_page.pd_offset;
	//kprintf("Checking for pd entry at %ld\n", pd);
	if (pd->pd_pres == 1) {
		//kprintf("pd entry exists\n");
		//page directory entry is present
		//means page table is present
		//page entry is not preent 
		//means page is not present

		//page table ptr update		
		pt = NBPG*(pd->pd_base);
		pt = pt + fault_page.pt_offset;

		if (pt->pt_pres == 1){
			//kprintf("pt entry exists\n");
			//kprintf("page frame number is %d\n", pt->pt_base);
			//kprintf("something is not right here!\n");
		}
		else {
			//kprintf("page entry is not present\n");
			//get frame for page
			int pg_avail;
			int add_pg = create_page(&pg_avail, vp, pt);
			//fill the page table entry
			pt->pt_pres = 1;
			pt->pt_write = 1;
			pt->pt_user = 0;
			pt->pt_pwt = 0;
			pt->pt_pcd = 0;
			//pt->pt_acc = 0;
			pt->pt_dirty = 0;
			pt->pt_mbz = 0;
			pt->pt_global = 0;
			pt->pt_avail = 0;
			pt->pt_base = (FRAME0 + pg_avail);
			frm_tab[(pd->pd_base - FRAME0)].fr_refcnt++;
			if (add_pg == 1) {
				global_page_add(pg_avail, pt);
			}
			//kprintf("pt is in frame %d\n", pd->pd_base - FRAME0);
			//kprintf("ref count for the pt is %d\n", frm_tab[(pd->pd_base - FRAME0)].fr_refcnt);
		}
	}
	else {
		//kprintf("pd entry does not exist\n");
		//page directory entry is not present
		//implies page table is not present
		//implies page is not present

		//get frame for page table
		int pt_avail;
		if (create_page_table(&pt_avail) == SYSERR) {
			return SYSERR;
		}
		//kprintf("new frame for page table is %d\n", pt_avail);
		//fill pde
		//kprintf("Modifying pd entry at %ld\n", pd);
		pd->pd_pres = 1;
		pd->pd_write = 1;
		pd->pd_user = 0;
		pd->pd_pwt = 0;
		pd->pd_pcd = 0;
		pd->pd_acc = 0;
		pd->pd_mbz = 0;
		pd->pd_fmb = 0;
		pd->pd_global = 0;
		pd->pd_avail = 0;
		pd->pd_base = (FRAME0 + pt_avail);
		//kprintf("page directory pd is %d\n", (proctab[currpid].pdbr/NBPG)-FRAME0);
		//frm_tab[((proctab[currpid].pdbr)/NBPG)-FRAME0].fr_refcnt++;
		//kprintf("dir is pointing to %d frame which is pt\n", pd->pd_base);
		//kprintf("ref count for the pd is %d\n", frm_tab[((proctab[currpid].pdbr)/NBPG)-FRAME0]);

		//get frame for page
		int pg_avail;
		int add_pg = create_page(&pg_avail, vp);
		//kprintf("new fame for page is %d\n", pg_avail);
		//fill pte
		pt = NBPG*(FRAME0 + pt_avail);
		//kprintf("PT starts at %ld\n", pt);
		pt = pt + fault_page.pt_offset;
		//kprintf("Modifying pt entry at %ld\n", pt);
		pt->pt_pres = 1;
		pt->pt_write = 1;
		pt->pt_user = 0;
		pt->pt_pwt = 0;
		pt->pt_pcd = 0;
		pt->pt_acc = 0;
		pt->pt_dirty = 0;
		pt->pt_mbz = 0;
		pt->pt_global = 0;
		pt->pt_avail = 0;
		pt->pt_base = (FRAME0 + pg_avail);
		frm_tab[pt_avail].fr_refcnt++;
		if (add_pg == 1) {
			global_page_add(pg_avail, pt);
		}
		//kprintf("pt is in frame %d\n", pt_avail);
		//kprintf("ref count for the pt is %d\n", frm_tab[pt_avail].fr_refcnt);
	}

	//write the backing store page to the page in RAM
	
	//get backing store id
	int bs_id = 0;
	if(get_bs_id(&bs_id) == SYSERR) {
		return SYSERR;
	}
	//kprintf("backing store id is: %d\n", bs_id);
	
	//get page offset
	int page_offset = get_page_offset(vp, bs_id);
	//kprintf("page offset is: %d\n", page_offset);

	//call read_bs
	read_bs( NBPG*(pt->pt_base), bs_id, page_offset);
	//kprintf("read success\n\n");
	//write pdbr to cr3
	write_cr3(proctab[currpid].pdbr);
	return OK;
}
