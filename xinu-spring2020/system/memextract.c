#include <xinu.h>

/*------------------------------------------------------------------------
 *  memextract  - 
 *------------------------------------------------------------------------
 */
void  	memextract (struct inusememblk *hdptr, char *memptr)
{
	intmask	mask;			/* Saved interrupt mask		*/

	/* Added for Lab 5, Section 4 */
        struct  memblk  *usedblk, *ptr;
        struct  inusememblk *newnode, *temp, *prhdptr;
	struct	inusememblk *usedprev, *usedcurr;

	ptr = (struct memblk *)memptr;
	prhdptr = (struct inusememblk *)hdptr;
	temp = prhdptr;
	temp = temp->mnext;

	mask = disable();
	usedprev = prhdptr;
	usedcurr = prhdptr->mnext;

	while (usedcurr != NULL) {
		if ((uint32)(usedcurr->memblockptr) == (uint32)(ptr)) {
//			kprintf("In memextract, MATCHED BLOCK: %d\n", ptr);
			break;
		}
		usedprev = usedcurr;
		usedcurr = usedcurr->mnext;
	}
	usedprev->mnext = usedcurr->mnext;
	
	restore(mask);
	return (char *)SYSERR;

}
