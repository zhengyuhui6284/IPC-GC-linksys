/* sendb.c - send may be blocked */

#include <xinu.h>


/* ---------------------------------------------------------------
 * sendb - blocking message send 
 * NOTICE: sender(current process) is calling this function, 
 * 		checking how many messages in buffer.
 * --------------------------------------------------------------*/

syscall sendb(pid32 pid, umsg32 msg) {

	intmask	mask;
	struct procent	*prptr;

	mask = disable();

	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if ((prptr->prstate == PR_FREE) || (prptr->prhasmsgb > 3)) {
		restore(mask);
		return SYSERR;
	}

	
	/* buffer is full. block self! */ 
	if (prptr->prhasmsgb == 3) {			
		proctab[currpid].prstate = PRSND;

		if (prptr->tobe_send == -1) {
			restore(mask);
			return SYSERR;
		}
		enqueue(currpid, prptr->tobe_send);
		resched();
	}

	switch (prptr->prhasmsgb) {
		case 0:
			/* no message in buffer. check whether to wake up any receiver! */
			
		//kprintf("[sendb]: case 0 \r\n");
			prptr->prmsgb[0] = msg;
			prptr->prhasmsgb = 1;

			if (prptr->prstate == PRREC) {
				ready(pid, RESCHED_NO);
//kprintf("[sendb]: case0 - wake up receiver\r\n");
			}

			break;
		
		case 1: 
			/* no additional procedure */
			
		//kprintf("[sendb]: case 1 \r\n");
			prptr->prmsgb[1] = msg;
			prptr->prhasmsgb = 2;
			
			break;
		
		case 2:
			/* no additional procedure */
			
		//kprintf("[sendb]: case 2 \r\n");
			prptr->prmsgb[2] = msg;
			prptr->prhasmsgb = 3;

			break;
		
		case 3: 
		default:
		//kprintf("[sendb]: default \r\n");
			restore(mask);
			return SYSERR;
	}

	restore(mask);
	return OK;
}

