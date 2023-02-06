#include <am.h>
#include <unisys.h>
#include <klib-macros.h>

void __am_timer_init();
void __am_gpu_init();
void __am_audio_init();
void __am_uart_init();

void __am_uart_status_r(AM_UART_STATUS_T *);
void __am_uart_status_w(AM_UART_STATUS_T *);
void __am_uart_get_data(AM_UART_RX_T *);
void __am_uart_send_data(AM_UART_TX_T *);

void __am_input_keybrd(AM_INPUT_KEYBRD_T *);

void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_timer_intr(AM_TIMER_INTR_T *);

void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);

void __am_audio_config(AM_AUDIO_CONFIG_T *);
void __am_audio_ctrl(AM_AUDIO_CTRL_T *);
void __am_audio_status(AM_AUDIO_STATUS_T *);
void __am_audio_play(AM_AUDIO_PLAY_T *);

void __am_disk_config(AM_DISK_CONFIG_T *cfg);
void __am_disk_status(AM_DISK_STATUS_T *stat);
void __am_disk_blkio(AM_DISK_BLKIO_T *io);

void __am_timer_config(AM_TIMER_CONFIG_T *cfg);
void __am_input_config(AM_INPUT_CONFIG_T *cfg);
void __am_uart_config(AM_UART_CONFIG_T *cfg);
static void __am_net_config (AM_NET_CONFIG_T *cfg) { cfg->present = false; }



typedef void (*handler_t)(void *buf);

static void *lut_r[128] = {
  [AM_UART_CONFIG]  = __am_uart_config_r,
  [AM_UART_TX]      = NULL,
  [AM_UART_RX]      = __am_uart_get_data,
  [AM_UART_STATUS]  = __am_uart_status_r,
  [AM_UART_CTAL]    = __am_uart_ctrl_r,
  [AM_UART_BAUD]    = __am_uart_baud_r,
  [AM_TIMER_UPTIME] = __am_timer_uptime_r,
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC]    = __am_timer_rtc_r,
  [AM_TIMER_INTR]   = __am_timer_intr_r
};

static void *lut_w[128] = {
  [AM_UART_CONFIG]  = NULL,
  [AM_UART_TX]      = __am_uart_send_data,
  [AM_UART_RX]      = NULL,
  [AM_UART_STATUS] = __am_uart_status_w,
  [AM_UART_CTAL]    = __am_uart_ctrl_w,
  [AM_UART_BAUD]    = __am_uart_baud_w,
  [AM_TIMER_UPTIME] = NULL,
  [AM_TIMER_CONFIG] = NULL,
  [AM_TIMER_RTC]    = __am_timer_rtc_w,
  [AM_TIMER_INTR]   = __am_timer_intr_w
};



static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut_r); i++)
    if (!lut_r[i]) lut_r[i] = fail;
  for (int i = 0; i < LENGTH(lut_w); i++)
    if (!lut_w[i]) lut_w[i] = fail;
  __am_gpu_init();
  __am_uart_init();
  __am_timer_init();
  __am_audio_init();
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut_r[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut_w[reg])(buf); }
