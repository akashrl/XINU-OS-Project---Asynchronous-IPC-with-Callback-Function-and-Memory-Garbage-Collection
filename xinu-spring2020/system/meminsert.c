/* meminsert.c - meminsert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  meminsert  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
bool8 memblockkflag;

void  	meminsert (struct inusememblk *hdptr, char *memptr)
{
	intmask	mask;			/* Saved interrupt mask		*/
	/* Added for Lab 5, Section 4 */
        struct  memblk  *usedblk, *ptr;
        struct  inusememblk *newnode, *temp, *prhdptr;

	ptr = (struct memblk *)memptr;
	prhdptr = (struct inusememblk *)hdptr;
//	kprintf("In meminsert usedblk: %d\n", ptr->mlength);
//	kprintf("In meminsert prhdptr: %d, %d\n", *prhdptr, prhdptr);


	mask = disable();
	memblockkflag = 1;
	newnode = (struct inusememblk *)getmem(sizeof(struct inusememblk));
	if (newnode == NULL) {
//		kprintf("In meminsert I am newnode NULL\n");
	}
	newnode->memblockptr = ptr;
	newnode->memlen = ptr->mlength;
	newnode->mnext = NULL;
//	kprintf("In meminsert(), newnode->memlen: %d\n", newnode->memlen);
//	kprintf("In meminsert(), newnode->memblockptr: %d\n", newnode->memblockptr);
//	kprintf("Checking prhdptr->mlen: %d\n", prhdptr->memlen);

	if (prhdptr->mnext == NULL) {
//		kprintf("In meminsert(), firstnode prthdptr->mlen is NULL\n");
		prhdptr->mnext = newnode;
	}

	else {
//		kprintf("In meminsert(), firstnode prthdptr->mlen NOT NULL\n");
		newnode->mnext = prhdptr->mnext;
		prhdptr->mnext = newnode;
	}

	restore(mask);
	return (char *)SYSERR;

}
