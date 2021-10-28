#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  int64_t result = 0;
  __asm__(
  "movq %1, %0;"\
  "addq %2, %0":
  "=r" (result):
  "m" (a), "m" (b)
  );
  return result;
}

int asm_popcnt(uint64_t x) {
  uint64_t s = 0;
  uint64_t c1 = 0x5555555555555555, c2 = 0x3333333333333333, c3 = 0x0F0F0F0F0F0F0F0F, 
    c4 = 0x00FF00FF00FF00FF, c5 = 0x0000FFFF0000FFFF, c6 = 0x00000000FFFFFFFF;
  //有效在%0中
  __asm__(
  "movq %2, %0;"\
  "shrq $1, %2;"\
  "andq %3, %2;"\
  "andq %3, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;"\

  "shrq $2, %2;"\
  "andq %4, %2;"\
  "andq %4, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;"\

  "shrq $4 ,%2;"\
  "andq %5, %2;"\
  "andq %5, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;"\

  "shrq $8, %2;"\
  "andq %6, %2;"\
  "andq %6, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;"\

  "shrq $16, %2;"\
  "andq %7, %2;"\
  "andq %7, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;"\

  "shrq $32, %2;"\
  "andq %8, %2;"\
  "andq %8, %0;"\
  "addq %2, %0;"\
  "movq %0, %2;":
  "=r" (s):
  "0" (s), "r" (x), "m"(c1), "m"(c2), "m"(c3), "m"(c4), "m"(c5), "m"(c6)
  );
  
  // for (int i = 0; i < 64; i++) {
  //   if ((x >> i) & 1) s++;
  // }
  return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  __asm__(
    "mov $0, %%rsi;"\
    "1:;"\
    "movb (%0, %%rsi), %%al;"\
    "movb %%al, (%1, %%rsi);"\
    "inc %%rsi;"\
    "cmp %%rsi, %2;"\
    "jg 1b":
    :
    "r"(src), "r"(dest), "r"(n):
    "memory", "rsi", "rax"
  );
  
  return dest;
  // return memcpy(dest, src, n);
}

int asm_setjmp(asm_jmp_buf env) {
  __asm__(
    "movq (%%rbp), %%eax;"\
    "movq %%rax, 8(%0);"\
    // #存rbx
    "movq %%rbx, (%0);"\
    "movq %%rsi, 16(%0);"\
    // #存rdi
    "movq %%rdi, 24(%0);"\
    "leaq 16(%%rbp), %%rax;" \
    // #存rsp
    "movq %%rax, 32(%0);"\
    //存返回地址rip
    "movq 8(%%rbp), %%rax;"\
    "movq %%rax, 40(%0)":
    :
    "c" (env):
    "memory", "rax"
  );

  return 0;
  //return setjmp(env);
}

void asm_longjmp(asm_jmp_buf env, int val) {
  __asm__(
    "movl %1, %%rax;"\
    // 恢复rbx
    "movq (%0), %%rbx;"\
    // 恢复rbp
    "movq 8(%0), %%rbp;"\
    // 恢复rsi
    "movq 16(%0), %%rsi;"\
    // 恢复rdi
    "movq 24(%0), %%rdi;"\
    // 恢复rsp
    "movq 32(%0), %%rsp;"\
    // 取rip
    "movq 40(%0), %%rcx;"\
    //恢复rip
    "jmp *%%rcx;":
    :"c" (env), "m" (val)
  );

  //longjmp(env, val);
}
