/* Header file for pa1 functions*/
#ifndef _LAB_H_
#define _LAB_H_

#define EXPDISTSCHED 1 
#define LINUXSCHED 2 

extern int global_sched_flag;

extern int global_init_flag;

void setschedclass(int);

int getschedclass();

int getexpdistnextproc(int random, int head);

int get_next_expdist_proc(int random, int head);

void abc(int head);

int get_epoch_value();

int get_queue_count(int head);

void initialize_quantum_goodness();

void global_initialize_quantum_goodness();

int get_next_goody_process_from_queue();

int find_max_goodness();

int total_goodness();

int dequeue_goody_process();

#endif
