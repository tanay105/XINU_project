/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	if (source < 0 || source > 7 || npages <= 0 || npages > 256) {
		return SYSERR;
	}
	if (bsm_tab[source].bs_pid == currpid && bsm_tab[source].bs_npages == npages) {
		bsm_tab[source].bs_vpno = virtpage;
		return OK;
	}
	return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	int store = 0, page_offset = 0;
	if (bsm_lookup(currpid, virtpage, &store, &page_offset) == SYSERR) {
		return SYSERR;
	}

	int npages = bsm_tab[store].bs_npages;
	int i=0;
	for(i=0;i<NFRAMES;i++) {
		if (frm_tab[i].fr_pid == currpid && frm_tab[i].fr_vpno >= virtpage && frm_tab[i].fr_vpno < virtpage + npages) {
			//write back to bs
			write_bs((FRAME0 + i)*NBPG, store, frm_tab[i].fr_vpno - virtpage);
		}
	}

	//if (free_bsm(store) == SYSERR) {
	//	return SYSERR;
	//}	

	return OK;
}
