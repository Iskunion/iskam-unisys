#include <am.h>
#include <unisys.h>

static uint64_t time_base = 0;

static uint64_t get_time_from_device(){
  uint64_t highhalf = inl(RTC_ADDR + 4);
  uint64_t lowhalf = inl(RTC_ADDR);
  uint64_t device_time = (highhalf << 32) | lowhalf;
  return device_time;
}

void __am_timer_init() {
  time_base = get_time_from_device();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = get_time_from_device() - time_base;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
