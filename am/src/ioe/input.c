#include <am.h>
#include <unisys.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  short code = inl(KBD_ADDR);
  kbd->keydown = code < 0;
  kbd->keycode = (char) code;
}
