#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "time.h"

typedef int time_t;
extern int boottime;

int sys_fork(void) { return fork(); }

int sys_exit(void)
{
    exit();
    return 0; // not reached
}

int sys_wait(void) { return wait(); }

int sys_kill(void)
{
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

int sys_getpid(void) { return myproc()->pid; }

int sys_getppid(void) { return myproc()->parent->pid; }

int sys_kgetprogname()
{
    char* dst;
    if (argptr(0, &dst, 16) < 0)
        return -1;
    safestrcpy(dst, myproc()->name, 16);
    return 0;
}

time_t sys_time(void) { return unix_uptime(); }

int sys_ksetprogname(void)
{
    char* src;
    if (argstr(0, &src) < 0)
        return -1;
    safestrcpy(myproc()->name, src, 16);
    return 0;
}

int sys_sbrk(void)
{
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;
    addr = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

int sys_sleep(void)
{
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

time_t sys_uptime(void)
{
    int ucurtime = unix_uptime();
    return ucurtime - boottime;
}
