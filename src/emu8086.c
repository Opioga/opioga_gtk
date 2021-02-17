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
 * emu8086.c
 * CPU class
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <emu8086.h>

//void message();

/* 
    assm.c
*/

/* emu.c */
struct emu8086 *emu8086_new(void)
{
    struct emu8086 *new = (struct emu8086 *)malloc(sizeof(struct emu8086));
    if (new == NULL)
    {
        fprintf(stderr, "Out of memory for use \n");
        exit(1);
        return NULL;
    }

    if (new->mSFR == NULL)
    {
        fprintf(stderr, "Out of memory for sfr \n");
        exit(1);
        return NULL;
    }
    new->skip_next = 0;
    for (int i = 0; i < 22; i++)
        new->mSFR[i] = 0x0000;
    
    new->mDataMem = (unsigned char *)calloc((0xfffff + 1), sizeof(unsigned char ) );
    op_setptrs(new);
    new->instructions_list = NULL;
    new->call_stack = 0;
    new->instruction_cache_loop = NULL;
    new->is_first = 1;
    new->port = -1;
    //new->sfrread((struct struct emu8086 *)new, REG_DS);
    return new;
}
/*end emu.c */

// ops.c

// {
//     struct emu8086 *aCPU = emu8086_new();
//     op_setptrs(aCPU);
//     do_assembly(aCPU, "test.asm");
//     assembler_step = 1;
//     do_assembly(aCPU, "test.asm");
//     CS = aCPU->code_start_addr / 0x10;
//     DS = 0x03ff;

//     _SS = ((CS * 0x10) - 0x20000) / 0x10;
//     while (IP < aCPU->end_address - 1)
//     {
//         int op = *(CODE_SEGMENT_IP), handled = 0;

//         aCPU->op[op](aCPU, &handled);
//         if (!handled)
//         {
//             char buf[15];
//             sprintf(buf, "Unhandled instrution on line %d, opcode: %x", _INSTRUCTIONS->line_number, op); //message()
//             message(buf, ERR);
//         }
//         if (aCPU->skip_next)
//             aCPU->skip_next = 1;
//         else if (_INSTRUCTIONS->next != NULL)
//             _INSTRUCTIONS = _INSTRUCTIONS->next;
//         if (IP > 0xffff)
//         {
//             IP = IP - 0xffff;
//             CS = CS - 0x10000;
//         }
//         handled = 0;
//     }
//     dump_registers(aCPU);
// }