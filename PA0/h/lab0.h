/*
 */

#ifndef _LAB0_H_
#define _LAB0_H_

#define FREEMEM     0
#define CHPRIO      1
#define GETPID      2
#define GETPRIO     3
#define GETTIME     4
#define KILL        5
#define RECEIVE     6
#define RECVCLR     7
#define RECVTIM     8
#define RESUME      9
#define SCOUNT      10
#define SDELETE     11
#define SEND        12
#define SETDEV      13
#define SETNOK      14
#define SCREATE     15
#define SIGNAL      16
#define SIGNALN     17
#define SLEEP       18
#define SLEEP10     19
#define SLEEP100    20
#define SLEEP1000   21
#define SRESET      22
#define STACKTRACE  23
#define SUSPEND     24
#define UNSLEEP     25
#define WAIT        26

extern unsigned long ctr1000; 

extern int syscall_record;

extern int frequency[][27];

extern int time[][27];

extern int to_print[];

void printsegaddress();

void entryfunction();

void printtos();

void printprocstks(int);

void syscallsummary_start();

void syscallsummary_stop();

void printsyscallsummary();

#endif
