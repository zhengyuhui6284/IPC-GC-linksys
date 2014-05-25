/* **************************************
 * Testcase for blocked msg passing
 *
 * to use: overwrite main.c, recompile
 * **************************************/

#include <xinu.h>
#include <stdio.h>
#include <string.h>

uint32 recvbuf;		/* lab3-2: extern uint32 recvbuf */

void sender(pid32 receiver) {
	uint32 i;
	intmask mask;
	for(i=0; i<15; i++) {
		if( sendb(receiver, i) == SYSERR ) {
			mask = disable();
			kprintf("[sender]: Fail to send msg %d!\r\n", i);
			restore(mask);
		} else {
			mask = disable();
			kprintf("[sender]: Send msg %d to receiver!\r\n", i);
			restore(mask);
			}
		}
	return;
}

void receiver(void) {
	int i;
	uint32 msg;
	intmask mask;

	for(i=0; i<15; i++) {
		msg = receiveb();
		if( msg == SYSERR ) {
			mask = disable();
			kprintf("[receiver]: Fail to recieve msg!\r\n");
			restore(mask);
		} else {
			mask = disable();
			kprintf("[receiver]: Recieve msg %d from sender!\r\n", msg);
			restore(mask);
		}
		sleep(1);
	}
	return;
}
 
int main(int argc, char **argv) {
	pid32 spid, rpid;
	rpid = create(receiver, 2014, 20, "receiver", NULL);
	spid = create(sender, 2048, 20, "sender", 1, rpid);
	resume(rpid);
	resume(spid);

	sleep(20);
	kprintf("[main]: finished\r\n");
	while(1) {
		sleep(100);
	}
	return OK;
}
