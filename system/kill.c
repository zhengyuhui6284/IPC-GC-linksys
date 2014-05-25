/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	procent *prptr;		/* ptr to process' table entry	*/
	struct	mb_ent	*mbptr;		/* pointer to heap memlist	*/
	int32	i;			/* index into descriptors	*/
	int 	debug;

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}
	
	/* lab3 - 1 */
	while (nonempty(prptr->tobe_send)) {
		ready(getfirst(prptr->tobe_send), RESCHED_NO);
	}

	/* lab3 - 3 */
	if (prptr->prhasmem == TRUE) {
		mbptr = prptr->prmemlist->mb_next;
	
		while (mbptr != NULL) {
			prptr->prmemlist->mb_next = mbptr->mb_next;
			debug = freemem((char *)mbptr, mbptr->mb_size);
			if (debug == SYSERR) {
				kprintf("[kill]: %s memfree err\r\n", prptr->prname);
			}
			mbptr = prptr->prmemlist->mb_next;
		}

		
		prptr->prhasmsg = FALSE;
		kprintf("[garbage collection]: freed %10d\r\n", prptr->prmemlist->mb_size);
		prptr->prmemlist->mb_size = 0;
		prptr->prmemlist->mb_next = NULL;
	}

	if (--prcount <= 1) {		/* last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* fall through */

	case PR_READY:
		getitem(pid);		/* remove from queue */
		/* fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
