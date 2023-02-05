#ifndef NEMU_H__
#define NEMU_H__

#include <riscv.h>
#include <klib-macros.h>

//for compatiblity
#define nemu_trap(code) asm volatile("mv a0, %0; ebreak" : :"r"(code))

#define KBD_BASE      0x00000000
#define UART_BASE     0x20000000
#define TIMER_BASE    0x40000000
#define VGA_BASE      0x60000000
#define FB_BASE       0xa0000000
#define SD_BASE       0xc0000000
#define AUDIO_BASE    0xe0000000

extern char _pmem_start;
#define PMEM_SIZE (256 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)
#define NEMU_PADDR_SPACE \
  RANGE(&_pmem_start, PMEM_END)//, 
  // RANGE(FB_ADDR, FB_ADDR + 0x200000), 
  // RANGE(MMIO_BASE, MMIO_BASE + 0x1000) 
  /* serial, rtc, screen, keyboard */

typedef uintptr_t PTE;

#define exp2(x)         (1u << (x))
#define BITMASK(bits)   (exp2(bits) - 1)
#define PGW             (12)
#define PGSIZE          exp2(PGW)
#define PGMASK          BITMASK(PGW)
#define PGPN(addr)      (((uintptr_t) addr) >> (PGW))
#define PGOFF(addr)     (((uintptr_t) addr) & (PGMASK))
#define PGSTART(pn)     ((void *) ((pn) << (PGW)))

#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))

#define T0SZ            (10)
#define T1SZ            (10)

#define VPN_0(vaddr)  BITS(((uintptr_t) vaddr), (PGW + T0SZ - 1), PGW)
#define VPN_1(vaddr)  BITS(((uintptr_t) vaddr), (PGW + T0SZ + T1SZ - 1), (PGW + T0SZ))

#endif
