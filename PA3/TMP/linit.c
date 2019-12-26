#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>

struct	lentry	lockarray[NLOCKS];	/* lock table		*/
int nextlck;

int linit() {
	int i=0;
	int j=0;
	for(i=0;i<NLOCKS;i++) {
		lockarray[i].lstate = LFREE;
		lockarray[i].lqtail = 1 + (lockarray[i].lqhead = newqueue());
		for (j=0;j<NPROC;j++) {
			lockarray[i].acquired[j] = -1;
		}
		lockarray[i].lmax = 0;
	}
	nextlck = NLOCKS - 1;
	return 0;
}
