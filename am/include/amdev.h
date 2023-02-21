#ifndef __AMDEV_H__
#define __AMDEV_H__

//Based on ProjectN AM, added some extra interface bring by unisys
//We try to stay compatible with original AM
#include <stdbool.h>
#include <stdint.h>
#include <am.h>

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

//unisys GPU
AM_DEVREG(29, GPU_DRAW256,  WR, uint32_t x, y; void *pixels; uint32_t w, h; bool sync);
AM_DEVREG(30, GPU_TERMINAL, WR, uint32_t i, j; char content; uint8_t fg_color, bg_color; bool sync, clear);
AM_DEVREG(31, GPU_CHCONFIG, RD, bool present; int width, height);
AM_DEVREG(32, GPU_CHSCROLL, WR, bool sync);

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
  AM_KEY_0=0x45,
  AM_KEY_1=0x16,
  AM_KEY_2=0x1e,
  AM_KEY_3=0x26,
  AM_KEY_4=0x25,
  AM_KEY_5=0x2e,
  AM_KEY_6=0x36,
  AM_KEY_7=0x3d,
  AM_KEY_8=0x3e,
  AM_KEY_9=0x46,
  AM_KEY_A=0x1c,
  AM_KEY_APOSTROPHE=0x0e,
  AM_KEY_APPLICATION=0xa5,
  AM_KEY_B=0x32,
  AM_KEY_BACKSLASH=0x5d,
  AM_KEY_BACKSPACE=0x66,
  AM_KEY_C=0x21,
  AM_KEY_CAPSLOCK=0x58,
  AM_KEY_COMMA=0x41,
  AM_KEY_D=0x23,
  AM_KEY_DELETE=0xa6,
  AM_KEY_DOWN=0xa7,
  AM_KEY_E=0x24,
  AM_KEY_END=0xa8,
  AM_KEY_EQUALS=0x55,
  AM_KEY_ESCAPE=0x76,
  AM_KEY_F10=0x09,
  AM_KEY_F11=0x78,
  AM_KEY_F12=0x07,
  AM_KEY_F1=0x05,
  AM_KEY_F2=0x06,
  AM_KEY_F3=0x04,
  AM_KEY_F4=0x0c,
  AM_KEY_F5=0x03,
  AM_KEY_F6=0x0b,
  AM_KEY_F7=0x83,
  AM_KEY_F8=0x0a,
  AM_KEY_F9=0x01,
  AM_KEY_F=0x2b,
  AM_KEY_G=0x34,
  AM_KEY_GRAVE=0x52,
  AM_KEY_H=0x33,
  AM_KEY_HOME=0xaa,
  AM_KEY_I=0x43,
  AM_KEY_INSERT=0xab,
  AM_KEY_J=0x3b,
  AM_KEY_K=0x42,
  AM_KEY_L=0x4b,
  AM_KEY_LALT=0x11,
  AM_KEY_LCTRL=0x14,
  AM_KEY_LEFT=0xac,
  AM_KEY_LEFTBRACKET=0x54,
  AM_KEY_LSHIFT=0x12,
  AM_KEY_M=0x3a,
  AM_KEY_MINUS=0x4e,
  AM_KEY_N=0x31,
  AM_KEY_O=0x44,
  AM_KEY_P=0x4d,
  AM_KEY_PAGEDOWN=0xa1,
  AM_KEY_PAGEUP=0xa2,
  AM_KEY_PERIOD=0x49,
  AM_KEY_Q=0x15,
  AM_KEY_R=0x2d,
  AM_KEY_RALT=0x11,
  AM_KEY_RCTRL=0x14,
  AM_KEY_RETURN=0x5a,
  AM_KEY_RIGHT=0xa3,
  AM_KEY_RIGHTBRACKET=0x5b,
  AM_KEY_RSHIFT=0x59,
  AM_KEY_S=0x1b,
  AM_KEY_SEMICOLON=0x4c,
  AM_KEY_SLASH=0x4a,
  AM_KEY_SPACE=0x29,
  AM_KEY_T=0x2c,
  AM_KEY_TAB=0x0d,
  AM_KEY_U=0x3c,
  AM_KEY_UP=0xa4,
  AM_KEY_V=0x2a,
  AM_KEY_W=0x1d,
  AM_KEY_X=0x22,
  AM_KEY_Y=0x35,
  AM_KEY_Z=0x1a
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
