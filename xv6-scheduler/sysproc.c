#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include <stddef.h>

// prototype for settickets which returns 0 on successful ticket setting
int 
settickets(void) 
{
  int number;
  if (argint(0, &number) < 0 || argint(0, &number) > 10000) {
    return -1;
  }
  else {
    myproc()->tickets = argint(0, &number);
    return 0;
  }
}

int 
getprocessesinfo(void) 
{
  struct processes_info *p;
  if (argptr(0, (void*)&p, sizeof(*p)) || p == NULL) return -1;
  // Check if the size matches
  if (sizeof(struct processes_info) != sizeof(*p)) return -1;

  ptable_t *ptable = get_ptable();

  struct proc *proci; // process iterator
  int i = 0; // counting loop iterations for vectors, should not exceed num_processes?
  int n = 0; // Initialize the count of processes

  acquire(&ptable->lock); // get lock

  // iterate through process table (from code for kill())
  for(proci = ptable->proc, i = 0; proci < &ptable->proc[NPROC]; proci++) {
    // use proci->pid to get pid of process proci
    if (proci->state != UNUSED) {
        // Populate p->pids, p->times_scheduled, p->tickets here
        p->pids[i] = proci->pid;
        p->times_scheduled[i] = proci->timesscheduled; // TODO: set times_scheduled[i] to the number of times proci has been scheduled since creation
        p->tickets[i] = proci->tickets; // sets tickets[i] to # of tickets assigned to each process
        i++; // Increment i for the next valid process
        n++; // Increment the count of processes
    }
  }

  p->num_processes = i;

  // release lock before returning
  release(&ptable->lock);

  return 0;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_yield(void)
{
  yield();
  return 0;
}

int sys_shutdown(void)
{
  shutdown();
  return 0;
}
