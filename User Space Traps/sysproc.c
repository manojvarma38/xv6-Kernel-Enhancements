#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  myproc()->trace_mask = mask; /*storing the mask in new field (trace_mask) inside struct proc*/
  return 0;
}
// lab4
//  sigalarm(int ticks, void (*handler)())
uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler; // user-space function pointer

  argint(0, &ticks);

  argaddr(1, &handler); // argaddr reads uintptr-like arg

  struct proc *p = myproc();

  acquire(&p->lock);
  
  // set fields under proc
  p->alarm_interval = ticks;
  p->alarm_handler = handler;
  p->alarm_ticks_left = ticks;

  release(&p->lock);

  return 0;
}

// sigreturn(void)
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  uint64 ret;

  acquire(&p->lock);
  // We must return the saved a0 so syscall() doesn't clobber the user a0 we are restoring below.
  ret = p->alarm_tf.a0;

  // restore user registers exactly as before the handler
  memmove(p->trapframe, &p->alarm_tf, sizeof(struct trapframe));

  p->alarm_on = 0;

  if (p->alarm_interval > 0)
    p->alarm_ticks_left = p->alarm_interval;

  release(&p->lock);
  return ret;
}
