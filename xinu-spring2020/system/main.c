/*  main.c  - main */

#include <xinu.h>

void receiver_eint1(void);
void receiver_eint2(void);
void receiver_scall(void);

void testgmem(void);
//extern bool8 memblockkflag = 0;

process	main(void)
{

	// Experimental code to trigger interrupt 0 by divide by 0
	// int x = 8/0;
	// kprintf("\nMAIN IS RUNNING\n");    
    	kprintf("\nI'm the first XINU app and running function main() in system/main.c.\n");
    	kprintf("\nI was created by nulluser() in system/initialize.c using create().\n");
    	kprintf("\nMy creator will turn itself into the do-nothing null process.\n");
    	kprintf("\nI will create a second XINU app that runs shell() in shell/shell.c as an example.\n");
    	kprintf("\nYou can do something else, or do nothing; it's completely up to you.\n");
    	kprintf("\n...creating a shell\n");



	/* ******** Lab 5 Section 3 Case (i) Tests ******** */

	pid32 receiverscall = create(receiver_scall, 1024, 20, "syscall receive", 0, NULL);
	resume(receiverscall);

	sleepms(30);

        pid32 sender1 = create(send, 8192, 20, "mysender1", 2, receiverscall, "hello1"); // Sender process second iteration
        resume(sender1); // Run sender process first iteration

	sleepms(10); // Sending multiple messages, second message should be discarded

        pid32 sender2 = create(send, 8192, 20, "mysender2", 2, receiverscall, "hello2"); // Sender process second iteration
        resume(sender2); // Run sender process first iteration


	/* ******** Lab 5 Section 3 Case (ii) Tests ******** */
/*
	pid32 receivereint1 = create(receiver_eint1, 1024, 20, "print A", 0, NULL);
	resume(receivereint1);
	kprintf("receivereint1 pid in main: %d\n", receivereint1);

	pid32 receivereint2 = create(receiver_eint2, 1024, 20, "print B", 0, NULL);
	resume(receivereint2);
	kprintf("receivereint2 pid in main: %d\n", receivereint2);


        pid32 sender2 = create(mysend, 8192, 20, "mysender2", 2, receivereint2, "hello2"); // Sender process second iteration
        resume(sender2); // Run sender process first iteration
*/

	/* ******** Lab 5 Section 4 Test ******** */
        pid32 mempid = create(testgmem, 8192, 50, "testgmem", 0, NULL);
        resume(mempid);
//        kill(mempid);

	/* ******** End Test Cases ******** */



	/* Run the Xinu shell */

	recvclr();
	resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */


/*	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
*/
	return OK;
    

}
