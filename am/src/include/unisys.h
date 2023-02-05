#ifndef NEMU_H__
#define NEMU_H__

#include <riscv.h>
#include <klib-macros.h>

# define nemu_trap(code) asm volatile("mv a0, %0; ebreak" : :"r"(code))

# define DEVICE_BASE 0xa0000000

#define MMIO_BASE 0xa0000000

#define SERIAL_PORT     (DEVICE_BASE + 0x00003f8)
#define KBD_ADDR        (DEVICE_BASE + 0x0000060)
#define RTC_ADDR        (DEVICE_BASE + 0x0000048)
#define VGACTL_ADDR     (DEVICE_BASE + 0x0000100)
#define AUDIO_ADDR      (DEVICE_BASE + 0x0000200)
#define DISK_ADDR       (DEVICE_BASE + 0x0000300)
#define PRINT_ADDR      (DEVICE_BASE + 0x00009f0)
#define FB_ADDR         (MMIO_BASE   + 0x1000000)
#define AUDIO_SBUF_ADDR (MMIO_BASE   + 0x1200000)
#define SHARE_ADDR      (MMIO_BASE   + 0x3000000)



extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)
#define NEMU_PADDR_SPACE \
  RANGE(&_pmem_start, PMEM_END), \
  RANGE(FB_ADDR, FB_ADDR + 0x200000), \
  RANGE(MMIO_BASE, MMIO_BASE + 0x1000) /* serial, rtc, screen, keyboard */

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
