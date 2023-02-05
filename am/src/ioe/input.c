#include <am.h>
#include <unisys.h>

void __am_input_config(AM_INPUT_CONFIG_T *cfg) {
  cfg->present = false;
}

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  // short code = inl(KBD_ADDR);
  // kbd->keydown = code < 0;
  // kbd->keycode = (char) code;
}
