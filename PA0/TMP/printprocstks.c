#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
#include <stdio.h>

unsigned long *stackptr;

void printprocstks(int priority)
{
	asm("movl %esp, stackptr");
	int i=0;
	while(i < NPROC) {
                if (((&proctab[i])->pstate != PRFREE) && ((&proctab[i])->pprio > priority)) 
                {
       		 	kprintf("Process [%s]\n", (&proctab[i])->pname);
       		 	kprintf("        pid: %d\n", i);
       		 	kprintf("        priority: %d\n", (&proctab[i])->pprio);
       		 	kprintf("        base: 0x%x\n", (&proctab[i])->pbase);
       		 	kprintf("        limit: 0x%x\n", (&proctab[i])->plimit);
       		 	kprintf("        len: %d\n", (&proctab[i])->pstklen);
       		 	if (i == currpid) {
				kprintf("        pointer: 0x%x\n", stackptr);
			}
			else {
				kprintf("        pointer: 0x%x\n", (&proctab[i])->pesp);
			}
                }
		i++;
        }
}
