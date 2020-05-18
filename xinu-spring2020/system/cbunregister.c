/* cbunregister.c - cbunregister */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  cbunregister  -  Added for Lab 5, Part 3
 *------------------------------------------------------------------------
 */

syscall cbunregister(void) {
	intmask mask;
	mask = disable();

	struct procent *prptr = &proctab[currpid];
	if (prptr->prcbvalid == 0) { // If not already registered, return SYSERR
		restore(mask);
		return SYSERR;
	}

	prptr->prcbvalid = 0; // Callback function unregistered successfully	
	restore(mask);
	return OK;
}
