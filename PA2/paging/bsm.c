/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[8];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	//STATWORD ps;    
	int i=0;
	//disable(ps);
	for (i=0; i<9; i++) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].bs_npages = -1;
		bsm_tab[i].bs_sem = -1;
	}
	//restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	//STATWORD ps;    
	int i;
	//disable(ps);
	for (i=0;i<8;i++) {
		if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
			*avail = i;
			//restore(ps);
			return OK;
		}
	}
	// return error when nothing is found
	//restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	//STATWORD ps;    
	//disable(ps);
	if (bsm_tab[i].bs_status == BSM_MAPPED) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].bs_npages = -1;
		bsm_tab[i].bs_sem = -1;
		//restore(ps);
		return OK;
	}
	else {
		// return error when nothing is found
		//restore(ps);
		return SYSERR;
	}
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	//STATWORD ps;    
	//disable(ps);
	//NOT IMPLEMENTED YET
	//looking for matching pid and vaddr in the corresponding 
	//store to return the offset of pageth page.
	int i;
	for(i=0; i<8; i++) {
		if (bsm_tab[i].bs_pid == pid){
			//return what is req when you understand this
			*store = i;
			*pageth = vaddr/NBPG - bsm_tab[i].bs_vpno;
			//restore(ps);
			return OK;
		}
	}
	// returning error when nothing is found
	//restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	//STATWORD ps;    
	//disable(ps);
	//assuming source is the backing store id
	//assume called only once so assigning everything right now
	
	if (bsm_tab[source].bs_status == BSM_MAPPED || source<0 || source>8) {
		//restore(ps);
		return SYSERR;
	}	

	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid = pid;
	bsm_tab[source].bs_vpno = vpno;
	bsm_tab[source].bs_npages = npages;
	bsm_tab[source].bs_sem = -1;
	//restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	//STATWORD ps;    
	//flag is unkown in this implementation
	//unmap the bs that has the matching pid and vpno both
	int i;
	//disable(ps);
	for(i=0; i<8; i++) {
		if (bsm_tab[i].bs_pid == pid && bsm_tab[i].bs_vpno == vpno){
			bsm_tab[i].bs_status = BSM_UNMAPPED;
			bsm_tab[i].bs_pid = -1;
			bsm_tab[i].bs_vpno = -1;
			bsm_tab[i].bs_npages = -1;
			bsm_tab[i].bs_sem = -1;
			//restore(ps);
			return OK;
		}
	}
	//restore(ps);
	return SYSERR;
}

