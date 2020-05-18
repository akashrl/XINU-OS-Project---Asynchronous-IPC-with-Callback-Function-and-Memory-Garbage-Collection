/* userbuf.c - userbuf */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  userbuf  - Copy the message into userbuffer 
 *------------------------------------------------------------------------
 */

void userbuf(void) {

kprintf("I am userbuf\n");
kprintf("userbuf currpid: %d\n", currpid);

        intmask mask;
        struct procent *prptr;
        umsg32 msg;

//        mask = disable();
        prptr = &proctab[currpid];
        
	if (prptr->prtmpvalid == 1) {
		(prptr->prmbufptr) = prptr->prtmpbuf;
		prptr->prtmpvalid == 0;
//		kprintf("From usercb, message in prmbufptr is %s\n", prptr->prmbufptr);
//		kprintf("From usercb, message in prtmpbuf is %s\n", prptr->prtmpbuf);
//		restore(mask);
	}

}
