#ifndef __AMDEV_H__
#define __AMDEV_H__

//Based on ProjectN AM, added some extra interface bring by unisys
//We try to stay compatible with original AM
#include <stdbool.h>
#include <stdint.h>

#define AM_DEVREG(id, reg, perm, ...) \
  enum { AM_##reg = (id) }; \
  typedef struct { __VA_ARGS__; } AM_##reg##_T;

//standard AM UART
AM_DEVREG( 1, UART_CONFIG,    RD, bool present);
AM_DEVREG( 2, UART_TX,        WR, unsigned char data);
AM_DEVREG( 3, UART_RX,        RD, unsigned char data);
//unisys UART
AM_DEVREG( 4, UART_STATUS,    WR, bool tx_busy : 1; bool rx_ready : 1);
AM_DEVREG( 5, UART_CTRL,      WR, bool tx_enable : 1; bool rx_enable : 1);
AM_DEVREG( 6, UART_BAUD,      WR, short baud_clock_nr);

//standard AM TIMER
AM_DEVREG( 7, TIMER_CONFIG,   RD, bool present, has_rtc);
AM_DEVREG( 8, TIMER_RTC,      WR, int year, month, day, hour, minute, second);
AM_DEVREG( 9, TIMER_UPTIME,   RD, uint32_t us);
//unisys TIMER
AM_DEVREG(10, TIMER_INTR,     WR, uint32_t us);

//standard AM INPUT
AM_DEVREG(11, INPUT_CONFIG, RD, bool present);
AM_DEVREG(12, INPUT_KEYBRD, RD, bool keydown; int keycode);

//standard AM GPU
AM_DEVREG(13, GPU_CONFIG,   RD, bool present, has_accel; int width, height, vmemsz);
AM_DEVREG(14, GPU_STATUS,   RD, bool ready);
AM_DEVREG(15, GPU_FBDRAW,   WR, uint32_t x, y; void *pixels; uint32_t w, h; bool sync);
AM_DEVREG(16, GPU_MEMCPY,   WR, uint32_t dest; void *src; int size);
AM_DEVREG(17, GPU_RENDER,   WR, uint32_t root);
//unisys 256c GPU
AM_DEVREG(25, GPU_DRAW256,  WR, uint32_t x, y; void *pixels; uint32_t w, h; bool sync);

//standard AM AUDIO
AM_DEVREG(18, AUDIO_CONFIG, RD, bool present; int bufsize);
AM_DEVREG(19, AUDIO_CTRL,   WR, int freq, channels, samples);
AM_DEVREG(20, AUDIO_STATUS, RD, int count);
AM_DEVREG(21, AUDIO_PLAY,   WR, Area buf);

//standard AM DISK
AM_DEVREG(22, DISK_CONFIG,  RD, bool present; int blksz, blkcnt);
AM_DEVREG(23, DISK_STATUS,  RD, bool ready);
AM_DEVREG(24, DISK_BLKIO,   WR, bool write; void *buf; int blkno, blkcnt);

//standard AM NET
AM_DEVREG(25, NET_CONFIG,   RD, bool present);
AM_DEVREG(26, NET_STATUS,   RD, int rx_len, tx_len);
AM_DEVREG(27, NET_TX,       WR, Area buf);
AM_DEVREG(28, NET_RX,       WR, Area buf);

// Input

#define AM_KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define AM_KEY_NAMES(key) AM_KEY_##key,
enum {
  AM_KEY_NONE = 0,
  AM_KEYS(AM_KEY_NAMES)
};

// GPU

#define AM_GPU_TEXTURE  1
#define AM_GPU_SUBTREE  2
#define AM_GPU_NULL     0xffffffff

typedef uint32_t gpuptr_t;

struct gpu_texturedesc {
  uint16_t w, h;
  gpuptr_t pixels;
} __attribute__((packed));

struct gpu_canvas {
  uint16_t type, w, h, x1, y1, w1, h1;
  gpuptr_t sibling;
  union {
    gpuptr_t child;
    struct gpu_texturedesc texture;
  };
} __attribute__((packed));

#endif
