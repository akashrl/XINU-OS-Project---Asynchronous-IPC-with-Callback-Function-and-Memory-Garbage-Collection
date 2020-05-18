/* clkdisp.s - clkdisp (x86) */
# extern currpid // Added for Lab 5, Part 3
# extern proctab

/*------------------------------------------------------------------------
 * clkdisp  -  Interrupt dispatcher for clock interrupts (x86 version)
 *------------------------------------------------------------------------
 */
#include <icu.s>
		.text
		.globl	clkdisp		# Clock interrupt dispatcher
clkdisp:
		pushal			# Save registers
		cli			# Disable further interrupts
		movb	$EOI,%al	# Reset interrupt
		outb	%al,$OCW1_2

		call	clkhandler	# Call high level handler
//		call	userbuf		# Lab 5, Part 3
//		movl	$currpid, %ebx  # Index for the table
//		movl	$96, %eax	# Accessing the correct element in the table, size of proctab is 96 bytes
//		imul	%ebx, %eax	# imul is accessing the correct element in the index in the table
//		lea	$proctab(%eax), %ebx	# ebx points to our table entry for this process 

//		sti			# Restore interrupt status
//		popal			# Restore registers

//		push	%eax		# Saving eax, and it is creating the extra space on the stack
//		movl	4(%esp), %eax	# Move the contents of (12 + esp) into eax. Getting the word
//		movl	%eax, 8(%esp)	# Moves stack plus 3 words to stack plus 4 words. Putting it back to the stack
//		movl	8(%esp), %eax	# Getting the second word
//		movl	%eax, 12(%esp)	# Stores the word in the third position
//		movl	12(%esp), %eax	# Getting from the first position (EIP instruction)
//		movl	%eax, 16(%esp)	# Storing it to the second position
		# Here,  place the callback address at the top of the stack
//		movl	(%esp), %eax	# Restores eax without a pop
		# Restore EIP

		sti			# Restore interrupt status
		popal			# Restore registers
		iret			# Return from interrupt
