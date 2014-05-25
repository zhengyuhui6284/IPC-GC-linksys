/**************************************
 * Testcase for Asynchronous msg
 *
 * to use: overwrite main.c, recompile
 * ************************************/

#include <xinu.h>

uint32 recvbuf;

/* User registered function */
int myrecvhandler(void) {
	kprintf("msg received = %d\n", recvbuf);
	return(OK);
}
 
/* Sender */
void sender(pid32 receiver) {
	sleep(3);
	
	if( send(receiver, 20) == SYSERR ) {
		kprintf("Fail to send msg 20!\r\n");
	} else {
		kprintf("Send msg 20 to receiver!\r\n");
	}
	return;
}

/* Receiver */
void receiver(void) {

	if (registerrecv(&recvbuf, &myrecvhandler) != OK) {
		kprintf("recv handler registration failed\n");
		return;
	}
	
	while(1) {
		sleep(1);
	}
	return;
}

/* main */
int main(int argc, char **argv) {
	pid32 spid, rpid;
	rpid = create(receiver, 2014, 20, "receiver", NULL);
	spid = create(sender, 2048, 20, "sender", 1, rpid);
	resume(rpid);
	resume(spid);

	while(1) {
		sleep(100);
	}
	return OK;
}
