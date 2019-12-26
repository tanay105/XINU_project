#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}

void p1 (char *msg, int sem)
{
	wait(sem);
	kprintf ("  %s: acquired sem, sleep 10\n", msg);
	sleep(3);
	kprintf ("  %s: to release sem\n", msg);
	signal (sem);
}

void p3 (char *msg, int lck)
{
        lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired sem, sleep 10\n", msg);
	sleep(1);
	kprintf ("  %s: to release sem\n", msg);
        releaseall (1, lck);
}

void p4 (char *msg, int lck)
{
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired sem, sleep 10\n", msg);
	sleep(1);
	kprintf ("  %s: to release sem\n", msg);
        releaseall (1, lck);
}

void p2 (char *msg, int sem)
{
	int i = 0;
	char s = 'P';
	kprintf ("  %s: starting to print chars\n", msg);
	while (i < 100) {
		kprintf("%c", s);
		i++;
	}
	kprintf("\n");
	sleep(2);
	i=0;
	while (i < 100) {
		kprintf("%c", s);
		i++;
	}
	kprintf("\n");
	kprintf ("  %s: ending to print chars\n", msg);
}

void test1() {
	int sem;

        int     sem1, sem2, sem3;

	kprintf("Test using semaphores first\n");

	sem = screate(1);
        sem1 = create(p1, 2000, 20, "reader1", 2, "process A", sem);
        sem2 = create(p2, 2000, 35, "reader1", 2, "process B", sem);
        sem3 = create(p1, 2000, 50, "reader1", 2, "process C", sem);

	kprintf("Starting process A\n");
	resume(sem1);
	kprintf("Starting process B\n");
	resume(sem2);
	kprintf("Starting process C\n");
	resume(sem3);
	sleep(10);

	kprintf("\n***************************\n");

}

void test2() {
	int lck;
        int     lck1, lck2, lck3;
		
	kprintf("\nTest using locks first\n");

	lck = lcreate();
        lck1 = create(p3, 2000, 20, "reader1", 2, "process A", lck);
        lck2 = create(p2, 2000, 35, "reader1", 2, "process B", lck);
        lck3 = create(p4, 2000, 50, "reader1", 2, "process C", lck);
	
	kprintf("Starting process A\n");
	resume(lck1);
	kprintf("Starting process B\n");
	resume(lck2);
	kprintf("Starting process C\n");
	resume(lck3);
	sleep(10);
	
	kprintf("\n***************************\n");

}

int main() {

	test1();
	sleep(1);
	test2();
	sleep(1);
}
