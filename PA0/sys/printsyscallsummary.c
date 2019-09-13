#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
#include <stdio.h>

int frequency[NPROC][27];

int time[NPROC][27];

int to_print[NPROC];

int syscall_record = 0;

unsigned long ctr1000;

void syscallsummary_start()
{
	syscall_record = 1;
	int i=0, j=0;
	for(i=0;i<27;i++){
		for(j=0;j<27;j++){
            		frequency[i][j] = 0;
            		time[i][j] = 0;
		}
        }
	for(i=0;i<NPROC;i++) {
		to_print[i] = 0;
	}
}

void syscallsummary_stop()
{
	syscall_record = 0;
}

void printsyscallsummary()
{
	int i=0, j=0;
	i = 49;
	for(i=0;i<NPROC;i++) {
		if (to_print[i] == 1) {
			kprintf("Process [pid: %d]\n", i);
			for(j=0;j<27;j++) {
				if (frequency[i][j] != 0){
					kprintf("printing j = %d\n", j);
					kprintf("        Syscall: , count: %d, average execution time: %d (ms)\n", frequency[i][j], time[i][j]/frequency[i][j]);
				}
			}
		}
	}
}
