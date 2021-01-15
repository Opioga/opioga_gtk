/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * m_ops.h
 * CPU class
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
void neg_addr8(struct emu8086 *aCPU, int *handled);
void not_addr8(struct emu8086 *aCPU, int *handled);
void mul_addr8(struct emu8086 *aCPU, int *handled);
void push_to_stack(struct emu8086 *aCPU, int value);
#endif