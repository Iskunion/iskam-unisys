#include <am.h>
#include <unisys.h>

#define KBD_ADDR  (KBD_BASE + 0x04)
//low 16 key code    up : 
void __am_input_config(AM_INPUT_CONFIG_T *cfg) {
  cfg->present = true;
}

typedef struct kbd_reg{
  unsigned char keycode;
  unsigned char extcode;
}__attribute__((packed)) kbd_reg;


void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint16_t keydata = inw(KBD_ADDR);
  kbd_reg keyreg = *(kbd_reg *) &keydata;
  // while (keyreg.keycode == AM_KEY_NONE) {
  //   keydata = inw(KBD_ADDR);
  //   keyreg = *(kbd_reg *) &keydata;
  // }
  kbd->keycode = keyreg.keycode;
  kbd->keydown = (keyreg.extcode != 0xf0);
}
