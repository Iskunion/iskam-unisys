#include <am.h>
#include <unisys.h>

// #define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

// #define SCREEN_WIDTH (inw(VGACTL_ADDR + 2))
// #define SCREEN_HEIGHT (inw(VGACTL_ADDR + 0))
// #define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t))

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = false, .has_accel = false,
  //   .width = SCREEN_WIDTH,
  //   .height = SCREEN_HEIGHT,
  //   .vmemsz = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t)
  };
  return ;
}
 
// void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
//   uint32_t *pixels = ctl->pixels;
//   for(int i = 0; i < ctl->w; i++)
//     for(int j = 0; j < ctl->h; j++)
//       if(ctl->x + i < SCREEN_WIDTH && ctl->y + j < SCREEN_HEIGHT)
//         outl(FB_ADDR + (ctl->x + i + SCREEN_WIDTH * (ctl->y + j)) * 4, pixels[i + ctl->w * j]);
//   if(ctl->sync) outl(SYNC_ADDR, 1);
// }

// void __am_gpu_status(AM_GPU_STATUS_T *status) {
//   status->ready = true;
// }
