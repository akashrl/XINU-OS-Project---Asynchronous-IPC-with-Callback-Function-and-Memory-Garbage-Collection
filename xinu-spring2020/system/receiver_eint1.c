/* receiver_eint1.c - receiver_eint1 */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receiver_eint1  - 
 *------------------------------------------------------------------------
 */
void	receiver_eint1(void)
{
//	int x;
//	x = 5;

	umsg32 buffer;
	void usercb(void);

	kprintf("T currpid of testreceiver: %d\n", currpid);
/*
        kprintf("T usercb address + 0: (0x%08x)\n", &usercb + 0); 
        kprintf("T usercb address + 1: (0x%08x)\n", &usercb + 1); 
        kprintf("T usercb address + 2: (0x%08x)\n", &usercb + 2); 
        kprintf("T usercb address + 3: (0x%08x)\n", &usercb + 3); 
*/
	if (cbregister(&usercb, &buffer) != OK) {
		kprintf("userb registration failed\n");
		return;
	}

//        while(TRUE) {
        	for (int i = 0; i <= 2000; i++) {
    
                	kprintf("A");

        	}
//	}

/*
	while(TRUE) {
		sleepms(300);
	}
*/
}



//	intmask	mask;			/* Saved interrupt mask		*/
//	struct	procent *prptr;		/* Ptr to process's table entry	*/
//	umsg32	msg;			/* Message to return		*/

//	mask = disable();
//	prptr = &proctab[currpid];
//	if (prptr->prhasmsg == FALSE) {
//		prptr->prstate = PR_RECV;
//		resched();		/* Block until message arrives	*/
//	}
//	msg = prptr->prmsg;		/* Retrieve message		*/
//	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
//	restore(mask);
//	return msg;
//}
