#include <stdio.h>
//#include <proc.h>
//#include <kernel.h>
#include <lab0.h>
//#include <conf.h>

//static unsigned long *esp;
static unsigned long *ebp;

void printtos()
{
	unsigned long *nextstackptr, *nextframeptr, *prevstackptr;
	//asm("movl %esp,esp");
	asm("movl %ebp,ebp");
	//nextstackptr = esp;
	nextframeptr = ebp;
	//prevstackptr = *nextframeptr;

	kprintf("Before[0x%08x]: 0x%08x\n", nextframeptr + 2, *(nextframeptr + 2));
	kprintf("After [0x%08x]: 0x%08x\n", nextframeptr, *(nextframeptr));

	kprintf("        element[0x%08x]: 0x%08x\n",nextframeptr-1, *(nextframeptr-1));
	kprintf("        element[0x%08x]: 0x%08x\n",nextframeptr-2, *(nextframeptr-2));
	kprintf("        element[0x%08x]: 0x%08x\n",nextframeptr-3, *(nextframeptr-3));
	kprintf("        element[0x%08x]: 0x%08x\n",nextframeptr-4, *(nextframeptr-4));
}
