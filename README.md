# xv6 Kernel Enhancements

This repository contains custom implementations and feature enhancements for the **xv6 RISC-V** operating system, developed as part of operating system lab coursework. The project focuses on extending core kernel capabilities across memory allocation, page table manipulation, and user-space interrupt mechanisms.

---

## Repository Overview

* **COW (Copy-on-Write Fork)**  
  Implements a Copy-on-Write mechanism to optimize the `fork()` system call. Instead of duplicating physical memory pages immediately upon process creation, parent and child processes share the same physical pages marked read-only. When either process attempts a write, a page fault (`scause` 13/15) is triggered, allocating a new physical page on demand.
  * **Key Components:** Physical page reference counting (`kalloc.c`), page table entry flag management (`PTE_COW` in `riscv.h`), and kernel page-fault handling (`trap.c`).

* **Memory Management**  
  Focuses on virtual memory control, address translation, and page table inspection. Provides utility functions to traverse, map, and display hierarchical page table structures across kernel and user space.
  * **Key Components:** Page table walking and visualization routines (`vmprint` in `vm.c`), kernel-to-user memory space configurations, and process page table management (`proc.c`).

* **User Space Traps**  
  Introduces user-level trap and timer capabilities by allowing application processes to handle CPU interrupts. Enables periodic execution of user-space functions without compromising kernel space safety or context integrity.
  * **Key Components:** Custom system calls `sigalarm` and `sigreturn` (`sysproc.c`), per-process tick tracking and trap frame state restoration (`trap.c` & `proc.h`), and user-space stubs (`usys.pl`).
