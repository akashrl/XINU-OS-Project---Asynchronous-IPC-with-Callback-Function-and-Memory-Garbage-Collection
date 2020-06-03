# XINU-OS-Project---Asynchronous-IPC-with-Callback-Function-and-Memory-Garbage-Collection

You can view this Lab Project here: https://www.cs.purdue.edu/homes/cs354/lab5/lab5.html

The modified source code is under `xinu-spring2020/system/` and `xinu-spring2020/include/`.

Full project handout:

## 1. Objectives

The objectives of this lab are two-fold: first, implement asynchronous IPC with callback function that preserves isolation/protection and, second, implement memory garbage collection so that when a process terminates any memory not freed by the process is returned to the kernel.

## 3. Asynchronous IPC with callback function

### 3.1 Overall implementation structure

This problem concerns implementation of asynchronous IPC with callback function in XINU as discussed in class. A process registers a user callback function with the kernel requesting that it be executed on behalf of the process when a 1-word message arrives. The callback function contains code written by the app programmer. Although XINU does not implement isolation/protection, we will adopt a design compatible for implementation in modern operating systems such as Linux and Windows that must ensure isolation/protection is preserved when a kernel provides this service. For example, in the traditional system call trap in Linux and Windows using the software interrupt int (what we used in lab2), the instruction iret which returns from a system call switches a process from kernel mode back to user mode. In XINU where system calls are regular C function calls, the instruction ret is used to return to the caller from kernel mode to kernel mode since XINU processes perpetually run in kernel mode. Modify the ROP-based run-time context manipulation technique from Problem 4, lab4, to affect execution of the callback function in the process's context (back in user mode in Linux and Windows in case of iret) that registered the callback function before ultimately returning to synchronous user code (i.e., user code not asynchronously through callback functions) so that the app process can continue where it left off. The callback function takes on the role of quietmalware() in Problem 4.3, lab4.

### 3.2 Callback function registration and unregistration

The kernel exports a system call that is used by an application to specify a user space callback function to be executed in its context when a message is received. The callback function prototype is

`syscall cbregister(void (* fp)(void), umsg32 *mbufptr)`

where fp is a function pointer to a user callback function and mbufptr is a pointer to a 1-word message buffer. The return value is SYSERR if registration failed, OK otherwise. In our XINU implementation, registration fails if the system call determines that fp does not lie in the address boundary of the text segment and/or mbufptr points inside the text segment. Check the nulluser() function in initialize.c which outputs segment boundaries to determine how to do that. A process registers a callback function (typically at the beginning of app code) using

```
   if (cbregister(&usercb, bufpt) != OK) {
      kprintf("registration of %x failed\n", (uint32) usercb);
      userret();        /* proceeding with app code is nonsensical */
   }
```

where `userret()` plays the role of `exit()` in Linux.

XINU remembers that a callback function has been registered by introducing five new fields in the process table structure procent:

```
   bool8 prcbvalid;        /* Nonzero if callback function has been registered */
   void (* prcbptr)();     /* Pointer to callback function */
   umsg32 *prmbufptr;      /* Pointer to user space message buffer */
   umsg32 prtmpbuf;        /* Temporary message buffer */
   bool8 prtmpvalid;       /* Nonzero if temporary message buffer is non-empty */
```

prcbvalid is a Boolean flag, prcbptr stores the callback function address, prmbufptr stores the user message buffer address, prtmpbuf is a temporary message buffer, and prtmpvalid is a flag that indicates if the temporary message buffer is non-empty. The role of prtmpbuf and prtmpvalid is explained in 3.5. When a process tries to register a second callback function, cbregister() returns an error indicating that registration has failed. That is, a process is allowed to register only one callback function for asynchronous IPC. To register a different callback function, an app process must first unregister the existing callback function by invoking the system call

`syscall cbunregister(void)`

which sets the flag prcbvalid to be invalid and returns OK. If there was no registered callback function, cbunregister() returns SYSERR.

### 3.3 Callback function usage

When a message arrives for a process that registered a callback function for asynchronous IPC, the kernel copies the message to the process's user space message buffer. Then XINU arranges for the callback function to be executed before the user process resumes normal synchronous operation. What the callback function does is up to the app programmer. An example callback function is

```
   void usercb(void) {
   extern umsg32 ubuffer;

      kprintf("message is %d\n", ubuffer);
   }
```

where ubuffer is the 1-word message buffer whose address has been communicated to the kernel when invoking cbregister(). The kernel ensures that the callback function is executed in the process's context (in user mode in Linux and Windows) that registered it to preserve isolation/protection. Note that in this version of asynchronous IPC, for a process that has registered a callback function the receive() system call must not be called since no message will be stored in the receiver's 1-word message buffer prmsg in the process table. Instead, the send() system call will copy a message directly to a message buffer in user memory and arrange for the callback function to be executed when the receiver process resumes synchronous execution.

### 3.4 Callback function execution in user context

**Overview** When a 1-word message is sent to the receiver process that registered a callback function -- in our uni-processor galileo backends this implies that the sender process is the current process running on the CPU -- XINU will not run the callback function in kernel context borrowing the context of the sender process. Nor will XINU run the callback function in the context of the sender process since processes must be shielded from each other to achieve isolation/protection. Instead, XINU waits until its scheduler decides to context-switch in the process that registered the callback function, call it the receiver process. Before resuming the receiver process's execution where it left off, using ROP-based run-time process context manipulation XINU will arrange a jump to the registered callback function so that the callback function is executed in the context of the process that registered it. In tandem, the 1-word message is written to the receiver's message buffer whose address communicated to the kernel through the cbregister() system call. When the callback function completes, the kernel arranges for a final jump so that the receiver process resumes where it left off before it was context-switched out.

**Design** The basic technique for injecting the callback function in the receiver process's execution path at run-time is the same as in Problem 4, lab4, albeit with a twist. In Linux and Windows the return address to be manipulated would be the one used by iret since it is at that point that the receiver process switches from kernel mode back to user mode. Two scenarios are possible. A system call was invoked that trapped from user mode to kernel mode, and iret returns from kernel mode back to user mode. In the second scenario, an external interrupt (e.g., clock) caused the current process that registered a callback function to switch to kernel mode. While handling the interrupt in kernel mode the process is context-switched out. For example, in the case of a clock interrupt this would happen if the current process depleted its time slice and the scheduler context-switched in a higher priority ready process. In XINU, we also need to consider two cases:

Case (i) *The receiver process was context-switched out because it made a blocking system call.* To focus on programming technique and reduce unnecessary complications, we will only consider applications that make sleepms() blocking system calls. Other blocking system calls such as wait() in the kernel's upper half can be handled in a similar manner. Unlike Problem 4, lab4, where first we manipulated the return address of ctxsw() to jump to malware code, in asynchronous IPC we will manipulate the return address of sleepms(). Although in XINU sleepms() is a regular function that does not trap from user mode to kernel mode with hardware support (e.g., int instruction in lab2), manipulating the return address of the ret instruction of sleepms() corresponds to manipulation of the return address of iret had sleepms() been implemented to perform a trap. That is, after executing ret from sleepms(), we are back in user context where the process's callback function is executed which is consistent with a kernel design that preserves isolation/protection. The second jump we need to arrange is a jump from the callback function after it completes back to the instruction in the receiver code to which sleepms() would have originally returned to. Hence as in lab4, we need to arrange for two jumps. The difference is that our target is not the return from ctxsw() but sleepms(). Since these operations concern kernel code in the upper half, case (i) concerns changes in run-time behavior of the upper half that pertain to system calls.

Case (ii) *The receiver process was forcefully context-switched out because it depleted its time slice.* This means that a clock interrupt prompted execution of clkdisp.S which, in turn, called clkhandler(). The latter manages time slice countdown by calling resched() when preempt reaches 0. The moment the computing system returns to user context of the receiver process is when iret of clkdisp is executed after the receiver process is context-switched in, i.e., becomes current and resched() returns to clkhandler() which, in turn, returns to clkdisp. The mechanisms underlying iret we have already encountered when implementing trapped versions of system calls getpid() and getprio() in lab2. The return address of iret in clkdisp must be manipulated so that, as in case (i), first we jump to the receiver's callback function, and second, upon completion of the callback function the receiver process jumps back to the original return address of iret in clkdisp. Since these operations concern kernel code in the lower half, case (ii) concerns changes in run-time behavior of the lower half that pertain to external interrupts.

### 3.5 Implementation of return address manipulation

**Mods in send()** Preparatory actions for manipulation of the return address -- in case (i) ret from sleepms() and in case (ii) iret from clkdisp -- must be done by send() when a message send event to a receiver that registered a callback function occurs. XINU can determine if case (i) or case (ii) applies by checking the receiver process's state: PR_SLEEP implies case (i) and PR_READY implies case (ii). Manipulation of return addresses can be done in several ways. In this lab, follow an approach that is direct and does not use wrapper functions. In both case (i) and case (ii), modify send() so that if the receiver of a message has registered a callback function and has received a prior message for which its callback function has not been executed yet, the newly sent message is discarded. That is, only one asynchronous IPC message can be outstanding at a time. The process table field, prtmpvalid, is initialized to 0, and set to 1 if a message to an asynchronous receiver arrives. The message is temporarily stored in prtmpbuf. The last two operations are the responsibility of send(). When the receiver process becomes current, the kernel copies the content of prtmpbuf to the receiver's 1-word user space message buffer and resets prtmpvalid to 0. The specifics of these steps are described for case (i) and case (ii) separately.

**Mods in clkdisp** First, case (ii). Modify clkdisp so that before enabling interrupts, restoring the 8 general-purpose registers, and executing iret, clkdisp copies the content of prtmpbuf to the user space message buffer and resets prtmpvalid to 0. Then comes the key step of manipulating return addresses so that the receiver's callback function is executed in user context. That is, after executing iret. From lab2, recall that EFLAGS, CS, EIP were pushed by x86 onto the receiver's run-time stack. XINU's clkdisp code is written so that after popal ESP points to the stack location where EIP has been pushed. What we will do is shift the content of the top three locations (in unit of word) containing EFLAGS, CS, EIP "downward" by 1 word (i.e., 4 bytes). Downward means toward lower memory which implies increasing the stack by 1 word. This frees up the 1-word space in the stack where EFLAGS before the shift was located. Copy into this location the pushed EIP at the top of the stack which is the original return address of clkdisp. Note that ESP must be updated to reflect the growth of the stack by one word. Lastly, replace the content at the top of the stack which contains the original return address of iret with the address of the callback function. Then enable interrupts and restore the x86 general-purpose registers. iret will pop the top of the stack containing the callback function's address into register EIP, pop the next two words from the stack and populate CS and EFLAGS, at which point the stack pointer ESP will point to the original return address of clkdisp. iret will have caused a jump to the callback function, and when it returns upon completion, ESP will point to the original return address of clkdisp so that upon executing ret in the callback function, the receiver process resumes where it left off. The above is an outline and your task is to implement it and make it work.

**Mods in sleepms()** Case (i) differs from case (ii) implementation in that sleepms() is coded in C and sleepms() uses ret, not iret, to return upon completion. Not using iret simplifies coding since making a detour to the callback function and jumping to the original return address upon completion may be done by C code inserted at the end of sleepms(). Note that for system calls in Linux and Windows that trap from user mode to kernel mode, an approach similar to case (ii) must be followed for case (i).

Test your design and implementation using sender and receiver processes. This includes sending of multiple messages by a single sender and multiple sender processes, and checking the messages received by the receiver process. You do not need to consider the case where a process registers a callback function and, then, calls receive(). Since only one asynchronous IPC message for a receiver can be outstanding, process priority and scheduling will influence if messages are dropped or not. Also, no blocking system calls are allowed from within callback functions. Use the code from lab1 as your base code where using fixed process priority and time slice that you can configure, correctness of your implementation can be more readily evaluated.

## Memory garbage collection (120 pts)

### 4.1 Overview

XINU uses getmem() to allocate heap memory from a single linked list of free memory segments and freemem() to return unused memory to the free memory pool. The kernel keeps track of per-process stack memory so that when a process terminates its stack memory is returned to the free memory list via freestk(). This is not the case, however, for memory allocated by getmem() which gets only freed if a process explicitly deallocates memory by calling freemem() which is voluntary. That is, when a process terminates any dynamic memory that was allocated but has not been freed remains allocated. Examine kill() which is called to terminate a XINU process. Even when an application programmer ardently tries to free allocated memory before exiting, programming mistakes and bugs that prematurely terminate a process may result in build-up of memory that is not returned to the free list upon termination. This is one form of memory garbage.

Garbage collection aims to free allocated memory -- in general, while a process is still running -- when it can be ascertained that a process does not need the memory anymore, i.e., all references/pointers to the memory have been deleted. We are tackling a more modest problem of ensuring that all memory allocated to a process through getmem(), whether freed by a process or not, are reclaimed by XINU when the process terminates. This eliminates memory leakage by injecting garbage collection support inside the kernel. To do so, XINU must track dynamic memory allocation and deallocation on a per-process basis and return any unfreed memory to the free list when a process terminates through kill().

## 4.2 Per-process memory block list

Design and implement garbage collection support in XINU by modifying the system calls getmem(), freemem(), kill(), and other relevant parts of the kernel to eliminate memory leakage. For simplicity, assume that freemem() is always called by app code so that its first argument is a pointer returned by getmem() and the second argument is the memory size requested by getmem(). That is, freemem() behaves similar to free() in Linux where it takes a memory pointer as its sole argument. Add a new field to the process table

`struct inusememblk *prheaphd;      /* Header node of allocated memory list */`

where the struct inusememblk

```
   struct inusememblk  {                     
	char    *memblockptr;           /* Ptr to in-use memory block */
	uint32  memlen;			/* Size of memory allocated (multiple of 8 bytes) */
        struct  inusememblk  *mnext;    /* Ptr to next list element  */
   };
```

is used to keep track of in-use (i.e., allocated) memory blocks. Define inusememblk in include/memory.h. prheaphd points to a list that keeps track of all memory blocks allocated to a process using getmem() but not freed using freemem(). prheaphd->memblockptr contains the address of the first in-used memory block (null if no heap memory is in-used by the process), and prheaphd->mnext points to the next element in the memory block list. memlen specifies the size of the allocated memory block rounded up to be a multiple of 8 bytes. This per-process memory block list is a priority list whose elements are sorted in increasing order of memblockptr. Code an internal kernel function with function prototype

`void meminsert(struct inusememblk *hdptr, char *memptr);`

that deletes a list element whose memblockptr equals memptr.

Memory for a process's memory block list is dynamically allocated using getmem() in meminsert(). However, since a process's memory block list is part of kernel memory, it is not included in the process's memory block list. Since memextract() is kernel code and is assumed to be correctly coded -- i.e., calls freemem() when a process's memory block list element is deleted -- memory leakage from kernel code is not a concern. To exclude memory allocated for a process's memory block list, getmem() needs to know if it was called from meminsert(). In a similar vein, freemem() needs to know if it was called from memextract(). To do so, define a global variable, bool8 memblockkflag, that is initialized to 0. memblockkflag is set to 1 when getmem() is called from meminsert(). Similarly for freemem() from memextract(). If so, getmem() and freemem() reset the flag to 0 before returning.

### 4.3 Testing

Evaluate your garbage collection enabled XINU kernel on test cases where app programs omit freemem() system calls but garbage collection ensures that the unfreed memory is reclaimed when processes terminate. Assume that freemem() will always be correctly invoked, i.e., with the first argument being a pointer returned by getmem().

### Analysis

In Problem 4, lab4, an attacker modified the return address of ctxsw() to jump to its malware code which then behaved overtly or covertly depending on the attacker's objective. In the bonus problem, consider mounting a defense in the context of XINU running on galileo backends subject to the attacker of lab4. The solution cannot use a canary to detect corruption since return address overwrite was performed surgically without collateral corruption of surrounding memory. Instead, think about a ROP based defense where kernel code is modified to check if the return address of ctxsw() (and similarly for resched() and sleepms()) is valid, i.e., safe to jump to, and does so only if it is. Perhaps a modified return address may even be corrected to its original value so that jumping is feasible despite an attack. Describe a detailed solution in Lab5Answers.pdf in lab5/. There is no need to implement your solution.
