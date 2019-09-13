/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

int prX;
void halt();

prch(c)
char c;
{
	int i;
	getpid();	
}

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	kprintf("\n$$$ Problem 1: zfunction $$$\n");
	long a = zfunction(0xaabbccdd);
	kprintf("Return from zfunction is 0x%x\n", a);
	kprintf("\n$$$ Problem 2: printsegaddress $$$\n");
	printsegaddress();
	kprintf("\n$$$ Problem 3: printtos $$$\n");
	printtos();
	//resume(prX = create(prch,2000,20,"proc X",1,'A'));
	kprintf("\n$$$ Problem 4: printprocstks $$$\n");
	printprocstks(1);
	kprintf("\n$$$ Problem 5: printsyscallsummary $$$\n");
	syscallsummary_start(); 
        kprintf("number of processes is %d\n", numproc);
	//resume(prX = create(prch,2000,20,"proc X",1,'A'));
	getpid();
	sleep(1);
        kprintf("number of processes is %d\n", numproc);
	syscallsummary_stop();
	printsyscallsummary();

	return 0;
}
