/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */

void usercb(void) {

// kprintf("I am in usercb\n");

        intmask mask;
        struct procent *prptr;
	umsg32 ubuffer;
        umsg32 msg;

        mask = disable();
        prptr = &proctab[currpid];
        
	if (prptr->prtmpvalid == 1) {
		(prptr->prmbufptr) = prptr->prtmpbuf;
		ubuffer = prptr->prmbufptr;
		prptr->prtmpvalid == 0;
		kprintf("MESSAGE RECEIVED IN USERCB IS: %s\n", ubuffer);
		restore(mask);
	}

}
