/* registerrecv.c - asynchronous message receive */

#include <xinu.h>

#define HIGHEST_PRIO	100


/********************************
 * registerrecv - 
 * *****************************/

syscall registerrecv(umsg32 *a_message, int (*callback_addr)(void)) {

	intmask mask;
	struct procent	*prptr;
	cbent	*cbptr;
	mask = disable();

	prptr = &proctab[currpid];
	cbptr = (cbent*)callback_ent[currpid];

	if ((a_message == NULL) || (callback_addr == NULL)) {
		restore(mask);
		return SYSERR;
	}

	cbptr->async_msg = *a_message;
	cbptr->callback_func = callback_addr;
	
	prptr->prasync = TRUE;

	restore(mask);
	return OK;
}

