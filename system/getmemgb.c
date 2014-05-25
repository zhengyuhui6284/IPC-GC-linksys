/* getmemgb.c - getmem with garbage collection */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmemgb  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmemgb(
	  uint32	nbytes		/* size of memory requested	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	struct	procent	*prptr;
	struct	mb_ent	*mbptr;
	bool8	found_mb = FALSE;
	uint32	appendix = (uint32) roundmb(sizeof(mb_ent));

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* use memblk multiples	*/
	nbytes = nbytes + appendix;		/* add. space for gc	*/	
	
	prev = &memlist;
	curr = memlist.mnext;

	prptr = &proctab[currpid];

	while (curr != NULL) {			/* search free list	*/

		if (curr->mlength == nbytes) {	/* block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
			found_mb = TRUE;
			break;
			//restore(mask);
			//return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
			found_mb = TRUE;
			break;
			//restore(mask);
			//return (char *)(curr);
		} else {			/* move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}

	/* lab3 - 3 modify memlist */
	if (!found_mb) {
		restore(mask);
		return (char *)SYSERR;
	} else {
		prptr->prhasmem = TRUE;
		
		mbptr = (mb_ent*)curr;
		mbptr->mb_next = prptr->prmemlist->mb_next;
		mbptr->mb_size = nbytes;
		
		prptr->prmemlist->mb_next = mbptr;
		prptr->prmemlist->mb_size = prptr->prmemlist->mb_size + nbytes;	
	
		restore(mask);
		return (char *)(curr + appendix);
	}
}
