/* lab.h - header file for miscellaneous functions for pa2 */

#ifndef _LAB_H_
#define _LAB_H_

int get_bs_id(int *id);

int get_page_offset(int vp, int bs_id);

int init_global_pages();

int init_pd_for_null();

int init_default_pde(pd_t *pd, int pd_avail);

int init_pd();

int init_pt();

int init_pd_and_pt();

int add_to_sc(int pg, pt_t *pt);

#endif

