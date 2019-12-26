#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
	if (bs_id >=0 && bs_id < 8 && bsm_tab[bs_id].bs_pid == currpid && bsm_tab[bs_id].bs_status == BSM_MAPPED) {
		free_bsm(bs_id);
		return OK;
	}
	return SYSERR;

}

