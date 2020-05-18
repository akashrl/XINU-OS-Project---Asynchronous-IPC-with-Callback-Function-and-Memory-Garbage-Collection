/* mysend.c - mysend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  mysend  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	mysend(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	void usercb(void);

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
//	kprintf("In send 2, received PID: %d\n", pid);
//	kprintf("In send 2, received message: %s\n", msg);
	
//	kprintf("currpid of send: %d\n", currpid);
//	kprintf("pid received in send: %d\n", pid);
	prptr = &proctab[pid];
//	kprintf("In send prptr->prcbptr: (0x%08x)\n", prptr->prcbptr);
//	kprintf("In send *(prptr->prcbptr): (0x%08x)\n", *(prptr->prcbptr));


        /* ******** Lab 5 Section 3 Case (i) Tests ******** */

	/* If a message waiting and not processed, discard the new message */
	if (prptr->prcbvalid == 1 && prptr->prtmpvalid == 1) {
		restore(mask);
		return SYSERR;
	}

	prptr->prtmpvalid = 1;
	prptr->prtmpbuf = msg;

	if (prptr->prstate == PR_SLEEP) { // Case 1

        	// return address manipulation start ***************
        	int stkframes = 3; // Number of nested function calls while calling sleepms()

        	unsigned int * retaddr = prptr->prstkbase; // Search through base pointers to find return address
        	unsigned int * prevbaseptr = prptr->prstkbase; // Search through base pointers to find return address

        	while (stkframes > 0) { // Loop through to find third stack frame return address
                	while (*retaddr != prevbaseptr) {
                        	retaddr = retaddr - 1;
               		}
                	prevbaseptr = retaddr;
                	stkframes = stkframes - 1;
        	}

        	retaddr = retaddr + 1; // Copy return address one below base pointer

        	*(retaddr) = prptr->prcbptr + 3; // Replace return address with function's address
       		// return address manipulation end ********************

	}



//	kprintf("In send, my current state is %d\n", prptr->prstate);



        /* ******** Lab 5 Section 3 Case (ii) Tests ******** */
/*
        if (prptr->prstate == PR_READY) { // Case 2

                // return address manipulation start ***************
                int stkframes = 2; // Number of nested function calls while calling sleepms()

                unsigned int * retaddr = prptr->prstkbase; // Search through base pointers to find return address
                unsigned int * prevbaseptr = prptr->prstkbase; // Search through base pointers to find return address

                while (stkframes > 0) { // Loop through to find third stack frame return address
                        while (*retaddr != prevbaseptr) {
                                retaddr = retaddr - 1;
                        }   
                        prevbaseptr = retaddr;
                        stkframes = stkframes - 1;
                }   

                retaddr = retaddr + 1; // Copy return address one below base pointer

//                *(retaddr) = &usercb + 5;
                // return address manipulation end ********************
	}
*/



	
	if (prptr->prstate == PR_READY) {
			// Add	
	}

	if (prptr->prcbvalid == 1 && prptr->prtmpvalid == 1) {
		// Add	
	}

	/* End Lab 5 code */

/*
//	 If recipient waiting or in timed-wait make it ready


	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
*/
	restore(mask);		/* Restore interrupts */
	return OK;
}
