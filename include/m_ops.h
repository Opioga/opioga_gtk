/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the 
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE. 
 *
 * (i.e. the MIT License)
 *
 * code.c
 * General emulation functions
 */

#ifndef _M_OPS_H_
#define _M_OPS_H_
#include <emu8086.h>
void xor_addr8_i8(struct emu8086 *aCPU, int *handled);
void sub_addr8_i8(struct emu8086 *aCPU, int *handled);
void sbb_addr8_i8(struct emu8086 *aCPU, int *handled);
void adc_addr8_i8(struct emu8086 *aCPU, int *handled);
void and_addr8_i8(struct emu8086 *aCPU, int *handled);
void or_addr8_i8(struct emu8086 *aCPU, int *handled);
void add_addr8_i8(struct emu8086 *aCPU, int *handled);
void cmp_addr8_i8(struct emu8086 *aCPU, int *handled);
void xor_addr16_s8(struct emu8086 *aCPU, int *handled);
void sub_addr16_s8(struct emu8086 *aCPU, int *handled);
void sbb_addr16_s8(struct emu8086 *aCPU, int *handled);
void adc_addr16_s8(struct emu8086 *aCPU, int *handled);
void and_addr16_s8(struct emu8086 *aCPU, int *handled);
void or_addr16_s8(struct emu8086 *aCPU, int *handled);
void add_addr16_s8(struct emu8086 *aCPU, int *handled);
void cmp_addr16_s8(struct emu8086 *aCPU, int *handled);
void mov_addr_cs(struct emu8086 *aCPU, int *handled);
void mov_addr_ds(struct emu8086 *aCPU, int *handled);
void mov_addr_es(struct emu8086 *aCPU, int *handled);
void mov_addr_ss(struct emu8086 *aCPU, int *handled);
void xor_addr16_d16(struct emu8086 *aCPU, int *handled);
void sub_addr16_d16(struct emu8086 *aCPU, int *handled);
void sbb_addr16_d16(struct emu8086 *aCPU, int *handled);
void adc_addr16_d16(struct emu8086 *aCPU, int *handled);
void and_addr16_d16(struct emu8086 *aCPU, int *handled);
void or_addr16_d16(struct emu8086 *aCPU, int *handled);
void add_addr16_d16(struct emu8086 *aCPU, int *handled);
void cmp_addr16_d16(struct emu8086 *aCPU, int *handled);
void mov_cs_addr(struct emu8086 *aCPU, int *handled);
void mov_ds_addr(struct emu8086 *aCPU, int *handled);
void mov_es_addr(struct emu8086 *aCPU, int *handled);
void mov_ss_addr(struct emu8086 *aCPU, int *handled);
void or_addr16_s8(struct emu8086 *aCPU, int *handled);
void and_addr16_s8(struct emu8086 *aCPU, int *handled);
void sub_addr16_s8(struct emu8086 *aCPU, int *handled);
void xor_addr16_s8(struct emu8086 *aCPU, int *handled);

void dec_addr8(struct emu8086 *aCPU, int *handled);
void cmp_addr16_s8(struct emu8086 *aCPU, int *handled);
void push_to_stack(struct emu8086 *aCPU, int value);
#endif