# XINU-OS-Project---Asynchronous-IPC-with-Callback-Function-and-Memory-Garbage-Collection

You can view this Lab Project handout here: https://www.cs.purdue.edu/homes/cs354/lab5/lab5.html

The objectives of this lab are two-fold: first, implement asynchronous IPC with callback function that preserves isolation/protection and, second, implement memory garbage collection so that when a process terminates any memory not freed by the process is returned to the kernel. This problem concerns implementation of asynchronous IPC with callback function in XINU as discussed in class. A process registers a user callback function with the kernel requesting that it be executed on behalf of the process when a 1-word message arrives. The callback function contains code written by the app programmer. Although XINU does not implement isolation/protection, we will adopt a design compatible for implementation in modern operating systems such as Linux and Windows that must ensure isolation/protection is preserved when a kernel provides this service. For example, in the traditional system call trap in Linux and Windows using the software interrupt int (what we used in lab2), the instruction iret which returns from a system call switches a process from kernel mode back to user mode. In XINU where system calls are regular C function calls, the instruction ret is used to return to the caller from kernel mode to kernel mode since XINU processes perpetually run in kernel mode. Modify the ROP-based run-time context manipulation technique from Problem 4, lab4, to affect execution of the callback function in the process's context (back in user mode in Linux and Windows in case of iret) that registered the callback function before ultimately returning to synchronous user code (i.e., user code not asynchronously through callback functions) so that the app process can continue where it left off. The callback function takes on the role of quietmalware() in Problem 4.3, lab4.
The next objective of this lab is Memory garbage collection. XINU uses getmem() to allocate heap memory from a single linked list of free memory segments and freemem() to return unused memory to the free memory pool. The kernel keeps track of per-process stack memory so that when a process terminates its stack memory is returned to the free memory list via freestk(). This is not the case, however, for memory allocated by getmem() which gets only freed if a process explicitly deallocates memory by calling freemem() which is voluntary. That is, when a process terminates any dynamic memory that was allocated but has not been freed remains allocated. Examine kill() which is called to terminate a XINU process. Even when an application programmer ardently tries to free allocated memory before exiting, programming mistakes and bugs that prematurely terminate a process may result in build-up of memory that is not returned to the free list upon termination. This is one form of memory garbage.

Garbage collection aims to free allocated memory -- in general, while a process is still running -- when it can be ascertained that a process does not need the memory anymore, i.e., all references/pointers to the memory have been deleted. We are tackling a more modest problem of ensuring that all memory allocated to a process through getmem(), whether freed by a process or not, are reclaimed by XINU when the process terminates. This eliminates memory leakage by injecting garbage collection support inside the kernel. To do so, XINU must track dynamic memory allocation and deallocation on a per-process basis and return any unfreed memory to the free list when a process terminates through kill().
