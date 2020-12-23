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
    for (int i = 0; i < 0x10000; i++)
        *(new->mDataMem + 0x3ff0 + i) = 0;
    op_setptrs(new);
    new->instructions_list = NULL;
    new->call_stack = 0;
    new->instruction_cache_loop = NULL;
    new->is_first = 1;
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