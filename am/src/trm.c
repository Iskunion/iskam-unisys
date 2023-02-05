#include <am.h>
#include <unisys.h>
#include <klib-macros.h>
#include <stdio.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  io_write(AM_UART_TX, ch);
}

void halt(int code) {
  // nemu_trap(code);
  printf("Exist with %d", code);
  // we end the program by a dead loop
  while (1);
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
