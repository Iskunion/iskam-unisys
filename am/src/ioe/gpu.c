#include <am.h>
#include <unisys.h>
#include <klib-macros.h>

#define CTRL_ADDR (VGA_BASE + 0x00)
#define SYNC_ADDR (VGA_BASE + 0x04)
#define FB_ADDR   FB_BASE

static uint16_t screen_width, screen_height;

#define SCREEN_WIDTH screen_width
#define SCREEN_HEIGHT screen_height

enum {
  FONT_NONE = 0,
  FONT_FGBLACK = 30,
  FONT_FGRED,
  FONT_FGGREEN,
  FONT_FGYELLOW,
  FONT_FGBLUE,
  FONT_FGPURPLE,
  FONT_FGWHITE,
  FONT_BGBLACK = 40,
  FONT_BGRED,
  FONT_BGGREEN,
  FONT_BGYELLOW,
  FONT_BGBLUE,
  FONT_BGPURPLE,
  FONT_BGWHITE,
  FONT_LFGBLACK = 90,
  FONT_LFGRED,
  FONT_LFGGREEN,
  FONT_LFGYELLOW,
  FONT_LFGBLUE,
  FONT_LFGPURPLE,
  FONT_LFGWHITE
};

#define ASCII_HEIGHT 16
#define ASCII_WIDTH 9
//aka 6
#define ASCII_NR ((ASCII_HEIGHT * ASCII_WIDTH + 7) / 8)
#define CHAR_SIZE (ASCII_HEIGHT * ASCII_WIDTH)

#define TERMINAL_WIDTH (SCREEN_WIDTH / ASCII_WIDTH)
#define TERMINAL_HEIGHT (SCREEN_HEIGHT / ASCII_HEIGHT)
#define TERMINAL_NR (TERMINAL_HEIGHT * TERMINAL_WIDTH)

typedef struct Terminal_cell{
  unsigned char content;
  uint8_t fgcolor, bgcolor;
  bool dirty;
} Terminal_cell;

Terminal_cell *terminal_cells;
extern uint8_t ascii_image[];

void __am_gpu_init() {
  int i;
  screen_width = inw(CTRL_ADDR + 0x02);
  screen_height = inw(CTRL_ADDR);
  int w = SCREEN_WIDTH;  // TODO: get the correct width
  int h = SCREEN_HEIGHT;  // TODO: get the correct height
  printf("detected screen width: %d height: %d\n", w, h);
  uint8_t *fb = (uint8_t *) (uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 125;
  outl(SYNC_ADDR, 1);
  terminal_cells = malloc(TERMINAL_HEIGHT * TERMINAL_WIDTH * sizeof(Terminal_cell));
  for (i = 0; i < TERMINAL_HEIGHT * TERMINAL_WIDTH; i++)
    terminal_cells[i] = (Terminal_cell) {.bgcolor = 0, .fgcolor = 255, .dirty = 1, .content = ' '};
}

static void __am_write_ascii(Terminal_cell *target, int i, int j) {
  uint8_t *buffer = malloc(CHAR_SIZE);
  uint8_t *nowimage = ascii_image + (((int) target->content - 0x20) << 4);
  for (int i = 0; i < ASCII_HEIGHT; i++) {
    uint8_t nowline = nowimage[i];
    for (int j = 0; j < ASCII_WIDTH - 1; j++) {
      uint32_t current_pos = i * ASCII_WIDTH + j;
      if (nowline & (1ull << j))
        buffer[current_pos] = target->fgcolor;
      else buffer[current_pos] = target->bgcolor;
    }
    buffer[i * ASCII_WIDTH + ASCII_WIDTH - 1] = target->bgcolor;
  }
  io_write(AM_GPU_DRAW256, j * ASCII_WIDTH, i * ASCII_HEIGHT, buffer, ASCII_WIDTH, ASCII_HEIGHT, 0);
  target->dirty = false;
}

void __am_gpu_chscroll(AM_GPU_CHSCROLL_T * cfg) {

}

void __am_gpu_terminal(AM_GPU_TERMINAL_T *cfg) {
  if (cfg->clear) {
    for (int i = 0; i < TERMINAL_NR; i++) {
      terminal_cells[i] = (Terminal_cell) {
        .dirty = 1,
        .content = ' ',
        .bgcolor = 0,
        .fgcolor = 255
      };
    }
  }
  else if(cfg->content) {
    terminal_cells[cfg->j + cfg->i * TERMINAL_WIDTH] = (Terminal_cell) {
      .dirty = 1,
      .content = cfg->content,
      .fgcolor = cfg->fg_color,
      .bgcolor = cfg->bg_color
    };
  }

  if(cfg->sync) {
    for (int i = 0; i < TERMINAL_HEIGHT; i++)
      for (int j = 0; j < TERMINAL_WIDTH; j++) {
        int now = j + i * TERMINAL_WIDTH;
        if (terminal_cells[now].dirty)
          __am_write_ascii(terminal_cells + now, i, j);
      }
    sync_vga();
  }
  return ;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
    .vmemsz = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint8_t)
  };
  return ;
}

void __am_gpu_chconfig(AM_GPU_CHCONFIG_T *cfg) {
  *cfg = (AM_GPU_CHCONFIG_T) {
    .present = true,
    .height = TERMINAL_HEIGHT,
    .width = TERMINAL_WIDTH
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
  // printf("drawing: w %d h %d pixels at %p\n", ctl->w, ctl->h, ctl->pixels);
  uint8_t *pixels = ctl->pixels;
  for (int i = 0; i < ctl->h; i++)
    for (int j = 0; j < ctl->w; j++)
      outb(FB_ADDR + (ctl->x + j + SCREEN_WIDTH * (ctl->y + i)), pixels[j + ctl->w * i]);
  if(ctl->sync) outl(SYNC_ADDR, 1);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
