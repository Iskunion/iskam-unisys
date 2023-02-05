#include <am.h>
#include <unisys.h>

//from nemu
enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14) 
#define AUDIO_WPTR_ADDR      (AUDIO_ADDR + 0x18)

void __am_audio_init() {

}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = false;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}


void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  //fill the regs
  outl(AUDIO_FREQ_ADDR,     ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR,  ctrl->samples);
  //trigger the initialization
  outl(AUDIO_INIT_ADDR,     20030429);
  return ;
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

static inline int rem_length() {
  return inl(AUDIO_SBUF_SIZE_ADDR) - inl(AUDIO_COUNT_ADDR);
}


void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  // size_t req_length = ctl->buf->end - ctl->buf->begin;
  // while (req_length < rem_length()) ;
  // const unsigned char *now = ctl->buf->begin;
  // for (; now != ctl->buf->end; now++) {
  //   outl(AUDIO_SBUF_ADDR, )
  // }

}
