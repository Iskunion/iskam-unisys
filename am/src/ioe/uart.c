#include <am.h>
#include <unisys.h>

#include <klib-macros.h>
#include <stdbool.h>

#define CTRL_ADDR   UART_BASE
#define STATUS_ADDR UART_BASE + 0x04
#define BAUD_ADDR   UART_BASE + 0x08
#define TX_ADDR     UART_BASE + 0x0c
#define RX_ADDR     UART_BASE + 0x10

void __am_uart_status_r(AM_UART_STATUS_T* target) {
  uint32_t uart_status = inl(STATUS_ADDR);
  *(uint32_t *) target = uart_status;
}

void __am_uart_status_w(AM_UART_STATUS_T* target) {
  outl(*(uint32_t *) target, STATUS_ADDR);
}

void __am_uart_get_data(AM_UART_RX_T* target) {
  AM_UART_STATUS_T uart_status = io_read(AM_UART_STATUS);
  // am might not care this
  while (!uart_status.rx_ready)
    uart_status = io_read(AM_UART_STATUS);

  target->data = inb(RX_ADDR);
  uart_status.rx_ready = false;
  //set rx_ready
  ioe_write(AM_UART_STATUS, &uart_status);
}

void __am_uart_ctrl_r(AM_UART_CTRL_T *target) {
  uint32_t uart_ctrl = inl(CTRL_ADDR);
  *(uint32_t *) target = uart_ctrl;
}

void __am_uart_ctrl_w(AM_UART_CTRL_T *target) {
  outl(*(uint32_t *) target, CTRL_ADDR);
}

void __am_uart_send_data(AM_UART_TX_T* target){
  AM_UART_STATUS_T uart_status = io_read(AM_UART_STATUS);
  if (uart_status.tx_busy)
    outb(TX_ADDR, target->data);
}

void __am_uart_init(){
  AM_UART_CTRL_T uart_ctrl = {.rx_enable = true, .tx_enable = true};
  ioe_write
}