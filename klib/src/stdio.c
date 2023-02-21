#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char* fpt(uint32_t number, char *answer)
{
  if (number >= 10)
    answer = fpt(number/10, answer);
  number %= 10;
  *answer++ = number + '0';
  return answer;
}

static char* fptd(uint32_t number, char *answer)
{
  if (number >> 4)
    answer = fptd(number >> 4, answer);
  number &= 15;
  *answer++ = number < 10 ? (number + '0') : (number - 10 + 'a');
  return answer;
}

static char* itos(int number, char *answer, char type, int limit, char occupy){
  char numbuf[20];
  int numlength = 0;
  switch (type)
  {
    case 'p':
      *answer++ = '0';
      *answer++ = 'x';
      limit = 8;
      occupy = '0';
    case 'x':
      numlength = fptd((uint32_t) number, numbuf) - numbuf; break;
    case 'd':
      if (number < 0) number = -number, *answer++ = '-';
    case 'u':
      numlength = fpt((uint32_t) number, numbuf) - numbuf; break;
    default:
      return answer;
  }
  while (numlength < limit)
    *answer++ = occupy, limit--;
  strncpy(answer, numbuf, numlength);
  answer += numlength;
  return answer;
}

static bool isdigit(char a){
  return ((a >> 4) == 3) && ((a & 15) < 10);
}

int printf(const char *fmt, ...) {
  char out[4096];
  int val;
  va_list args;
  va_start(args, fmt);
  val = vsprintf(out, fmt, args);
  va_end(args);
  putstr(out);
  return val;
}

int fprint(void (*putc)(char), const char *fmt, ...) {
  char out[4096];
  int val;
  va_list args;
  va_start(args, fmt);
  val = vsprintf(out, fmt, args);
  va_end(args);
  for (const char *p = out; *p; p++) putc(*p);
  return val;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *_o, *s, c;
  int num;
  for(_o = out; *fmt; fmt++){
    if(*fmt != '%') {
      *_o++ = *fmt;
      continue;
    }
    //omit '%'
    fmt++;
    char occupy = ' ';
    int limit = 0;
restart:
    switch(*fmt){
      case 'd':
      case 'x':
      case 'p':
      case 'u':
        num = va_arg(ap, int);
        _o = itos(num, _o, *fmt, limit, occupy);
        break;
      case 'c':
        c = va_arg(ap, int);
        *_o++ = c;
        break;
      case 's':
        s = va_arg(ap, char*);
        strcpy(_o, s);
        _o += strlen(s);
        break;
      case '0':
        occupy = '0';
        fmt++;
        goto restart;
      default:
        if (isdigit(*fmt)) {
          limit = (*fmt & 15);
          fmt++;
          goto restart;
        }
        break;
    }
  }
  *_o = '\0';
  return _o - out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int val;
  va_start(args, fmt);
  val = vsprintf(out, fmt, args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
