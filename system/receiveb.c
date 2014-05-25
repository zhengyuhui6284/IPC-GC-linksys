/* receiveb.c - receive with blocking message passing */

#include <xinu.h>

/* ----------------------------------------------------
 * receiveb - blocking message receive
 * NOTICE: 
 * ---------------------------------------------------*/

umsg32 receiveb(void) {

	intmask	mask;
	struct	procent *prptr;
	umsg32	msg;
//	uint32	flag;

	mask = disable();
	prptr = &proctab[currpid];

//	flag = prptr->prhasmsgb;	/* here flag may cause error */
//					/* if & switch are paralelled by compiler*/

	/* no message to read - block self*/
	if (prptr->prhasmsgb == 0) {
		prptr->prstate = PRREC;
		resched();
	}

	switch (prptr->prhasmsgb) {
	
		case 1:
			/* read */
		//kprintf("[receiveb]: case 1\r\n");
			msg = prptr->prmsgb[0];
			prptr->prhasmsgb = 0;
			restore(mask);
			return msg;
		case 2:
			/* read */
		//kprintf("[receiveb]: case 2\r\n");
			msg = prptr->prmsgb[0];
			prptr->prmsgb[0] = prptr->prmsgb[1];
			prptr->prhasmsgb = 1;
			restore(mask);
			return msg;
		case 3:
			/* see if any sender is waiting vacancy */
		//kprintf("[receiveb]: case 3\r\n");
			msg = prptr->prmsgb[0];
			prptr->prmsgb[0] = prptr->prmsgb[1];
			prptr->prmsgb[1] = prptr->prmsgb[2];
			prptr->prhasmsgb = 2;
			
			if (prptr->tobe_send != -1) {
				if (nonempty(prptr->tobe_send)) {
//kprintf("[receiveb]: case 3 - wake up queuehead\r\n");
					ready(getfirst(prptr->tobe_send), RESCHED_NO);
				}
			}

//kprintf("[receiveb]: case 3 - prptr->prhasmsgb: %d\r\n", prptr->prhasmsgb);
			restore(mask);
			return msg;
		case 0:
			kprintf("[receiveb]:enterred 0\r\n");
		default:
			restore(mask);
			return SYSERR;
	}

}
