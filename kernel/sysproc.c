#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "time.h"
#include "console.h"
#include "spinlock.h"

extern struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

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
    int pid, sig;

    if (argint(0, &pid) < 0 || argint(1, &sig) < 0)
        return -1;
    return kill(pid, sig);
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

int sys_sigaction(void)
{
    int sig;
    void (*handler)(void);

    if (argint(0, &sig) < 0 || argptr(1, (char**)&handler, sizeof(handler)) < 0)
        return -1;

    if (sig < 0 || sig >= 32)
        return -1;

    myproc()->handlers[sig] = handler;
    return 0;
}

int sys_sigprocmask(void)
{
    int mask;

    if (argint(0, &mask) < 0)
        return -1;

    myproc()->blocked = mask;
    return 0;
}

int sys_sigreturn(void)
{
    struct proc* p = myproc();

    if (p->sig_tf) {
        *p->tf = *p->sig_tf;
        p->sig_tf = 0;
    }
    return 0;
}

int sys_setfg(void)
{
    int pid;
    struct proc* p;

    if (argint(0, &pid) < 0)
        return -1;

    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->pid == pid) {
            input.fgproc = p;
            release(&ptable.lock);
            return 0;
        }
    }
    release(&ptable.lock);
    return -1;
}
