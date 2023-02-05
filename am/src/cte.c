#include <am.h>
#include <riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_get_cur_as(Context *);
void __am_switch(Context *);

#define IRQ_TIMER 0x80000007  // for riscv32

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case -1: ev.event = EVENT_YIELD; break;
      case IRQ_TIMER: ev.event = EVENT_IRQ_TIMER; break;
      default:
        if (c->mcause < NR_SYSCALL) ev.event = EVENT_SYSCALL;
        else ev.event = EVENT_ERROR;
      break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = kstack.end - sizeof(Context);
  c->mstatus = 0x1880;
  c->GPR2 = (uintptr_t) arg;
  c->mepc = (uintptr_t) entry;
  c->np = KERNEL_STATE;
  c->pdir = NULL;
  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
