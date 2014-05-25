/* freememgb.c - freemem with garbage collection */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  freememgb  -  Free a memory block, returning the block to the free list
 *------------------------------------------------------------------------
 */
syscall	freememgb(
	  char		*blkaddr,	/* pointer to memory block	*/
	  uint32	nbytes		/* size of block in bytes	*/
	)
{
	intmask	mask;			/* saved interrupt mask		*/
	struct	memblk	*next, *prev, *block;
	uint32	top;

	struct	procent	*prptr;
	struct	mb_ent	*mbptr, *mbptr_prev;
	uint32	appendix = (uint32) roundmb(sizeof(mb_ent));
	bool8	found_mb = FALSE;

	mask = disable();
	if ((nbytes == 0) || ((uint32) blkaddr < (uint32) minheap)
			  || ((uint32) blkaddr > (uint32) maxheap)) {
		restore(mask);
		return SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes) + appendix;	/* use memblk multiples	*/
	block = (struct memblk *)(blkaddr - appendix);

	prev = &memlist;			/* walk along free list	*/
	next = memlist.mnext;
	while ((next != NULL) && (next < block)) {
		prev = next;
		next = next->mnext;
	}

	if (prev == &memlist) {		/* compute top of previous block*/
		top = (uint32) NULL;
	} else {
		top = (uint32) prev + prev->mlength;
	}

	/* Insure new block does not overlap previous or next blocks	*/

	if (((prev != &memlist) && (uint32) block < top)
	    || ((next != NULL)	&& (uint32) block+nbytes>(uint32)next)) {
		restore(mask);
		return SYSERR;
	}

	/* find block in prptr->prmemlist, if not return err */
	prptr = &proctab[currpid];
	mbptr_prev = prptr->prmemlist;
	mbptr = prptr->prmemlist->mb_next;
	
	while (mbptr != NULL) {
		if (mbptr == (mb_ent*)block) {
			found_mb = TRUE;
			break;
		}
		mbptr_prev = mbptr;
		mbptr = mbptr->mb_next;
	}

	if (found_mb == FALSE) {
		/* not allocated by currpid */
		restore(mask);
		return SYSERR;
	}

	if (mbptr->mb_size != nbytes) {
		/* size not equal */
		restore(mask);
		return SYSERR;
	}

	mbptr_prev->mb_next = mbptr->mb_next;
	mbptr_prev->mb_size = mbptr->mb_size;
	
	/* put back to free list */
	memlist.mlength += nbytes;

	/* Either coalesce with previous block or add to free list */

	if (top == (uint32) block) { 	/* coalesce with previous block	*/
		prev->mlength += nbytes;
		block = prev;
	} else {			/* link into list as new node	*/
		block->mnext = next;
		block->mlength = nbytes;
		prev->mnext = block;
	}

	/* Coalesce with next block if adjacent */

	if (((uint32) block + block->mlength) == (uint32) next) {
		block->mlength += next->mlength;
		block->mnext = next->mnext;
	}
	restore(mask);
	return OK;
}
