/*
 * UART driver for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */
#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

/*
 * Most of this file is actually a cleanup of the old xv6 UART driver, with
 * named constants and more understandable code. It is intentionally compatible
 * with the original xv6 code. It also utilizes spinlocks and sets the baud rate,
 * which is helpful.
 */

#define COM1        0x3F8

/* UART register offsets */
#define UART_DATA       0
#define UART_INT_EN     1
#define UART_FIFO_CTL   2
#define UART_LINE_CTL   3
#define UART_MCR        4
#define UART_LSR        5
#define UART_MSR        6
#define UART_SCR        7

#define UART_LSR_DR     0x01
#define UART_LSR_THRE   0x20

static int uart_present; /* Is a UART even present? */

static struct spinlock uart_lock;

/*
 * Helper functions
 */
static inline int uart_tx_ready(void) {
  return inb(COM1 + UART_LSR) & UART_LSR_THRE;
}

static inline int uart_rx_ready(void) {
  return inb(COM1 + UART_LSR) & UART_LSR_DR;
}

/* Set UART baud rate */
void uartsetbaud(uint baud)
{
  if (!uart_present)
    return;

  ushort divisor = 115200 / baud;
  outb(COM1 + UART_LINE_CTL, 0x80);
  outb(COM1 + UART_DATA, divisor & 0xFF);
  outb(COM1 + UART_INT_EN, (divisor >> 8) & 0xFF);
  outb(COM1 + UART_LINE_CTL, 0x03);
}

void uartinit(void)
{
  initlock(&uart_lock, "uart");
  outb(COM1 + UART_INT_EN, 0x00); /* Disable interrupts */

  /* Enable DLAB to set baud rate divisor */
  outb(COM1 + UART_LINE_CTL, 0x80);
  outb(COM1 + UART_DATA, 115200 / 9600);
  outb(COM1 + UART_INT_EN, 0x00);

  // 8 data bits, no parity, one stop bit
  /* 8 data bits, no parity, one stop bit */
  outb(COM1 + UART_LINE_CTL, 0x03);

  /* Enable FIFO */
  outb(COM1 + UART_FIFO_CTL, 0xC7);

  /* Enable IRQs */
  outb(COM1 + UART_MCR, 0x0B);

  /* Check if UART exists */
  if (inb(COM1 + UART_LSR) == 0xFF)
    return;
  else 
      uart_present = 1;

  /* Enable interrupts for received data */
  outb(COM1 + UART_INT_EN, 0x01);

  ioapicenable(IRQ_COM1, 0);

  inb(COM1 + UART_DATA);
  inb(COM1 + UART_LSR);

  /* This is the fastest we can probably get on QEMU */
  uartsetbaud(115200);
}

void uartputc(int c)
{
  /* 
   * TODO: maybe handle this better? We could make the functions which call
   * uartputc() to not waste time calling this function, and instead expose the
   * uart_present flag to functions like cprintf(). This would save the user
   * a bit (in fact, a few nanoseconds) of time.
   */
  if (!uart_present)
    return;

  acquire(&uart_lock);
  for (int i = 0; i < 128 && !uart_tx_ready(); i++)
    microdelay(10);

  outb(COM1 + UART_DATA, c);
  release(&uart_lock);
}

static int uartgetc(void)
{
  if (!uart_present || !uart_rx_ready())
    return -1;
  return inb(COM1 + UART_DATA);
}

void uartintr(void)
{
  consoleintr(uartgetc);
}
