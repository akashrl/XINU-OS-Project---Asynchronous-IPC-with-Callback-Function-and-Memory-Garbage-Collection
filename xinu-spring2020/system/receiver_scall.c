/* receiver_scall.c - receiver_scall */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receiver_scall  - 
 *------------------------------------------------------------------------
 */
void	receiver_scall(void)
{

	umsg32 ubuffer;
	void usercb(void);

//	kprintf("T currpid of testreceiver: %d\n", currpid);

	if (cbregister(&usercb, &ubuffer) != OK) {
		kprintf("userb registration failed\n");
		return;
	}

//	kprintf("\nbefore sleep\n");
	sleepms(300);
	kprintf("\n(Test Case, Not Debug Statement) I am receiver continuing my tasks after sleep\n");
//	sleepms(200);
//	kprintf("\nafter second sleep\n");

/*
	while(TRUE) {
		sleepms(300);
	}
*/
}
