#include <am.h>
#include <unisys.h>

void __am_media_print(AM_MEDIA_PRINT_T *chshell) {
    outb(PRINT_ADDR, chshell->ch);
}