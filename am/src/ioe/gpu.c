#include <am.h>
#include <unisys.h>
#include <klib-macros.h>

#define CTRL_ADDR (VGA_BASE + 0x00)
#define SYNC_ADDR (VGA_BASE + 0x04)
#define FB_ADDR   FB_BASE

#define SCREEN_WIDTH (inw(CTRL_ADDR + 0x02))
#define SCREEN_HEIGHT (inw(CTRL_ADDR))

void __am_gpu_init() {
  int i;
  int w = SCREEN_WIDTH;  // TODO: get the correct width
  int h = SCREEN_HEIGHT;  // TODO: get the correct height
  printf("detected screen width: %d height: %d\n", w, h);
  uint8_t *fb = (uint8_t *) (uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 125;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = false, .has_accel = false,
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
    .vmemsz = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint8_t)
  };
  return ;
}

typedef struct rgb_8bit
{
  uint8_t blue  : 2;
  uint8_t green : 3;
  uint8_t red   : 3;
}rgb_8bit;

struct rgb_32bit
{
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t alpha;
}__attribute__((packed));

typedef struct rgb_32bit rgb_32bit;

static inline uint8_t turn256c(uint32_t *pixel) {
  rgb_8bit target = (rgb_8bit) {
    .blue   = ((rgb_32bit *) pixel)->blue >> 6,
    .green  = ((rgb_32bit *) pixel)->green >> 5,
    .red    = ((rgb_32bit *) pixel)->red >> 5
  };
  return *(uint8_t *) &target;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (SCREEN_WIDTH  < ctl->x) return ;
  if (SCREEN_HEIGHT < ctl->y) return ;
  ctl->w = MIN((SCREEN_WIDTH - ctl->x),  ctl->w);
  ctl->h = MIN((SCREEN_HEIGHT - ctl->y), ctl->h);
  uint32_t *pixels = ctl->pixels;
  for (int i = 0; i < ctl->h; i++)
    for (int j = 0; j < ctl->w; j++)
      outb(FB_ADDR + (ctl->x + j + SCREEN_WIDTH * (ctl->y + i)), turn256c(pixels + j + ctl->w * i));
  if(ctl->sync) outl(SYNC_ADDR, 1);
}

void __am_gpu_fbdraw256(AM_GPU_DRAW256_T *ctl) {
  if (SCREEN_WIDTH  < ctl->x) return ;
  if (SCREEN_HEIGHT < ctl->y) return ;
  ctl->w = MIN((SCREEN_WIDTH - ctl->x),  ctl->w);
  ctl->h = MIN((SCREEN_HEIGHT - ctl->y), ctl->h);
  printf("drawing: w %d h %d pixels at %p\n", ctl->w, ctl->h, ctl->pixels);
  uint8_t *pixels = ctl->pixels;
  for (int i = 0; i < ctl->h; i++)
    for (int j = 0; j < ctl->w; j++)
      outb(FB_ADDR + (ctl->x + j + SCREEN_WIDTH * (ctl->y + i)), pixels[j + ctl->w * i]);
  if(ctl->sync) outl(SYNC_ADDR, 1);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
