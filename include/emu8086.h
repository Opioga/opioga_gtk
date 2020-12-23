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

#ifndef _EMU_8086_H_
#define _EMU_8086_H_
#define DEBUG 1
#define MAX_CALL_STACK 10
struct emu8086;

typedef void (*emu8086op)(struct emu8086 *aCPU, int *handled);

struct instruction
{

    struct instruction *next;
    struct instruction *prev;
    int line_number;
    int starting_address;
    int end_address;
    int is_16;
};
struct variable
{

    struct variable *right;
    struct variable *left;
    int line_number;
    int starting_address;
    int end_address;
    char name[20];
    int size;
    int offset;
    //int is_16;
};
struct instruction *_instruction_list;
struct instruction *_last_instruction;
struct instruction *_current_instruction, *_first_instruction;
struct variable *variable_list, *first_variable, *v_ordered_list;

struct emu8086
{
    int mCodeMemSize;
    emu8086op op[256]; // callback: SFR register written

    int mMemSize;
    int end_address;
    unsigned char mDataMem[0xfffff];
    char is_first;
    int code_start_addr;
    int mSFR[22];
    int skip_next;
    struct instruction *instructions_list;
    struct instruction *instruction_cache[MAX_CALL_STACK];
    struct instruction *instruction_cache_loop;
    char call_stack;
    struct variable *variable_list;
    __uint16_t last_ip;
};
enum MESSAGES_
{
    LOG = 0,
    WARN = 2,
    ERR = 1
};
enum registers_16
{
    REG_AX,
    REG_CX,
    REG_DX,
    REG_BX,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,

    REG_8,
    REG_9,
    REG_10,
    REG_11,
    REG_12,
    REG_13,
    REG_14,
    REG_15,

    REG_FLAGS,
    REG_IP,
    REG_CS,
    REG_DS,
    REG_ES,
    REG_SS

};

struct emu8086 *emu8086_new(void);
void do_assembly(struct emu8086 *aCPU, char *fname);
void sfrread(struct emu8086 *aCPU, int aRegister);
void sfrwrite(struct emu8086 *aCPU, int aRegister);

#define AX aCPU->mSFR[REG_AX]
#define BX aCPU->mSFR[REG_BX]
#define CX aCPU->mSFR[REG_CX]
#define DX aCPU->mSFR[REG_DX]
#define ES aCPU->mSFR[REG_ES]
#define CS aCPU->mSFR[REG_CS]
#define SP aCPU->mSFR[REG_SP]
#define BP aCPU->mSFR[REG_BP]
#define SI aCPU->mSFR[REG_SI]
#define DI aCPU->mSFR[REG_DI]
#define DS aCPU->mSFR[REG_DS]
#define REG_8 aCPU->mSFR[REG_8]
#define REG_9 aCPU->mSFR[REG_9]
#define REG_10 aCPU->mSFR[REG_10]
#define REG_11 aCPU->mSFR[REG_11]
#define REG_12 aCPU->mSFR[REG_12]
#define REG_13 aCPU->mSFR[REG_13]
#define REG_14 aCPU->mSFR[REG_14]
#define REG_15 aCPU->mSFR[REG_15]
#define FLAGS aCPU->mSFR[REG_FLAGS]
#define IP aCPU->mSFR[REG_IP]
#define _SS aCPU->mSFR[REG_SS]
#define CODE_SEGMENT aCPU->mDataMem + (CS * 0x10)
#define CODE_SEGMENT_IP aCPU->mDataMem + (CS * 0x10) + IP
#define DATA_SEGMENT aCPU->mDataMem + (DS * 0x10)
#define EXTRA_SEGMENT aCPU->mDataMem + (ES * 0x10)
#define GET_FLAG(f) (FLAGS >> f) & 1
#define SET_FLAG(f) FLAGS |= (1 << f)
#define CLEAR_FLAG(f) FLAGS &= (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS);

#define STACK_SEGMENT aCPU->mDataMem + (_SS * 0x10)
#define INSTRUCTIONS aCPU->instruction_list
#define _INSTRUCTIONS aCPU->instructions_list
#define SFRS aCPU->mSFR
void xor_addr8_i8(struct emu8086 *aCPU, int *handled);
void sub_addr8_i8(struct emu8086 *aCPU, int *handled);
void and_addr8_i8(struct emu8086 *aCPU, int *handled);
void or_addr8_i8(struct emu8086 *aCPU, int *handled);
void add_addr8_i8(struct emu8086 *aCPU, int *handled);
void cmp_addr8_i8(struct emu8086 *aCPU, int *handled);

void mov_addr_cs(struct emu8086 *aCPU, int *handled);
void mov_addr_ds(struct emu8086 *aCPU, int *handled);
void mov_addr_es(struct emu8086 *aCPU, int *handled);
void mov_addr_ss(struct emu8086 *aCPU, int *handled);

void mov_cs_addr(struct emu8086 *aCPU, int *handled);
void mov_ds_addr(struct emu8086 *aCPU, int *handled);
void mov_es_addr(struct emu8086 *aCPU, int *handled);
void mov_ss_addr(struct emu8086 *aCPU, int *handled);
void or_addr16_s8(struct emu8086 *aCPU, int *handled);
void and_addr16_s8(struct emu8086 *aCPU, int *handled);
void sub_addr16_s8(struct emu8086 *aCPU, int *handled);
void xor_addr16_s8(struct emu8086 *aCPU, int *handled);

void cmp_addr16_s8(struct emu8086 *aCPU, int *handled);
void push_to_stack(struct emu8086 *aCPU, int value);
void op_setptrs(struct emu8086 *aCPU);

void dump_stack(struct emu8086 *aCPU);

void dump_memory(struct emu8086 *aCPU);

void dump_registers(struct emu8086 *aCPU);

#endif //_HEADER_FILE_H_
