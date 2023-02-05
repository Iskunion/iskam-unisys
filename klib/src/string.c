#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  char *str;
  for (str = (char *) s; *str; str++);
  return str - s;
}

char *strcpy(char *dst, const char *src) {
  if (!dst || !src) return NULL;
  char *_src, *_dst;
  for (_src = (char *) src, _dst = dst; *_src; _src++, _dst++) *_dst = *_src;
  *_dst = *_src;
  return dst;
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  if (!dst || !src) return NULL;
  char *_src = (char *) src, *_dst = dst;
  while (n--) {
    *_dst = *_src;
    _dst++;
    if(*_src) _src++;
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  if (!dst || !src) return NULL;
  char *_dst = dst;
  while (*_dst) _dst++;
  strcpy(_dst, src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  if (!s1 || !s2) return 0;
  unsigned char *_1 = (unsigned char *) s1, *_2 = (unsigned char *) s2;
  for (;;_1++, _2++) {
    if (*_1 < *_2) return -1;
    if (*_1 > *_2) return 1;
    if (!*_1 || !*_2) break;
  }
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  unsigned char *_1 = (unsigned char *) s1, *_2 = (unsigned char *) s2;
  while (n--) {
    if (*_1 < *_2) return -1;
    if (*_1 > *_2) return 1;
    if (!*_1 || !*_2) break;
    _1++; _2++;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  if (!s) return NULL;
  unsigned char *_s = s;
  while (n--) *_s++ = c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  return memcpy(dst, src, n);
}

void *memcpy(void *out, const void *in, size_t n) {
  if (!out || !in) return NULL;
  char *dst = out, *src = (char *) in;
  if (dst >= src && dst <= src + n -1) {
    dst = dst + n - 1;
    src = src + n - 1;
    while (n--)
      *dst-- = *src--;
  }
  else{
    while (n--)
      *dst++ = *src++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char *_1 = s1, *_2 = s2;
  return strncmp(_1, _2, n);
}

#endif
