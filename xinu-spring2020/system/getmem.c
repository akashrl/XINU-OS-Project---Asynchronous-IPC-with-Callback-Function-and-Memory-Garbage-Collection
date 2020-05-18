/* getmem.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	/* Added for Lab 5, Section 4 */
	struct	memblk	*allocatedblk;
	struct	procent	*prptr = &proctab[currpid];
	struct	inusememblk *hdptr;

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;

			hdptr = prptr->prheaphd; // Added for Lab 5, Section 4
			curr->mlength = nbytes;
			allocatedblk = (char *)curr; // Added for Lab 5 Section 4
//			kprintf("In getmem allocatedblk: %d\n", allocatedblk->mlength);
//			kprintf("In getmem curr1->mlength: %d\n", curr->mlength);
//			kprintf("In getmem hdptr: %d, %d\n", *hdptr, hdptr);

			if (memblockkflag != 1) {
				meminsert(hdptr, allocatedblk); // Added for Lab 5, Section 4
			}

			memblockkflag = 0;
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
			hdptr = prptr->prheaphd; // Added for Lab 5, Section 4
			curr->mlength = nbytes;
			allocatedblk = (char *)curr; // Added for Lab 5 Section 4
//			kprintf("In getmem allocatedblk: %d\n", allocatedblk->mlength);
//			kprintf("In getmem curr2->mlength: %d\n", curr->mlength);
//			kprintf("In getmem hdptr: %d, %d\n", *hdptr, hdptr);

			if (memblockkflag != 1) {
				meminsert(hdptr, allocatedblk); // Added for Lab 5, Section 4
			}

			memblockkflag = 0;
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}
