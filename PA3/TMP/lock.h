/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define	LFREE	'\01'		/* this lock is free		*/
#define	LUSED	'\02'		/* this lock is used		*/

#define	READ	1		/* this lock is read type		*/
#define	WRITE	2		/* this lock is write type		*/

struct	lentry	{		/* lock table entry			*/
	char	lstate;		/* the state LFREE or LUSED		*/
	int	ltype;		/* the type READ or WRITE		*/
	int	lqhead;		/* q index of head of list		*/
	int	lqtail;		/* q index of tail of list		*/
	int	acquired[NPROC];	/* list of processes waiting on lock	*/
	int 	lprocessid;
	int	lmax;
	unsigned long lbirthtime;
};
extern	struct	lentry	lockarray[];
extern	int	nextlck;

#define	isbadlck(l)	(l<0 || l>=NLOCKS)

extern unsigned long ctr1000;

int printqueue(int head, int tail);

int linit();

SYSCALL lcreate();

SYSCALL ldelete(int lck);

SYSCALL lock(int lck, int type, int lock_priority);

SYSCALL releaseall(int numlocks, int ldes1, ...);

#endif
