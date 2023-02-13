#include <am.h>
#include <unisys.h>

#define RTC_ADDR    (TIMER_BASE + 0x00)
#define UPTIME_ADDR (TIMER_BASE + 0x04)
#define INTR_ADDR   (TIMER_BASE + 0x08)

static uint32_t time_base = 0;

static uint32_t get_time_from_device(){
  return inl(UPTIME_ADDR);
}

void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }

void __am_timer_init() {
  time_base = get_time_from_device();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = get_time_from_device() - time_base;
}

void __am_timer_intr_r(AM_TIMER_INTR_T *intr) {
  intr->us = inl(INTR_ADDR);
}

void __am_timer_intr_w(AM_TIMER_INTR_T *intr) {
  outl(INTR_ADDR, intr->us);
}

void __am_timer_rtc_r(AM_TIMER_RTC_T *rtc) {
  uint32_t rtcsec = inl(RTC_ADDR);
  rtc->second = rtcsec % 60;
  rtc->minute = (rtcsec / 60) % 60;
  rtc->hour   = (rtcsec / 3600) % 24;
  rtc->day    = (rtcsec / 86400);
  rtc->month  = 0;
  rtc->year   = 1970;
}

void __am_timer_rtc_w(AM_TIMER_RTC_T *rtc) {
  uint32_t rtc_yeardays = (rtc->year - 1970) * 365 + ((rtc->year - 1967) >> 2);
  uint32_t rtc_secs = rtc->day * 86400 + rtc->hour * 3600 + rtc->minute * 60 + rtc->second;
  outl(RTC_ADDR, rtc_secs);
}