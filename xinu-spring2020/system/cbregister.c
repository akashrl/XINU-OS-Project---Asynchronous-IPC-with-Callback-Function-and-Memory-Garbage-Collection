/* cbregister.c - cbregister */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  cbregister  -  Added for Lab 5, Part 3
 *------------------------------------------------------------------------
 */

syscall cbregister(void (* fp)(void), umsg32 *mbufptr) {
	intmask mask;
	mask = disable();
	struct procent *prptr = &proctab[currpid];

	/* Registration should fail if a process tries to register second callback function */
	if (prptr->prcbvalid == 1) { // If not already registered, return SYSERR
                restore(mask);
                return SYSERR;
        }   

	/* Registration should fail if fp or mbufptr out of memory bounds */
	if ((uint32)fp < (uint32)&text || (uint32)fp > (uint32)&etext) {
		restore(mask);
		return SYSERR;
	}
	if ((uint32)mbufptr > (uint32)&text && (uint32)mbufptr < (uint32)&etext) {
		restore(mask);
		return SYSERR;
	}

/*	
	if (prptr->prcbvalid == 1) {
		// return an error indicating that registration has failed
		cbunregister();
	}
*/
	prptr->prcbptr = *fp; // fp is a function pointer to a user callback function
	prptr->prmbufptr = mbufptr; // mbufptr is a pointer to a 1-word message buffer
	prptr->prcbvalid = 1; // Callback function registered successfully

//      kprintf("Range of text segment: [0x%08X to 0x%08X]\n", (uint32)&text, (uint32)&etext - 1);

//	kprintf("Location of *fp pointer: 0x%08X\n", fp);

//	kprintf("Location of mbufptr pointer: 0x%08X\n", mbufptr);

	restore(mask);
	return OK;
}
