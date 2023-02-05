#include <am.h>
#include <unisys.h>
#include <klib-macros.h>

// void __am_timer_init();
// void __am_gpu_init();
// void __am_audio_init();
// void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
// void __am_timer_rtc(AM_TIMER_RTC_T *);
// void __am_timer_uptime(AM_TIMER_UPTIME_T *);
// void __am_gpu_config(AM_GPU_CONFIG_T *);
// void __am_gpu_status(AM_GPU_STATUS_T *);
// void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);
// void __am_audio_config(AM_AUDIO_CONFIG_T *);
// void __am_audio_ctrl(AM_AUDIO_CTRL_T *);
// void __am_audio_status(AM_AUDIO_STATUS_T *);
// void __am_audio_play(AM_AUDIO_PLAY_T *);
// void __am_disk_config(AM_DISK_CONFIG_T *cfg);
// void __am_disk_status(AM_DISK_STATUS_T *stat);
// void __am_disk_blkio(AM_DISK_BLKIO_T *io);
// void __am_media_print(AM_MEDIA_PRINT_T *);

// static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
// static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }
// static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }
// static void __am_net_config (AM_NET_CONFIG_T *cfg)    { cfg->present = false; }

void __am_uart_status_r(AM_UART_STATUS_T* target);
void __am_uart_status_w(AM_UART_STATUS_T* target);
void __am_uart_get_data(AM_UART_RX_T* target);

typedef void (*handler_t)(void *buf);

static void *lut_r[128] = {
  [AM_UART_STATUS]  = __am_uart_status_r,
  [AM_UART_RX]      = __am_uart_get_data
};

static void *lut_w[128] = {
  [AM_UART_STATUS] = __am_uart_status_w,
  [AM_UART_RX]      = NULL
};



static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_gpu_init();
  __am_timer_init();
  __am_audio_init();
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut_r[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut_w[reg])(buf); }
