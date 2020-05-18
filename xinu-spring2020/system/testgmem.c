/* testgmem.c - testgmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  testgmem  - Test cases for Lab 5 Section 4 
 *------------------------------------------------------------------------
 */
void  	testgmem (void)
{
	char	*test64, *test128, *test256, *test512, *test1024, *test2048;
        struct  procent *prptr = &proctab[currpid];
	struct	inusememblk *tmp, *tmp1;
	tmp = (struct inusememblk *)prptr->prheaphd;
	tmp1 = (struct inusememblk *)prptr->prheaphd;

	test64 = getmem(64);
	test128 = getmem(128);
	test256 = getmem(256);
	test512 = getmem(512);
	test1024 = getmem(1024);
	test2048 = getmem(2048);

//	kprintf("In testgem - *test64: %d, %d\n", *(uint32 *)test64, (uint32 *)test64);

	kprintf("\nTest Case Below Assigned Blocks Before and After Cleanup of a Block\n");

	tmp = tmp->mnext;
	while (tmp != NULL) {
		kprintf("Block size %d\n", tmp->memlen);
		tmp = tmp->mnext;
	}

	freemem(test512, 512);

	kprintf("freemem() is called for 512\n");

	tmp1 = tmp1->mnext;
	while (tmp1 != NULL) {
		kprintf("Block size %d\n", tmp1->memlen);
		tmp1 = tmp1->mnext;
	}	
}
