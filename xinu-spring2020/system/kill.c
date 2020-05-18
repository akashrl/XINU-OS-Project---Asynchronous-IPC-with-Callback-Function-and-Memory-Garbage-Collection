/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);

	/*************** Added for Lab 5, Section 4 ****************/

	struct inusememblk *stillusedblocks;
	stillusedblocks = prptr->prheaphd->mnext;
	struct inusememblk *tmp, *tmp1;
	tmp = (struct inusememblk *)(prptr->prheaphd);
	tmp1 = prptr->prheaphd;
        tmp1 = tmp1->mnext;
//      kprintf("freemem() is called\n");
/*
        while (tmp1 != NULL) {
                kprintf("Block size %d\n", tmp1->memlen);
                tmp1 = tmp1->mnext;
        }
*/
//	while (stillusedblocks != NULL) {
//		freemem((char *)(stillusedblocks), stillusedblocks->memlen);
//		stillusedblocks = stillusedblocks->mnext;
//	}

//	tmp = tmp->mnext;
/*
	while(tmp != NULL) {
		kprintf("Block size %d\n", tmp->memlen);
		tmp = tmp->mnext;
	}
*/
	/*************** End Lab 5 Section 4 ***************/	
	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
