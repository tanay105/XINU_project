#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

	/* requests a new mapping of npages with ID map_id */
	//return ERR when allocated already by a vcreate
	//return the number of pages if allocated by a different 
	//backing store already
	if (npages <= 0 || npages > 256 || bs_id < 0 || bs_id > 7) {
		return SYSERR;
	}
	if (bsm_tab[bs_id].bs_status == BSM_MAPPED && bsm_tab[bs_id].bs_sem == 1) {
		kill(currpid);
		return SYSERR;
	}

	if (bsm_tab[bs_id].bs_status == BSM_UNMAPPED) {
		//backing store is not assigned
		if (bsm_map(currpid, -1, bs_id, npages) != OK) {
			return SYSERR;
		}
	}
	else {
		//backing store is unassigned
	}
	proctab[currpid].store = bs_id;
	return npages;
}

