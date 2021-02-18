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
 * emu8086.h
 * Runner class
 */



#ifndef _EMU_8086_H_
#define _EMU_8086_H_

#define MAX_CALL_STACK 10
struct emu8086;

typedef void (*emu8086op)(struct emu8086 *aCPU, int *handled);

struct instruction
{

    struct instruction *next;
    struct instruction *prev;
    
 /*
    The line of code which the instruction  represents
 */
    int line_number;
    
    int starting_address;
    int end_address;
    int is_16;
    struct instruction *cache;
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

struct emu8086
{
    int mCodeMemSize;
    
/*
    An array of callbacks with each index containing a callback that emulates an 
    8086 opcode
*/
    emu8086op op[256]; 

    int mMemSize;
/*
    Represents the address last address of the emulated memory that contains a
    relevant opcode.
*/
    int end_address;

/*
    An array with each index representing a memory address.
*/    
    unsigned char *mDataMem;
    
/*
    Indicates whether the emulated CPU is about to execute it's first instruction
*/
    char is_first;
    int code_start_addr;

/*
    An array with each index representing an 8086 special function register
*/
    unsigned short mSFR[22];
    
/*
    Indicates whether we should skip incrementing the IP
*/
    int skip_next;
    
/*
    Linked list of instructions to emulate
*/
    struct instruction *instructions_list;
    
/*
    An Array of instructions following call instructions for example
    10 call proc
    11 stc
    Instruction on line 11 is stored in this array to optimize efficiency

*/
    struct instruction *instruction_cache[MAX_CALL_STACK];
    
/*
    Holds the instruction that is located at the beginning of a loop
*/
    struct instruction *instruction_cache_loop;
    
/*
    Holds the current location in the instruction_cache
*/
    int call_stack;
    struct variable *variable_list;
    int port;
    int last_ip;
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
#define SET_BIT(f, s) f |= (1 << s)
#define CLEAR_BIT(f, s, l) f &= (((~f & l) | (1 << s)) ^ f);

#define CLEAR_FLAG(f) FLAGS &= (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS);
#define IS_SET(f, s) (f >> s) & 1
void op_setptrs(struct emu8086 *aCPU);

#define STACK_SEGMENT aCPU->mDataMem + (_SS * 0x10)
#define INSTRUCTIONS aCPU->instruction_list
#define _INSTRUCTIONS aCPU->instructions_list
#define SFRS aCPU->mSFR

void dump_stack(struct emu8086 *aCPU);

void dump_memory(struct emu8086 *aCPU);

void dump_registers(struct emu8086 *aCPU);

#endif //_HEADER_FILE_H_
