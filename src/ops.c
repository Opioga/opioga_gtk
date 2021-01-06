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

#include <opcodes.h>
#include <m_ops.h>

extern struct instruction *_instruction_list;
extern struct instruction *_last_instruction;
extern struct instruction *_current_instruction, *_first_instruction;
extern struct variable *variable_list, *first_variable, *v_ordered_list;
enum err_index
{
    MAX_CALL,
    INVALID_RET,
    UNDEFINED,
    UNIMPLEMENTED

};

static char *errors_str[] = {
    "Max call stack on line %d",
    "Invalid ret on line %d",
    "Undefined label: on line %d", "Unimplemented instruction: on line %d"

};

int is_16 = 0;
extern void message(char *m, int level, int ln);
void massage(char *m, int level)
{
    message(m, level, 1);
};
void push_to_stack(struct emu8086 *aCPU, int value)
{
    *(STACK_SEGMENT + SP) = value & 0xff;
    SP--;
    *(STACK_SEGMENT + SP) = value >> 8;
    SP--;
}

void pop_from_stack(struct emu8086 *aCPU, int *value)
{
    SP++;
    *value = 0;
    *value = *(STACK_SEGMENT + SP) << 8;
    SP++;
    *value |= *(STACK_SEGMENT + SP);
}
void compare_set_flags(struct emu8086 *aCPU, int v1, int v2)
{
    int value = v1 - v2;
    if (value < 0)
    {
        FLAGS |= 129;
    }
    else if (value == 0)
    {
        FLAGS |= 64;
    }
}
void find_instruction16(struct emu8086 *aCPU)
{
    if (_INSTRUCTIONS == NULL)
    {
    }

    short off = *(CODE_SEGMENT_IP);
    //__uint128_t add = 0;

    IP++;
    off |= *(CODE_SEGMENT_IP) << 8;
    IP++;
    printf("leen: n%x\n", off);

    // off = off < 0 ? 0 - off : off;
    __uint128_t add = 0;
    // char is_back = off >> 7;
    _current_instruction = _INSTRUCTIONS;
    struct instruction *prev = _current_instruction->prev;
    struct instruction *next = _current_instruction->next;

    // if (_current_instruction->starting_address == *(CODE_SEGMENT + IP))
    // {
    printf("leen: n%d\n", off);

    //     IP = *(CODE_SEGMENT_IP);
    //     return;
    // }
    if (off >= 0)
        prev = NULL;
    else
        next = NULL;
    int b = IP + off;
    while (1)
    {
        if (prev != NULL)
        {
            if (prev->starting_address == b)
            {

                _current_instruction = prev;
                add = b;
                break;
            }
            else
                prev = prev->prev;
        }
        if (next != NULL)
        {
            if (next->starting_address == b)
            {

                _current_instruction = next;
                add = b;
                break;
            }
            else
                next = next->next;
        }
        if ((prev == NULL) && (next == NULL))
        {

            char buf[15];
            sprintf(buf, errors_str[UNDEFINED],
                    _current_instruction->line_number);
            massage(buf, ERR);
        }
    }
    // IP++;
    printf("l: \n\n%x\n\n", _current_instruction->starting_address);

    IP = off < 0 ? b : b > 0xff ? b + 2 : b + 1;
    _INSTRUCTIONS = _current_instruction;
    // if ()
}

void find_instruction(struct emu8086 *aCPU)
{
    if (_INSTRUCTIONS == NULL)
    {
    }

    char off = *(CODE_SEGMENT + IP);
    printf("lee: \n\n%d\n\n", off);
    IP++;
    // off = off < 0 ? 0 - off : off;
    __uint128_t add = 0;
    // char is_back = off >> 7;
    _current_instruction = _INSTRUCTIONS;
    struct instruction *prev = _current_instruction->prev;
    struct instruction *next = _current_instruction->next;
    if (off >= 0)
        prev = NULL;
    else
        next = NULL;
    int b = _current_instruction->end_address + off;
    if (_current_instruction->starting_address == b)
    {

        IP = b;
        return;
    }
    IP = _current_instruction->end_address;
    // printf("lee: \n\n%x %d , %x\n\n", b, off, _current_instruction->end_address);
    if (_current_instruction->end_address == b)
    { //  if(_current_instruction->end_address)

        // IP = (b + 1);
        _INSTRUCTIONS = _current_instruction->next;
        // return;
    }
    while (1)
    {
        if (prev != NULL)
        {
            if (prev->starting_address == b)
            {

                _current_instruction = prev;
                add = b; //+ 1;
                break;
            }
            else
                prev = prev->prev;
        }
        if (next != NULL)
        {
            if (next->starting_address == b)
            {

                _current_instruction = next;
                add = b;
                break;
            }
            else
                next = next->next;
        }
        if ((prev == NULL) && (next == NULL))
        {

            char buf[256];
            sprintf(buf, errors_str[UNDEFINED], _INSTRUCTIONS->line_number);
            massage(buf, ERR);
            break;
        }
    }
    // IP++;

    IP = b;
    printf("l: b = %x %d , %x\n\n", IP, off, _current_instruction->end_address);
    //    / if (off > 0)
    //         IP++;
    _INSTRUCTIONS = _current_instruction;
    // if ()
}

void find_instruction_call(struct emu8086 *aCPU)
{
    IP++;
    short value = *(CODE_SEGMENT_IP);
    int add = 0;

    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;

    _current_instruction = _INSTRUCTIONS;
    struct instruction *prev = _current_instruction->prev;
    struct instruction *next = _current_instruction->next;
    int _next = next != NULL ? next->starting_address : aCPU->end_address;

    if (value >= 0)
        prev = NULL;
    else
        next = NULL;
    int b = IP + value;
    if (SP == 0)
    {
        char buf[256];
        sprintf(buf, "Stack reached maximum: on line %d",
                _current_instruction->line_number);
        massage(buf, ERR);
    }
    if (b == 0xfffd)
    {
        IP = _current_instruction->starting_address;

        push_to_stack(aCPU, _next);

        return;
    }
    if (_current_instruction->starting_address == b + 1)
    {
        add = b;
    }
    else
    {
        while (1)
        {
            if (prev != NULL)
            {

                if (prev->starting_address == b + 1)
                {
                    add = b;

                    _current_instruction = prev;
                    break;
                }
                else
                    prev = prev->prev;
            }
            if (next != NULL)
            {
                if (next->starting_address == b + 1)
                {
                    add = b;
                    _current_instruction = next;
                    break;
                }
                else
                    next = next->next;
            }
            if ((prev == NULL) && (next == NULL))
            {
                char buf[256];
                sprintf(buf, errors_str[UNDEFINED],
                        _INSTRUCTIONS->line_number);
                massage(buf, ERR);
                break;
            }
        }
    }
    // IP++;
    IP = add > 0xff ? add + 2 : add + 1;
    printf("nn: %x\n", add);
    push_to_stack(aCPU, _next);
    //  }

    _INSTRUCTIONS = _current_instruction;
    // if ()
}

int high_reg = 0;
int get_ops_reg_8(struct emu8086 *aCPU, unsigned char opn, int **ops, int **dest)
{
    high_reg = 0;
    unsigned char mod = (opn & 0b11000000) >> 6,
                  reg = (opn & 0b00111000) >> 3,
                  r_m = opn & 0b111;
    if (mod != 3)
        return 0;
    high_reg = 0;

    if (!is_16)
    {
        if (r_m > 3)
        {
            high_reg++;
            r_m -= 4;
        }
        if (reg > 3)
        {
            high_reg += 2;
            reg -= 4;
        }
    }
    *dest = SFRS + r_m;
    *ops = SFRS + reg;

    return 1;
}

int get_ops_reg_8_addr(struct emu8086 *aCPU, unsigned char opn, int **ops, unsigned char **dest)
{
    high_reg = 0;
    unsigned char mod = (opn & 0b11000000) >> 6,
                  reg = (opn & 0b00111000) >> 3,
                  r_m = opn & 0b111;

    *dest = DATA_SEGMENT;
    if (mod == 0 && r_m == 6)
    {
        // special
        *dest = DATA_SEGMENT;
        IP++;
        int v = *(CODE_SEGMENT_IP);
        IP++;
        v |= *(CODE_SEGMENT_IP) << 8;
        *dest += v;
    }
    else
    {
        // special
        switch (r_m)
        {
        case 0:
            *dest = DATA_SEGMENT + BX + SI;

            break;
        case 1:
            *dest = DATA_SEGMENT + BX + DI;
            break;
        case 2:
            *dest = STACK_SEGMENT + BP + SI;
            break;
        case 3:
            *dest = STACK_SEGMENT + BP + DI;
            break;
        case 4:
            // printf("33jjjj\n");
            // special
            *dest = DATA_SEGMENT + SI;
            // printf("33jjjj\n, %d\n", **dest);

            break;
        case 5:
            *dest = DATA_SEGMENT + DI;
            break;
        case 6:
            *dest = STACK_SEGMENT + BP;
            break;
        case 7:
            *dest = DATA_SEGMENT + BX;
            break;
        }
    }
    if (reg > 3 && !is_16)
    {
        reg -= 4;
        high_reg = 1;
    }

    *ops = SFRS + reg;

    int width = mod, displacement = 0;
    if (width > 0)
    {
        IP++;
        displacement = *(CODE_SEGMENT_IP);
    }
    if (width > 1)
    {
        IP++;
        int d = 0;
        d = *(CODE_SEGMENT_IP);
        displacement |= d << 8;
    }

    *dest += displacement;
    return 1;
}

void setFlags(struct emu8086 *aCPU, int value)
{
    /*
O  D  I  T  S Z  A  P  C
11 10 9  8  7 6  4  2  0
    */
    if (is_16)
    {
        if (value > 255)
            FLAGS |= ((1 << 11) | (1 << 4));
        else
        {
            CLEAR_FLAG(11);
            CLEAR_FLAG(4);
        }
    }
    if (value > 0xffff)
        FLAGS |= ((1 << 11) | (1));
    else
    {
        CLEAR_FLAG(11);
        CLEAR_FLAG(1);
    }
    if (value == 0)
        FLAGS |= (1 << 6);

    else
    {
        CLEAR_FLAG(6);
    }
    if (value < 0)
    {
        FLAGS |= ((1 << 11) | (1 << 7) | 1);
        // FLAGS |= 1;
    }

    else
    {
        CLEAR_FLAG(11);
        CLEAR_FLAG(7);
        CLEAR_FLAG(0);
    }
}
// add

void add_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value += *op2;
        *op2 = (value & 0xffff);
        setFlags(aCPU, value);
        IP += 1;
        *handled = 1;
        return;
    }
    b = get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value = *(op1);
    value |= (*(op1 + 1) << 8);
    //  IP++;
    value += *(op2);
    setFlags(aCPU, value);
    *(op1) = (value & 0xffff) & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    *handled = 1;
    IP++;

    return;
}

void add_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 + (value);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
    }

    //  IP += 4;
    return;
}

void add_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value += *(op1);
        setFlags(aCPU, value);
        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;
        return;
    }
}

void add_addr8_reg8(struct emu8086 *aCPU, int *handled)
{

    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;

    if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) + (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) + (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
        {
            value = (*op2 & 0xff) + (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else
        {
            value = (*op2 >> 8) + (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        IP += 1;
        setFlags(aCPU, value);
        *handled = 1;
        return;
    }
    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
    IP++;
    value += *(op1);

    *(op1) = value & 0xff;
    setFlags(aCPU, value);

    *handled = 1;
    return;
}

void add_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) + *(CODE_SEGMENT_IP);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);
    *handled = 1;
    IP++;
}

void add_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value += AX;
    AX = (value & 0xffff);

    setFlags(aCPU, value);
    *handled = 1;
    IP += 1;
}

void add_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    int offset = 0, *op3;
    IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    is_16 = 0;

    int value, *op4;

    unsigned char reg = (opn & 0b00111000) >> 3;
    if (get_ops_reg_8(aCPU, opn, &op3, &op4))
    {
        // opn = opn - 0x20;
        value = 0;
        IP++;
        value = *(CODE_SEGMENT_IP);
        high_reg = high_reg == 1;
        if (opn < 0xc8)
            value += (high_reg ? (*op4 >> 8) : (*op4 & 0xff));
        else if (opn < 0xd0)
            value |= (high_reg ? (*op4 >> 8) : (*op4 & 0xff));
        else if (opn < 0xd8)
        {
            value += (high_reg ? (*op4 >> 8) : (*op4 & 0xff));
            value += (FLAGS & 1);
        }
        else if (opn < 0xe0)
            value = (high_reg ? (*op4 >> 8) : (*op4 & 0xff)) - value - (FLAGS & 1);
        else if (opn < 0xe8)
            value &= (high_reg ? (*op4 >> 8) : (*op4 & 0xff));
        else if (opn < 0xf0)
            value = (high_reg ? (*op4 >> 8) : (*op4 & 0xff)) - value;
        else if (opn < 0xf8)
            value ^= (high_reg ? (*op4 >> 8) : (*op4 & 0xff));
        else
        {
            printf("hr: %d\n", high_reg);
            compare_set_flags(aCPU, (high_reg ? (*op4 >> 8) : (*op4 & 0xff)), value);
            *handled = 1;
            IP++;
            return;
        }
        setFlags(aCPU, value);
        *op4 = high_reg ? ((*op4 & 0xff) | ((value & 0xff) << 8)) : ((*op4 & 0xff00) | (value & 0xff));

        *handled = 1;
        IP++;
        return;
    }
    if (reg == 1)
        return or_addr8_i8(aCPU, handled);
    else if (reg == 2)
        return adc_addr8_i8(aCPU, handled);
    else if (reg == 3)
        return sbb_addr8_i8(aCPU, handled);
    else if (reg == 4)
        return and_addr8_i8(aCPU, handled);
    else if (reg == 5)
        return sub_addr8_i8(aCPU, handled);
    else if (reg == 6)
        return xor_addr8_i8(aCPU, handled);
    else if (reg == 7)
        return cmp_addr8_i8(aCPU, handled);

    if (opn == 0x6)
    {
        // opn
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op4, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }
    IP++;
    value = *(CODE_SEGMENT + IP);
    value = value + *op1;
    *op1 = value & 0xff;
    // *(op1 + 1) = value >> 8;
    IP++;
    setFlags(aCPU, value);
    *handled = 1;
}

void add_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    unsigned int offset = 0;
    IP++;
    unsigned char opn;
    opn = *(CODE_SEGMENT + IP);
    unsigned char *op1 = NULL;
    int *op2, value = 0, *op3;
    if (get_ops_reg_8(aCPU, opn, &op2, &op3))
    {
        IP++;
        value = *(CODE_SEGMENT_IP);
        if (opn < 0xc8)
            value += *op3;
        else if (opn < 0xd0)
            value |= *op3;
        else if (opn < 0xd8)
        {
            value += *op3;
            value += (FLAGS & 1);
        }
        else if (opn < 0xe0)
            value = *op3 - value - (FLAGS & 1);
        else if (opn < 0xe8)
            value &= *op3;
        else if (opn < 0xf0)
            value = *op3 - value;
        else if (opn < 0xf8)
            value ^= *op3;
        else
        {
            compare_set_flags(aCPU, *op3, value);
            *handled = 1;
            IP++;
            return;
        }

        *op3 = value & 0xffff;
        *handled = 1;
        IP++;
        setFlags(aCPU, value);
        return;
    }
    unsigned char reg = (opn & 0b00111000) >> 3;
    if (reg == 1)
        return or_addr16_s8(aCPU, handled);
    else if (reg == 2)
        return adc_addr16_s8(aCPU, handled);
    else if (reg == 3)
        return sbb_addr16_s8(aCPU, handled);
    else if (reg == 4)
        return and_addr16_s8(aCPU, handled);
    else if (reg == 5)
        return sub_addr16_s8(aCPU, handled);
    else if (reg == 6)
        return xor_addr16_s8(aCPU, handled);
    else if (reg == 7)
        return cmp_addr16_s8(aCPU, handled);

    if (opn == 0x6)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;

        // offset = INSTRUCTIONS->value;
    }
    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    value += value2;
    setFlags(aCPU, value);
    *op1++ = value & 255;
    *op1 = (value >> 8) & 255;
    IP++;
    *handled = 1;
}

void add_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    unsigned int offset = 0;
    IP++;
    unsigned char opn;
    opn = *(CODE_SEGMENT + IP);
    unsigned char *op1 = NULL;
    int *op2, value = 0, *op3;
    if (get_ops_reg_8(aCPU, opn, &op2, &op3))
    {

        IP++;
        value = *(CODE_SEGMENT_IP);
        IP++;
        value |= *(CODE_SEGMENT_IP) << 8;
        if (opn < 0xc8)
            value += *op3;
        else if (opn < 0xd0)
            value |= *op3;
        else if (opn < 0xd8)
        {
            value += *op3;
            value += (FLAGS & 1);
        }
        else if (opn < 0xe0)
            value = *op3 - value - (FLAGS & 1);
        else if (opn < 0xe8)
            value &= *op3;
        else if (opn < 0xf0)
            value = *op3 - value;
        else if (opn < 0xf8)
            value ^= *op3;
        else
        {
            compare_set_flags(aCPU, *op3, value);
            *handled = 1;
            IP++;
            return;
        }

        *op3 = value & 0xffff;
        *handled = 1;
        IP++;
        setFlags(aCPU, value);
        return;
    }
    unsigned char reg = (opn & 0b00111000) >> 3;
    if (reg == 1)
        return or_addr16_d16(aCPU, handled);
    else if (reg == 2)
        return adc_addr16_d16(aCPU, handled);
    else if (reg == 3)
        return sbb_addr16_d16(aCPU, handled);
    else if (reg == 4)
        return and_addr16_d16(aCPU, handled);
    else if (reg == 5)
        return sub_addr16_d16(aCPU, handled);
    else if (reg == 6)
        return xor_addr16_d16(aCPU, handled);
    else if (reg == 7)
        return cmp_addr16_d16(aCPU, handled);

    if (opn == 0x6)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;

        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value += value2;
    *op1++ = value & 255;
    *op1 = (value >> 8) & 255;
    IP++;
    setFlags(aCPU, value);
    *handled = 1;
}

// or
void or_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value |= *op2;
        *op2 = (value & 0xffff);

        setFlags(aCPU, value);
        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value |= *(op2);

        *(op1) = (value & 0xffff) & 0xff;
        *(op1 + 1) = (value & 0xffff) >> 8;
        *handled = 1;
        IP++;
        setFlags(aCPU, value);
        return;
    }

    *handled = 0;
    return;
}

void or_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value |= *(op2);

        *(op2) = (value & 0xffff);

        *handled = 1;
        IP++;

        setFlags(aCPU, value);
        return;
    }
    *handled = 0;
    return;
}

void or_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2;
    int opn = *(CODE_SEGMENT + IP);
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value |= *(op1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void or_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    printf("hhh:\n");
    exit(1);
    int b = 0;
    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) | (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) | (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
        {
            value = (*op2 & 0xff) | (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else
        {
            value = (*op2 >> 8) | (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        IP += 1;
        setFlags(aCPU, value);
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value |= *(op1);

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void or_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) | *(CODE_SEGMENT_IP);
    AX = (AX & 0xff00) | (value & 0xff);

    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void or_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value |= AX;
    AX = (value & 0xffff);

    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}
void or_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2, value;
    if (opn == 0xe)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);
        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    // value2 |= *(op1 + 1) << 8;
    value = value | value2;
    setFlags(aCPU, value);

    *op1 = value & 0xff;
    //  *(op1 + 1) = value >> 8;
    IP++;

    *handled = 1;
}

void or_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1 = NULL, opn;
    opn = *(CODE_SEGMENT + IP);
    // offset = opn > (0x79 + 8) ? 0x40 : 0;
    op1 = &opn;
    int *op2, value;
    if (opn == 0xe)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;

    value = *(CODE_SEGMENT_IP) | value2;
    setFlags(aCPU, value);

    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    IP++;

    if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
}

void or_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1 = NULL, opn;
    opn = *(CODE_SEGMENT + IP);
    // offset = opn > (0x79 + 8) ? 0x40 : 0;
    op1 = &opn;
    int *op2, value;
    if (opn == 0xe)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;

    value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;

    value |= value2;
    setFlags(aCPU, value);

    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    IP++;

    if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
}

// adc
void adc_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value += *op2 + (FLAGS & 1);
        *op2 = (value & 0xffff);

        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }
    b = get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value = *(op1);
    value |= (*(op1 + 1) << 8);
    //  IP++;
    value += *(op2) + (FLAGS & 1);

    *(op1) = (value & 0xffff) & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    *handled = 1;
    IP++;

    setFlags(aCPU, value);

    return;
}

void adc_reg16_addr16(struct emu8086 *aCPU, int *handled)
{

    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 + (value) + (FLAGS & 1);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
    }

    //  IP += 4;
    return;
}

void adc_reg8_addr8(struct emu8086 *aCPU, int *handled)
{

    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value += *(op1) + (FLAGS & 1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;
        setFlags(aCPU, value);

        return;
    }
}

void adc_addr8_reg8(struct emu8086 *aCPU, int *handled)
{

    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;

    if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) + (*op3 & 0xff) + (FLAGS & 1);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) + (*op3 & 0xff) + (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 & 0xff) + (*op3 >> 8) + (FLAGS & 1);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else
        {
            value = (*op2 >> 8) + (*op3 >> 8) + (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        IP += 1;
        *handled = 1;
        setFlags(aCPU, value);

        return;
    }
    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
    IP++;
    value += *(op1) + (FLAGS & 1);

    *(op1) = value & 0xff;

    setFlags(aCPU, value);

    *handled = 1;
    return;
}

void adc_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) + *(CODE_SEGMENT_IP) + (FLAGS & 1);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void adc_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value += AX + (FLAGS & 1);
    AX = (value & 0xffff);

    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}

void adc_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    int offset = 0;

    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    is_16 = 0;

    int value, *op4;
    if (opn == 0x16)
    {
        // opn
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op4, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    value = value + *op1 + (FLAGS & 1);
    *op1 = value & 0xff;
    // *(op1 + 1) = value >> 8;
    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

void adc_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    unsigned int offset = 0;

    unsigned char opn;

    opn = *(CODE_SEGMENT + IP);

    unsigned char *op1 = NULL;
    int *op2, value = 0;
    if (opn == 0x16)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;

        // offset = INSTRUCTIONS->value;
    }
    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);

    value += value2 + (FLAGS & 1);
    *op1++ = value & 255;
    *op1 = (value >> 8) & 255;
    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

void adc_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    unsigned int offset = 0;

    unsigned char opn;

    opn = *(CODE_SEGMENT + IP);

    unsigned char *op1 = NULL;
    int *op2, value = 0;
    if (opn == 0x16)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;

        // offset = INSTRUCTIONS->value;
    }
    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value += value2 + (FLAGS & 1);
    *op1++ = value & 255;
    *op1 = (value >> 8) & 255;
    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

// sbb
void sbb_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value = *op2 - value - (FLAGS & 1);
        *op2 = (value & 0xffff);

        setFlags(aCPU, value);
        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value -= *(op2) - (FLAGS & 1);

        *(op1) = (value & 0xffff) & 0xff;
        *(op1 + 1) = (value & 0xffff) >> 8;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sbb_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 - (value) - (FLAGS & 1);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
        //  IP += 4;
        return;
    }

    *handled = 0;
    return;
}

void sbb_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2; //, *op3;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value -= *(op1) - (FLAGS & 1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sbb_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) - (*op3 & 0xff) - (FLAGS & 1);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) - (*op3 >> 8) - (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 & 0xff) - (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op2 & 0xff) - (*op3 >> 8) - (FLAGS & 1);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        high_reg = 0;
        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value = *(op1)-value - (FLAGS & 1);

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sbb_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) - *(CODE_SEGMENT_IP) - (FLAGS & 1);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void sbb_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = AX - value - (FLAGS & 1);
    AX = (value & 0xffff);

    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}

void sbb_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);
    offset = opn > (0x9f + 8) ? 0x40 : 0;

    int *op2 = NULL, value;
    if (opn == 0x1e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);
        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    //  value2 |= *(op1 + 1) << 8;
    value = value - value2 - (FLAGS & 1);
    *op1 = value & 0xff;
    //  *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

void sbb_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x1e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = value2 - *(CODE_SEGMENT_IP) - (FLAGS & 1);
    *op1 = value & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;
    setFlags(aCPU, value);
    *handled = 1;
}

void sbb_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x1e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = value2 - value - (FLAGS & 1);
    *op1 = value & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

// jmp
void jmp_8(struct emu8086 *aCPU, int *handled)
{

    IP++;

    find_instruction(aCPU);

    aCPU->skip_next = 1;
    *handled = 1;
}
void jmp_16(struct emu8086 *aCPU, int *handled)
{

    IP++;

    find_instruction16(aCPU);

    aCPU->skip_next = 1;
    *handled = 1;
}
void jcxz_8(struct emu8086 *aCPU, int *handled)
{

    if (CX == 0)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jc_8(struct emu8086 *aCPU, int *handled)
{
    int CF = GET_FLAG(0);
    if (CF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jnc_8(struct emu8086 *aCPU, int *handled)
{
    int CF = GET_FLAG(0);
    if (!CF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}
void jo_8(struct emu8086 *aCPU, int *handled)
{
    int OF = GET_FLAG(11);
    if (OF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jno_8(struct emu8086 *aCPU, int *handled)
{
    int OF = GET_FLAG(11);
    if (!OF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}
void js_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);
    if (SF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jns_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);
    if (!SF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jz_8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);
    if (ZF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jnz_8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);
    if (!ZF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void ja_8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);

    int CF = GET_FLAG(0);

    if (!(CF | ZF))
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jbe_8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);

    int CF = GET_FLAG(0);

    if ((CF && ZF))
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jl_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);

    int OF = GET_FLAG(11);

    if (!(SF == OF))
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jle_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);
    int ZF = GET_FLAG(6);
    int OF = GET_FLAG(11);

    if (!(SF == OF) || ZF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jge_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);

    int OF = GET_FLAG(11);

    if ((SF == OF))
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jg_8(struct emu8086 *aCPU, int *handled)
{
    int SF = GET_FLAG(7);
    int ZF = GET_FLAG(6);

    int OF = GET_FLAG(11);

    if ((SF == OF) && !ZF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jpe_8(struct emu8086 *aCPU, int *handled)
{
    int PF = GET_FLAG(2);
    if (PF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

void jpo_8(struct emu8086 *aCPU, int *handled)
{
    int PF = GET_FLAG(2);
    if (!PF)
    {
        jmp_8(aCPU, handled);
        return;
    }
    IP += 2;
    *handled = 1;
}

// loop

void loop_a8(struct emu8086 *aCPU, int *handled)
{

    IP++;
    if (CX > 0)
        CX--;

    if (CX < 1)
    {
        *handled = 1;
        aCPU->instruction_cache_loop = NULL;
        IP++;
        return;
    }
    if (aCPU->instruction_cache_loop == NULL)
    {
        find_instruction(aCPU);
        aCPU->instruction_cache_loop = _INSTRUCTIONS;
    }
    else
    {
        IP = aCPU->instruction_cache_loop->starting_address;
        _INSTRUCTIONS = aCPU->instruction_cache_loop;
    }

    aCPU->skip_next = 1;
    *handled = 1;
}

void loopz_a8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);
    if (ZF)
    {
        loop_a8(aCPU, handled);
        return;
    }
    IP += 1;
    *handled = 1;
}

void loopnz_a8(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);
    if (!ZF)
    {
        loop_a8(aCPU, handled);
        return;
    }
    IP += 1;
    *handled = 1;
}

// and

void and_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value &= *op2;
        *op2 = (value & 0xffff);

        setFlags(aCPU, value);
        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value &= *(op2);

        *(op1) = (value & 0xffff) & 0xff;
        *(op1 + 1) = (value & 0xffff) >> 8;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void and_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 & (value);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
        //  IP += 4;
        return;
    }

    *handled = 0;
    return;
}

void and_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2; //, *op3;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value &= *(op1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void and_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) & (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) & (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) & (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op2 & 0xff) & (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        high_reg = 0;
        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value &= *(op1);

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void and_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) & *(CODE_SEGMENT_IP);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void and_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value &= AX;
    AX = (value & 0xffff);
    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}

void and_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2, value;
    if (opn == 0x26)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);
        op1 += offset;
        // offset = INSTRUCTIONS->value;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    // value2 |= *(op1 + 1) << 8;
    value = value & value2;
    *op1 = value & 0xff;
    //*(op1 + 1) = value >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

void and_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x26)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT_IP)&value2;
    setFlags(aCPU, value);

    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    IP++;

    if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
}

void and_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x26)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value &= value2;
    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    setFlags(aCPU, value);

    IP++;

    if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
}

// sub
void sub_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value = *op2 - value;
        *op2 = (value & 0xffff);

        setFlags(aCPU, value);
        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value -= *(op2);

        *(op1) = (value & 0xffff) & 0xff;
        *(op1 + 1) = (value & 0xffff) >> 8;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sub_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 - (value);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
        //  IP += 4;
        return;
    }

    *handled = 0;
    return;
}

void sub_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2; // *op3;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value -= *(op1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sub_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) - (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) - (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op2 & 0xff) - (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        high_reg = 0;
        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value = *(op1)-value;

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void sub_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) - *(CODE_SEGMENT_IP);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void sub_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = AX - value;
    AX = (value & 0xffff);

    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}

void sub_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);
    offset = opn > (0x9f + 8) ? 0x40 : 0;

    int *op2 = NULL, value;
    if (opn == 0x2e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);
        op1 += offset;
        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    //  value2 |= *(op1 + 1) << 8;
    value = value - value2;
    *op1 = value & 0xff;
    //  *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

void sub_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x2e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = value2 - *(CODE_SEGMENT_IP);
    *op1 = value & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;
    setFlags(aCPU, value);
    *handled = 1;
}

void sub_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x2e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
        // offset = INSTRUCTIONS->value;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = value2 - value;
    *op1 = value & 0xff;
    *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

// xor
void xor_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value = *op2 ^ value;
        *op2 = (value & 0xffff);
        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value ^= *(op2);

        *(op1) = (value & 0xffff) & 0xff;
        *(op1 + 1) = (value & 0xffff) >> 8;
        *handled = 1;
        IP++;
        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void xor_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 ^ (value);
        *op2 = value & 0xffff;
        setFlags(aCPU, value);

        *handled = 1;
        IP++;
        //  IP += 4;
        return;
    }

    *handled = 0;
    return;
}

void xor_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    // int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2; //;, *op3;
    int opn = *(CODE_SEGMENT + IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value ^= *(op1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void xor_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) ^ (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) ^ (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) ^ (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op2 & 0xff) ^ (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        high_reg = 0;
        if (value > 0xff)
            setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value = *(op1) ^ value;

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void xor_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) ^ *(CODE_SEGMENT_IP);
    AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void xor_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = AX ^ value;
    AX = (value & 0xffff);

    setFlags(aCPU, value);
    *handled = 1;
    IP += 1;
}

void xor_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2, value;
    if (opn == 0x36)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);
        op1 += offset;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    // value2 |= *(op1 + 1) << 8;
    value = value ^ value2;
    *op1 = value & 0xff;
    //*(op1 + 1) = value >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

void xor_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x36)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT_IP) ^ value2;
    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

void xor_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    int *op2 = NULL, value;
    op1 = &opn;
    if (opn == 0x36)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        op1 += offset;

        // IP++;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value ^= value2;
    *op1 = value & 0xff;
    *(op1 + 1) = value >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

// mov

void mov_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int b = 0, *op2;
    unsigned char *op1 = NULL, opn = *(CODE_SEGMENT_IP);
    b = get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    if (b)
    {
        *handled = 1;
        IP++;
        *op1 = *(CODE_SEGMENT_IP);

        IP += 1;
    }
}
void mov_addr16_i16(struct emu8086 *aCPU, int *handled)
{

    IP++;

    unsigned char *op1 = NULL, opn;
    int *op2 = NULL;
    opn = *(CODE_SEGMENT_IP);

    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    IP++;
    *op1++ = *(CODE_SEGMENT_IP);
    IP++;
    *op1 = *(CODE_SEGMENT_IP);
    IP++;
    *handled = 1;
}

void mov_addr16_r16(struct emu8086 *aCPU, int *handled)
{
    unsigned char *op1 = DATA_SEGMENT;
    int *op2 = NULL, *op3 = NULL;
    is_16 = 1;
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP);

    int b = get_ops_reg_8(aCPU, opn, &op3, &op2);

    if (b)

    {
        int value = (*op3);
        *op2 = value;
        if (value > 0xffff)
        {
            FLAGS |= 1;
        }
        else if (value == 0)
        {
            FLAGS |= 64;
        }
        *handled = 1;
        IP += 1;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *op2;
        *(op1) = *op2 & 0xff;
        *(op1 + 1) = *op2 >> 8;
        if (value > 0xffff)
        {
            FLAGS |= 1;
        }
        else if (value == 0)
        {
            FLAGS |= 64;
        }
        *handled = 1;
        IP += 1;
    }
}

void mov_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    IP++;
    unsigned char *op1;
    int *op2, *op3, opn = *(CODE_SEGMENT_IP);
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);

    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }

        if (value == 0)
        {
            FLAGS |= 64;
        }
        IP += 1;
        *handled = 1;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = *(op1);

        *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        if (value > 0xff)
        {
            FLAGS |= 8;
        }

        else if (value == 0)
        {
            FLAGS |= 64;
        }
        return;
    }

    *handled = 0;
    return;
}

void mov_reg8_i8(struct emu8086 *aCPU, int *handled)
{
    int op = *(CODE_SEGMENT + IP);
    IP++;
    int reg_offset = op - 176;
    unsigned char value = *(CODE_SEGMENT + IP);
    IP++;
    int *op1 = aCPU->mSFR + (reg_offset < 4 ? reg_offset : reg_offset - 4);
    if (reg_offset < 4)
        *op1 = (*op1 & 0xff00) | (value);
    else
        *op1 = (*op1 & 0xff) | ((value) << 8);

    *handled = 1;
}

void mov_reg16_i16(struct emu8086 *aCPU, int *handled)
{

    // IP++;
    int reg_offset = *(CODE_SEGMENT_IP)-184;
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    int *op1 = aCPU->mSFR + reg_offset;

    *op1 = (value & 0xffff);
    IP += 1;

    *handled = 1;
}

void mov_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    unsigned char *op1;
    IP++;
    int *op2, *op3, opn = *(CODE_SEGMENT_IP);
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 1)
        {
            value = (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }

        if (value > 0xff)
        {
            FLAGS |= 8;
        }

        else if (value == 0)
        {
            FLAGS |= 64;
        }
        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);

        *(op1) = value & 0xff;

        *handled = 1;
        IP++;

        if (value == 0)
        {
            FLAGS |= 64;
        }
        return;
    }

    *handled = 0;
    return;
}

void mov_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 0;
    unsigned char *op1;
    op1 = DATA_SEGMENT;
    int *op2 = NULL;
    IP++;
    int opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        int value = *op1++;
        value |= (*op1 << 8);

        *op2 = (value & 0xffff);
        if (value > 0xffff)
        {
            FLAGS |= 1;
        }
        else if (value == 0)
        {
            FLAGS |= 64;
        }
        //((*op2 & 0xff00) | *(op1 + INSTRUCTIONS->value)) | ((*op2 & 0xff) | *(op1 + INSTRUCTIONS->value + 1))) : ((*op2 & 0xff00) | *(op1 + INSTRUCTIONS->value));
        *handled = 1;
        IP += 1;
        return;
    }

    *handled = 0;
    return;
}

void mov_al_addr(struct emu8086 *aCPU, int *handled)
{

    int width = 2;
    IP++;

    int offset = *(CODE_SEGMENT_IP);
    if (width > 1)
    {
        IP++;
        offset |= *(CODE_SEGMENT_IP) << 8;
    }
    unsigned int value = *(DATA_SEGMENT + offset);
    AX = (AX & 0xff00) | (value & 0xff);
    if (value > 0xff)
    {
        FLAGS |= 8;
    }

    else if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
    IP += 1;
}

void mov_ax_addr(struct emu8086 *aCPU, int *handled)
{
    int width = 2;
    IP++;
    int offset = *(CODE_SEGMENT_IP);
    if (width > 1)
    {
        IP++;
        offset |= *(CODE_SEGMENT_IP) << 8;
    }
    int value = *(DATA_SEGMENT + offset++);
    value |= (*(DATA_SEGMENT + offset) << 8);
    AX = (value & 0xffff);
    if (value > 0xffff)
    {
        FLAGS |= 8;
    }

    else if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
    IP += 1;
}

void mov_addr_al(struct emu8086 *aCPU, int *handled)
{

    int width = 2;
    IP++;

    int offset = *(CODE_SEGMENT_IP);
    if (width > 1)
    {
        IP++;
        offset |= *(CODE_SEGMENT_IP) << 8;
    }
    unsigned char *mem_loc = (DATA_SEGMENT + offset);
    int value = (AX & 0xff);
    *mem_loc = value;
    if (value > 0xff)
    {
        FLAGS |= 8;
    }

    else if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
    IP += 1;
}

void mov_addr_ax(struct emu8086 *aCPU, int *handled)
{
    int width = 2;
    IP++;

    int offset = *(CODE_SEGMENT_IP);
    if (width > 1)
    {
        IP++;
        offset |= *(CODE_SEGMENT_IP) << 8;
    }
    unsigned char *mem_loc = (DATA_SEGMENT + offset); // int value = AX;
    int value = AX;
    *mem_loc++ = value & 0xff;
    *mem_loc = value >> 8;
    if (value > 0xffff)
    {
        FLAGS |= 8;
    }

    else if (value == 0)
    {
        FLAGS |= 64;
    }
    *handled = 1;
    IP += 1;
}

void mov_addr_cs(struct emu8086 *aCPU, int *handled)
{
    IP++;
    is_16 = 1;
    unsigned int offset = 0;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int *op2 = NULL, *op3 = NULL;

    unsigned char reg = (opn & 0b00111000) >> 3;
    if (reg == 0)
        return mov_addr_es(aCPU, handled);
    else if (reg == 3)
        return mov_addr_ds(aCPU, handled);
    else if (reg == 2)
        return mov_addr_ss(aCPU, handled);
    if (opn == 0xe)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        *op2 = CS;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }

    *op1++ = CS & 0xff;
    *op1 = CS >> 8;
    *handled = 1;
    IP++;
}

void mov_addr_ds(struct emu8086 *aCPU, int *handled)
{
    //  IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int *op2 = NULL, *op3 = NULL;
    if (opn == 0x1e)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        *op2 = DS;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }

    *op1++ = DS & 0xff;
    *op1 = DS >> 8;

    *handled = 1;
    IP += 1;
}

void mov_addr_es(struct emu8086 *aCPU, int *handled)
{
    // IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int *op2 = NULL, *op3 = NULL;
    if (opn == 0x6)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        *op2 = ES;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }

    *op1++ = ES & 0xff;
    *op1 = ES >> 8;

    *handled = 1;
    IP += 1;
}

void mov_addr_ss(struct emu8086 *aCPU, int *handled)
{
    //IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int *op2 = NULL, *op3 = NULL;
    if (opn == 0x16)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        *op2 = _SS;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }

    *op1++ = _SS & 0xff;
    *op1 = _SS >> 8;

    *handled = 1;
    IP += 1;
}

void mov_cs_addr(struct emu8086 *aCPU, int *handled)
{
    IP++;
    is_16 = 1;
    unsigned int offset = 0;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int value = 0, *op2 = NULL, *op3 = NULL;

    unsigned char reg = (opn & 0b00111000) >> 3;
    if (reg == 0)
        return mov_es_addr(aCPU, handled);
    else if (reg == 3)
        return mov_ds_addr(aCPU, handled);
    else if (reg == 2)
        return mov_ss_addr(aCPU, handled);
    if (opn == 0xe)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        CS = *op2;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }
    value = *op1++;
    value |= *(op1) << 8;
    CS = value;
    *handled = 1;
    IP++;
}

void mov_ds_addr(struct emu8086 *aCPU, int *handled)
{
    //  IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int value = 0, *op2 = NULL, *op3 = NULL;
    if (opn == 0x1e)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        DS = *op2;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }
    value = *op1++;
    value |= *(op1) << 8;
    DS = value;

    *handled = 1;
    IP += 1;
}

void mov_es_addr(struct emu8086 *aCPU, int *handled)
{
    // IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int value = 0, *op2 = NULL, *op3 = NULL;
    if (opn == 0x6)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        ES = *op2;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }

    value = *op1++;
    value |= *(op1) << 8;
    ES = value;

    *handled = 1;
    IP += 1;
}

void mov_ss_addr(struct emu8086 *aCPU, int *handled)
{
    //IP++;
    unsigned int offset = 0;
    is_16 = 1;
    unsigned char *op1, opn = *(CODE_SEGMENT_IP);
    int value = 0, *op2 = NULL, *op3 = NULL;
    if (opn == 0x16)
    {
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        offset = *(CODE_SEGMENT_IP);
        if (width > 1)
        {
            IP++;
            offset |= *(CODE_SEGMENT_IP) << 8;
        }
        op1 = DATA_SEGMENT + offset;
    }
    else if (get_ops_reg_8(aCPU, opn, &op3, &op2))
    {
        _SS = *op2;
        *handled = 1;
        IP++;
        return;
    }
    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }
    value = *op1++;
    value |= *(op1) << 8;
    _SS = value;
    *handled = 1;
    IP += 1;
}

// push
void push_reg(struct emu8086 *aCPU, int *handled)
{
    unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1;
    switch (opcode)
    {
    case PUSH_CS:
        op1 = SFRS + REG_CS;
        break;
    case PUSH_SS:
        op1 = SFRS + REG_SS;
        break;
    case PUSH_DS:
        op1 = SFRS + REG_DS;
        break;
    case PUSH_ES:
        op1 = SFRS + REG_ES;
        break;
    default:
        *handled = 0;
        return;
    }
    *handled = 1;
    push_to_stack(aCPU, *op1);
    IP += 1;
}

void push_reg16(struct emu8086 *aCPU, int *handled)
{
    unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1;
    int reg_offset = opcode - 80;
    op1 = SFRS + reg_offset;

    *handled = 1;
    push_to_stack(aCPU, *op1);
    IP += 1;
}
void push_flags(struct emu8086 *aCPU, int *handled)
{
    //  unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1;
    // int reg_offset = opcode - 80;
    op1 = SFRS + REG_FLAGS;

    *handled = 1;
    push_to_stack(aCPU, *op1);
    IP += 1;
}

void push_addr(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP);
    unsigned char *op1;
    int value = 0, *op2 = NULL;
    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    value = *op1++;
    value |= *op1 << 8;
    push_to_stack(aCPU, value);
    IP += 1;
    *handled = 1;
}
// pop
void pop_reg(struct emu8086 *aCPU, int *handled)
{
    unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1, value = 0;
    switch (opcode)
    {

    case POP_SS:
        op1 = SFRS + REG_SS;
        break;
    case POP_DS:
        op1 = SFRS + REG_DS;
        break;
    case POP_ES:
        op1 = SFRS + REG_ES;
        break;
    default:
        *handled = 0;
        return;
    }
    *handled = 1;
    pop_from_stack(aCPU, &value);
    *op1 = value;
    //  SP -= 2;

    IP += 1;
}
void pop_reg16(struct emu8086 *aCPU, int *handled)
{
    unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1, value = 0;
    int reg_offset = opcode - 88;
    op1 = SFRS + reg_offset;

    *handled = 1;

    pop_from_stack(aCPU, &value);

    *op1 = value;

    IP += 1;
}

void pop_flags(struct emu8086 *aCPU, int *handled)
{
    //  unsigned char opcode = *(CODE_SEGMENT_IP);
    int *op1, value = 0;

    op1 = SFRS + REG_FLAGS;

    *handled = 1;

    pop_from_stack(aCPU, &value);

    *op1 = value;

    IP += 1;
}
void pop_addr(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP);
    unsigned char *op1;
    int value = 0, *op2 = NULL;
    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    pop_from_stack(aCPU, &value);

    *op1++ = value & 0xff;
    *op1 = value >> 8;

    //SP -= 2;
    *handled = 1;
    IP += 1;
}

// inc

void inc_reg16(struct emu8086 *aCPU, int *handled)
{
    int reg_offset = *(CODE_SEGMENT_IP)-64;
    int *op1 = SFRS + reg_offset;
    unsigned int value = 0;
    value = *op1 + 1;
    *op1 = 0xffff & value;
    setFlags(aCPU, value);
    IP += 1;
    *handled = 1;
}

void inc_addr8(struct emu8086 *aCPU, int *handled)
{
    // me
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP), value;
    is_16 = 0;
    int *op1, *op2;
    if (opn < 0xc8)
    {
        if (get_ops_reg_8(aCPU, opn, &op2, &op1))
        {

            value = 0;
            if (high_reg == 0)
            {
                value = (*op1 & 0xff) + 1;

                *op1 = (*op1 & 0xff00) | (value & 0xff);
            }

            else
            {
                value = (*op1 >> 8) + 1;
                *op1 = (*op1 & 0x00ff) | (value & 0xff) << 8;
            }
            IP++;
            setFlags(aCPU, value);

            high_reg = 0;
            *handled = 1;
        }
    }
    else
    {
        dec_addr8(aCPU, handled);
        return;
    }
}

// dec
void dec_addr8(struct emu8086 *aCPU, int *handled)
{
    // me
    // IP++;
    unsigned char opn = *(CODE_SEGMENT_IP), value;
    is_16 = 0;
    int *op1, *op2;

    if (get_ops_reg_8(aCPU, opn, &op2, &op1))
    {

        value = 0;
        if (high_reg == 0)
        {
            value = (*op1 & 0xff) - 1;

            *op1 = (*op1 & 0xff00) | (value & 0xff);
        }

        else
        {
            value = (*op1 >> 8) - 1;
            *op1 = (*op1 & 0x00ff) | (value & 0xff) << 8;
        }
        IP++;
        *handled = 1;
        setFlags(aCPU, value);

        high_reg = 0;
    }
    else
    {
        return;
    }
}

void dec_reg16(struct emu8086 *aCPU, int *handled)
{
    int reg_offset = *(CODE_SEGMENT_IP)-72;
    int *op1 = SFRS + reg_offset;
    int value = 0;
    value = *op1 - 1;

    *op1 = 0xffff & value;
    setFlags(aCPU, value);

    IP += 1;
    *handled = 1;
}

// call
void call_addr(struct emu8086 *aCPU, int *handled)
{
    if (aCPU->call_stack > (MAX_CALL_STACK - 1))
    {
        char buf[256];
        sprintf(buf, errors_str[MAX_CALL], _INSTRUCTIONS->line_number);
        massage(buf, ERR);
        // massage("MAX CALL EXCEEDED\n", ERR);
        *handled = 1;
        IP = aCPU->end_address;
        aCPU->skip_next = 1;
        while (_INSTRUCTIONS->next != NULL)
            _INSTRUCTIONS = _INSTRUCTIONS->next;
        return;
    }
    aCPU->instruction_cache[aCPU->call_stack] = _INSTRUCTIONS->next;
    aCPU->call_stack++;
    find_instruction_call(aCPU);
    *handled = 1;
    aCPU->skip_next = 1;
    return;
}

void ret_addr(struct emu8086 *aCPU, int *handled)
{
    int value;
    if (aCPU->call_stack == 0)
    {
        char buf[256];
        sprintf(buf, errors_str[INVALID_RET], _INSTRUCTIONS->line_number);
        massage(buf, ERR);
        *handled = 1;
        IP = aCPU->end_address;
        aCPU->skip_next = 1;
        return;
    }
    pop_from_stack(aCPU, &value);
    aCPU->call_stack--;
    _INSTRUCTIONS = aCPU->instruction_cache[aCPU->call_stack];

    // aCPU->instruction_cache[aCPU->call_stack] = NULL;
    *handled = 1;
    IP = value;
    aCPU->skip_next = 1;
    return;
}

// cmp
void cmp_addr16_reg16(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 1;

    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value = *op3;
        value = *op2 - value;
        // *op2 = (value & 0xffff);

        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        int value = *(op1);
        value |= (*(op1 + 1) << 8);
        //  IP++;
        value -= *(op2);

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void cmp_reg16_addr16(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2, opn = *(CODE_SEGMENT_IP);

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        // IP++;

        int value = *(op1);
        value |= (*(op1 + 1) << 8);

        value = *op2 - (value);

        setFlags(aCPU, value);

        *handled = 1;
        IP++;
        //  IP += 4;
        return;
    }

    *handled = 0;
    return;
}

void cmp_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2;
    int opn = *(CODE_SEGMENT + IP);
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        value = value - *(op1);

        //  *op2 = high_reg ? (*op2 & 0xff) | ((value & 0xff) << 8) : ((*op2 & 0xff00) | (value & 0xff));
        high_reg = 0;
        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void cmp_addr8_reg8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;
    // int _value = INSTRUCTIONS->value;
    IP++;
    unsigned char *op1;
    int opn = *(CODE_SEGMENT + IP);

    int *op2, *op3;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value;
        if (high_reg == 0)
        {
            value = (*op2 & 0xff) - (*op3 & 0xff);
        }
        else if (high_reg == 3)
        {
            value = (*op2 >> 8) - (*op3 >> 8);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 & 0xff);
        }
        else
        {
            value = (*op2 & 0xff) - (*op3 >> 8);
        }
        // printf("p5: %d\n", *oop2);

        high_reg = 0;
        setFlags(aCPU, value);

        IP += 1;
        *handled = 1;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))

    {
        int value = high_reg ? ((*op2 & 0xff00) >> 8) : (*op2 & 0xff);
        //  IP++;
        value = *(op1)-value;

        *handled = 1;
        IP++;

        setFlags(aCPU, value);

        return;
    }

    *handled = 0;
    return;
}

void cmp_al_i8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = (AX & 0xff) - *(CODE_SEGMENT_IP);
    // AX = (AX & 0xff00) | (value & 0xff);
    setFlags(aCPU, value);

    *handled = 1;
    IP++;
}

void cmp_ax_i16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    int value = *(CODE_SEGMENT_IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value = AX - value;
    AX = (value & 0xffff);
    setFlags(aCPU, value);

    *handled = 1;
    IP += 1;
}

void cmp_addr8_i8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);
    offset = opn > (0x9f + 16) ? 0x40 : 0;

    int *op2 = NULL, value;
    if (opn == 0x3e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;
        IP++;
        value = *(CODE_SEGMENT + IP);

        op1 += offset;
    }

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {
        IP++;
        value = *(CODE_SEGMENT + IP);
    }

    int value2;
    value2 = *op1;
    // value2 |= *(op1 + 1) << 8;
    value = value2 - value;

    //  *(op1 + 1) = (value & 0xffff) >> 8;
    IP++;

    setFlags(aCPU, value);

    *handled = 1;
}

void cmp_addr16_s8(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);
    op1 = &opn;
    int *op2 = NULL, value;
    if (opn == 0x3e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = value2 - *(CODE_SEGMENT_IP);
    setFlags(aCPU, value);

    IP++;

    *handled = 1;
}

void cmp_addr16_d16(struct emu8086 *aCPU, int *handled)
{
    unsigned int offset = 0;
    // IP++;
    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);
    op1 = &opn;
    int *op2 = NULL, value;
    if (opn == 0x3e)
    {
        IP++;
        op1 = DATA_SEGMENT;
        offset = *(CODE_SEGMENT + IP);
        IP++;
        offset |= *(CODE_SEGMENT + IP) << 8;

        op1 += offset;
    }

    else
        get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value2;
    value2 = *op1;
    value2 |= *(op1 + 1) << 8;
    IP++;
    value = *(CODE_SEGMENT + IP);
    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;
    value -= value2;

    IP++;
    setFlags(aCPU, value);

    *handled = 1;
}

// int_

void int_(struct emu8086 *aCPU, int *handled)
{

    IP += 2;
    *handled = 1;
}

// nop

void nop(struct emu8086 *aCPU, int *handled)
{

    IP += 1;
    *handled = 1;
}

// xchg

void xchg_ax_r16(struct emu8086 *aCPU, int *handled)
{
    // IP++;
    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    unsigned char reg = opn & 0b111;
    op2 = SFRS + reg;
    value = AX;
    AX = *op2;
    *op2 = value;
    *handled = 1;
    IP++;
}
void xchg_r16_d16(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP), *op3;
    int *op1, *op2, b;
    is_16 = 1;
    b = get_ops_reg_8(aCPU, opn, &op1, &op2);
    if (b)
    {
        int value = *op1, value2 = *op2;

        *op2 = value;
        *op1 = value2;

        IP += 1;
        *handled = 1;
        is_16 = 0;
        return;
    }

    if (get_ops_reg_8_addr(aCPU, opn, &op1, &op3))
    {
        int value = *(op1);
        int value2 = *op3;
        value2 |= *(op3 + 1) << 8;
        *op1 = value2;
        *op3++ = value & 0xff;
        *op3 = value >> 8;
        IP += 1;
        *handled = 1;
        is_16 = 0;
    }
}

void xchg_r8_d8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char *op1 = NULL, opn = *(CODE_SEGMENT_IP);
    int *op2, b;
    int *op3;
    is_16 = 0;
    b = get_ops_reg_8(aCPU, opn, &op3, &op2);
    if (b)
    {
        int value, value2;
        printf("%d \n", high_reg);
        if (high_reg == 0 && (op3 != op2))
        {

            value = *op3 & 0xff;
            value2 = *op2 & 0xff;

            *op3 = (*op3 & 0xff00) | value2;
            *op2 = (*op2 & 0xff00) | value;
        }
        else if (high_reg == 3 && (op3 != op2))
        {
            value = *op3 >> 8;
            value2 = *op2 >> 8;

            *op3 = (*op3 & 0xff) | (value2 << 8);
            *op2 = (*op2 & 0xff) | (value << 8);
        }

        else
        {
            value2 = *op2 >> 8;
            value = *op3 & 0xff;

            if (op3 == op2 || high_reg == 1)
            {
                *op2 = (*op2 & 0xff) | (value << 8);
                *op3 = (*op3 & 0xff00) | (value2);
            }
            else
            {
                value2 = *op2 & 0xff;
                value = *op3 >> 8;

                *op2 = (*op2 & 0xff00) | (value);
                *op3 = (*op3 & 0xff) | (value2 << 8);
            }
        }

        IP += 1;
        *handled = 1;
        is_16 = 0;
        return;
    }
    opn = opn > 0x79 ? opn - 0x40 : opn;
    high_reg = 0;
    if (get_ops_reg_8_addr(aCPU, opn, &op3, &op1))
    {
        unsigned int value = *op1;
        //  IP++;
        //  value += *(op1);
        int value2 = *op3;
        printf("%d %x %x\n", high_reg, value2 & 0xff, value2 >> 8);
        *op1 = high_reg ? value2 >> 8 : value2 & 0xff;
        // *(op1) = value & 0xff;
        *op3 = high_reg ? (*op3 & 0xff) | (value << 8) : (*op3 & 0xff00) | value;
        *handled = 1;
        IP++;
    }
    high_reg = 0;
}

// aaa
void aaa(struct emu8086 *aCPU, int *handled)
{
    unsigned char value_al = AX & 0xff,
                  value_ah = AX >> 8;

    unsigned char low_nibble = value_al & 0xf;
    if (low_nibble > 9 || value_ah == 1)
    {
        value_al = value_al + 6;
        value_al &= 0xf;
        value_ah++;
        AX = value_al;
        int v = value_ah << 8;
        AX |= v;
    }
    IP++;
    *handled = 1;
}

// aad
void aad(struct emu8086 *aCPU, int *handled)
{
    unsigned char value_al = AX & 0xff,
                  value_ah = AX >> 8;

    value_al = (value_ah * 10) + value_al;
    AX = value_al;
    IP += 2;
    *handled = 1;
}

// aam
void aam(struct emu8086 *aCPU, int *handled)
{
    unsigned char value_al = AX & 0xff,
                  value_ah = AX >> 8;

    value_ah = value_al / 10;
    value_al = value_al % 10;
    AX = value_al;
    int v = value_ah << 8;
    AX |= v;
    IP += 2;
    *handled = 1;
}

// aas
void aas(struct emu8086 *aCPU, int *handled)
{
    unsigned char value_al = AX & 0xff,
                  value_ah = AX >> 8;

    unsigned char low_nibble = value_al & 0xf;
    if (low_nibble > 9 || value_ah == 1)
    {
        value_al = value_al - 6;
        value_al &= 0xf;
        value_ah -= 1;
        AX = value_al;
        int v = value_ah << 8;
        AX |= v;
    }
    IP++;
    *handled = 1;
}

// cmpsb
void cmpsb(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int value = *(DATA_SEGMENT + SI) - *(EXTRA_SEGMENT + DI);
    setFlags(aCPU, value);
    int incr = DF ? -1 : 1;
    DI += incr;
    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// cmpsw
void cmpsw(struct emu8086 *aCPU, int *handled)
{
    int v1, v2;
    int DF = GET_FLAG(10);
    int incr = DF ? -1 : 1;

    v1 = *(DATA_SEGMENT + SI);
    SI += incr;
    v1 |= *(DATA_SEGMENT + SI) << 8;
    v2 = *(EXTRA_SEGMENT + DI);
    DI += incr;
    v2 |= *(EXTRA_SEGMENT + DI) << 8;
    DI += incr;
    SI += incr;
    int value = v1 - v2;
    setFlags(aCPU, value);

    IP++;
    *handled = 1;
    return;
}

//
void lodsb(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);
    int incr = DF ? -1 : 1;

    AX = (AX & 0xff00) | *(DATA_SEGMENT + SI);

    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// lodsw
void lodsw(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int incr = DF ? -1 : 1;

    AX = *(DATA_SEGMENT + SI);
    SI += incr;
    AX |= *(DATA_SEGMENT + SI) << 8;
    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// movsb
void movsb(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);
    int incr = DF ? -1 : 1;

    *(EXTRA_SEGMENT + DI) = *(DATA_SEGMENT + SI);
    DI += incr;
    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// movsw
void movsw(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int incr = DF ? -1 : 1;

    *(EXTRA_SEGMENT + DI) = *(DATA_SEGMENT + SI);
    DI += incr;
    SI += incr;
    *(EXTRA_SEGMENT + DI) = *(DATA_SEGMENT + SI);
    DI += incr;
    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// scasb
void scasb(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int value = (AX & 0xff) - *(EXTRA_SEGMENT + DI);
    setFlags(aCPU, value);
    int incr = DF ? -1 : 1;
    DI += incr;

    IP++;
    *handled = 1;
    return;
}

// scasw
void scasw(struct emu8086 *aCPU, int *handled)
{
    int v2;
    int DF = GET_FLAG(10);
    int incr = DF ? -1 : 1;

    v2 = *(EXTRA_SEGMENT + DI);
    DI += incr;
    v2 |= *(EXTRA_SEGMENT + DI) << 8;
    DI += incr;

    int value = AX - v2;
    setFlags(aCPU, value);

    IP++;
    *handled = 1;
    return;
}

// stosb
void stosb(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);
    int incr = DF ? -1 : 1;

    *(EXTRA_SEGMENT + DI) = AX & 0xff;

    DI += incr;
    IP++;
    *handled = 1;
    return;
}

// stosw
void stosw(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int incr = DF ? -1 : 1;

    *(EXTRA_SEGMENT + DI) = AX & 0xff;
    DI += incr;
    *(EXTRA_SEGMENT + DI) = (AX >> 8) & 0xff;
    DI += incr;
    IP++;
    *handled = 1;
    return;
}

void rep(struct emu8086 *aCPU, int *handled)
{
    if (CX == 0)
    {

        *handled = 1;
        IP += 2;
        return;
    }
    aCPU->op[*(CODE_SEGMENT + IP + 1)](aCPU, handled);
    aCPU->skip_next = 1;
    IP--;
    CX--;
}

void repne(struct emu8086 *aCPU, int *handled)
{
    int ZF = GET_FLAG(6);
    if (!ZF)
    {
        rep(aCPU, handled);
        return;
    }
    else
    {
        IP += 2;
        *handled = 1;
        return;
    }
}

// clc

void clc(struct emu8086 *aCPU, int *handled)
{
    // (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS))
    CLEAR_FLAG(0);
    IP++;
    *handled = 1;
}

// stc

void stc(struct emu8086 *aCPU, int *handled)
{
    // (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS))
    SET_FLAG(0);
    IP++;
    *handled = 1;
}

// cld

void cld(struct emu8086 *aCPU, int *handled)
{
    // (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS))
    CLEAR_FLAG(10);
    IP++;
    *handled = 1;
}

// std

void std(struct emu8086 *aCPU, int *handled)
{
    // (((~FLAGS & 0xffff) | (1 << f)) ^ FLAGS))
    SET_FLAG(10);
    IP++;
    *handled = 1;
}

// lea
void lea(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char opn;
    int *dest;
    dest = NULL;
    opn = *(CODE_SEGMENT_IP);
    unsigned char mod = (opn & 0b11000000) >> 6,
                  reg = (opn & 0b00111000) >> 3,
                  r_m = opn & 0b111;
    dest = SFRS + reg;
    if (mod == 0 && r_m == 6)
    {
        // special
        *dest = 0;
        IP++;
        int v = *(CODE_SEGMENT_IP);
        IP++;
        v |= *(CODE_SEGMENT_IP) << 8;
        *dest = v;
    }
    else
    {
        // special
        switch (r_m)
        {
        case 0:
            *dest = BX + SI;

            break;
        case 1:
            *dest = BX + DI;
            break;
        case 2:
            *dest = BP + SI;
            break;
        case 3:
            *dest = BP + DI;
            break;
        case 4:
            // printf("33jjjj\n");
            // special
            *dest = SI;
            // printf("33jjjj\n, %d\n", **dest);

            break;
        case 5:
            *dest = DI;
            break;
        case 6:
            *dest = BP;
            break;
        case 7:
            *dest = BX;
            break;
        }

        int width = mod, displacement = 0;
        if (width > 0)
        {
            IP++;
            displacement = *(CODE_SEGMENT_IP);
        }
        if (width > 1)
        {
            IP++;
            int d = 0;
            d = *(CODE_SEGMENT_IP);
            displacement |= d << 8;
        }

        *dest += displacement;
    }
    *handled = 1;
    IP++;
}

// les
void les(struct emu8086 *aCPU, int *handled)
{
    is_16 = 1;
    IP++;
    unsigned char *op1;
    int *op2; // *op3;
    op1 = NULL;
    op2 = NULL;
    unsigned char opn = *(CODE_SEGMENT + IP);
    get_ops_reg_8_addr(aCPU, opn, &op2, &op1);
    int value = 0;
    value = *op1;
    op1++;
    value = value | (*op1 << 8);
    op1++;
    *op2 = value;
    value = *op1;
    op1++;
    value = value | (*op1 << 8);
    op1++;
    ES = value;
    *handled = 1;
    IP++;
}

// rol_sar_8

void rol_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned char value;
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        int cf = IS_SET(value, 7);

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value |= cf;
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;

        value = *op3;
        int cf = IS_SET(value, 7);

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value = (value & 0xfe) | cf;

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void ror_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned char value;

        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        int cf = IS_SET(value, 0);
        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 7);
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;

        value = *op3;
        int cf = IS_SET(value, 0);

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value = (value & 0x7f) | (cf << 7);

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void rcl_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned char value;
        int cf = GET_FLAG(0);
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value |= cf;
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;
        int cf = GET_FLAG(0);

        value = *op3;
        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value = (value & 0xfe) | cf;

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void rcr_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        printf("%x", high_reg);
        unsigned char value;
        int cf = GET_FLAG(0);
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 7);
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;
        int cf = GET_FLAG(0);

        value = *op3;
        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value = (value & 0x7f) | (cf << 7);

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void shl_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        printf("%x", high_reg);
        unsigned char value;
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;
        value = *op3;
        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void shr_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        printf("%x", high_reg);
        unsigned char value;
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;
        value = *op3;
        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void sal_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned char value;
        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        int cf = IS_SET(value, 0);

        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value |= cf;
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;

        value = *op3;
        int cf = IS_SET(value, 0);

        if (IS_SET(value, 7))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= 1;
        value |= cf;

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void sar_8(struct emu8086 *aCPU, int *handled)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 0;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned char value;

        value = (high_reg) ? *op1 >> 8 : *op1 & 0xff;
        int cf = IS_SET(value, 7);
        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 7);
        *op1 = (high_reg) ? (*op1 & 0xff) | (value << 8) : (*op1 & 0xff00) | (value);
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned char value;

        value = *op3;
        int cf = IS_SET(value, 7);

        if (IS_SET(value, 0))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 7);

        *op3 = value;
    }
    IP++;

    *handled = 1;
}

void rol_sar_8(struct emu8086 *aCPU, int *handled)
{
    IP++;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int ins = (opn & 0b111000) >> 3;
    switch (ins)
    {
    case 0:
        rol_8(aCPU, handled);
        break;
    case 1:
        ror_8(aCPU, handled);
        break;
    case 2:
        rcl_8(aCPU, handled);
        break;
    case 3:
        rcr_8(aCPU, handled);
        break;
    case 4:

        shl_8(aCPU, handled);
        break;
    case 5:

        shr_8(aCPU, handled);
        break;

    case 6:

        sal_8(aCPU, handled);
        break;
    default:
        sar_8(aCPU, handled);

        break;
    }
}

void rol_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    printf("here %x \n", 3);

    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned short value;
        value = *op1;
        int shift = cl ? (CX & 0xf) : 1;

        int cf = IS_SET(value, (16 - shift));

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;
        value = *op3;
        value |= *(op3 + 1) << 8;

        int shift = cl ? (CX & 0xf) : 1;

        int cf = IS_SET(value, (16 - shift));

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        printf("%x \n", value);
        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void ror_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned short value;

        value = *op1;

        int shift = cl ? (CX & 0xf) : 1;
        int cf = IS_SET(value, (shift - 1));
        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;
        value |= (cf << 15);
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;

        value = *op3;
        value |= *(op3 + 1) << 8;

        int shift = cl ? (CX & 0xf) : 1;
        int cf = IS_SET(value, (shift - 1));

        if (cf)
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;
        value |= (cf << 15);

        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void rcl_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    printf("here %x \n", 3);

    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned short value;
        value = *op1;
        int cf = GET_FLAG(0);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;
        value = *op3;
        value |= *(op3 + 1) << 8;
        int cf = GET_FLAG(0);

        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        printf("%x \n", value);
        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void rcr_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned short value;

        value = *op1;
        int cf = GET_FLAG(0);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;
        value |= (cf << 15);
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;

        value = *op3;
        value |= *(op3 + 1) << 8;

        int cf = GET_FLAG(0);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))

            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;
        value |= (cf << 15);

        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void shl_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;

    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned short value;
        value = *op1;

        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        printf("%x, \n", value);
        *op1 = value & 0xffff;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;
        value = *op3;
        value |= *(op3 + 1) << 8;
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        printf("%x \n", value);
        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void shr_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned short value;

        value = *op1;
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;

        value = *op3;
        value |= *(op3 + 1) << 8;

        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= shift;

        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void sal_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    printf("here %x \n", 3);

    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {
        unsigned short value;
        value = *op1;
        int cf = IS_SET(value, 0);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;
        value = *op3;
        value |= *(op3 + 1) << 8;

        int cf = IS_SET(value, 0);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (16 - shift)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value <<= shift;
        value |= cf;
        printf("%x \n", value);
        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void sar_16(struct emu8086 *aCPU, int *handled, int cl)
{

    int *op2, value = 0;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int b = 0;
    int *op1;
    op2 = op1 = NULL;
    is_16 = 1;
    high_reg = 0;
    b = get_ops_reg_8(aCPU, (opn & 0xc7), &op2, &op1);
    if (b)
    {

        unsigned short value;

        value = *op1;
        int cf = IS_SET(value, 15);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 15);
        *op1 = value;
    }

    else
    {
        unsigned char *op3;
        op3 = NULL;
        get_ops_reg_8_addr(aCPU, (opn & 0xc7), &op2, &op3);
        // TODO - Warn error
        if (op3 == NULL)
            exit(1);
        unsigned short value;

        value = *op3;
        value |= *(op3 + 1) << 8;

        int cf = IS_SET(value, 15);
        int shift = cl ? (CX & 0xf) : 1;

        if (IS_SET(value, (shift - 1)))
            SET_FLAG(0);
        else
            CLEAR_FLAG(0);
        value >>= 1;
        value |= (cf << 15);

        *op3++ = value & 0xff;
        *op3 = value >> 8;
    }
    IP++;

    *handled = 1;
}

void rol_sar_16(struct emu8086 *aCPU, int *handled)
{
    int cl = *(CODE_SEGMENT + IP) == 0xd3;
    IP++;
    unsigned char opn = *(CODE_SEGMENT + IP);
    int ins = (opn & 0b111000) >> 3;
    switch (ins)
    {
    case 0:
        rol_16(aCPU, handled, cl);
        break;
    case 1:
        ror_16(aCPU, handled, cl);
        break;
    case 2:
        rcl_16(aCPU, handled, cl);
        break;
    case 3:
        rcr_16(aCPU, handled, cl);
        break;
    case 4:

        shl_16(aCPU, handled, cl);
        break;
    case 5:

        shr_16(aCPU, handled, cl);
        break;

    case 6:

        sal_16(aCPU, handled, cl);
        break;
    default:
        sar_16(aCPU, handled, cl);

        break;
    }
}

// ANCHOR

// unimplemented instructions
void unimp(struct emu8086 *aCPU, int *handled)
{
    IP = _INSTRUCTIONS->end_address + 1;
    char buf[35];
    sprintf(buf, errors_str[UNIMPLEMENTED], _INSTRUCTIONS->line_number);
    massage(buf, ERR);
    *handled = 1;
}
// end ops
#ifdef DEBUG
void dump_memory(struct emu8086 *aCPU)
{
    int start = DS * 10;
    int end = start + 0xffff;
    while (start < end)
    {
        if (aCPU->mDataMem[start] > 0)
            printf("%05x: %02x\n", start, aCPU->mDataMem[start]);
        start++;
    }
}
void dump_stack(struct emu8086 *aCPU)
{
    int start = _SS * 0x10;
    int end = start + 50;

    while (start < end)
    {
        if (aCPU->mDataMem[start] > 0)
            printf("%05x: %02x\n", start, aCPU->mDataMem[start]);
        start++;
    }
}
void dump_registers(struct emu8086 *aCPU)
{
    char *r[22] = {
        "REG_AX",
        "REG_CX",
        "REG_DX",
        "REG_BX",
        "REG_SP",
        "REG_BP",
        "REG_SI",
        "REG_DI",
        //,

        "REG_8",
        "REG_9",
        "REG_10",
        "REG_11",
        "REG_12",
        "REG_13",
        "REG_14",
        "REG_15",

        "REG_FLAGS",
        "REG_IP",
        "REG_CS",
        "REG_DS",
        "REG_ES",
        "REG_SS"
        //  "REG_ES",
    };
    for (int i = 0; i < 22; i++)
    {
        /* code */
        printf("%s: %04x\n", r[i], aCPU->mSFR[i]);
    }
}
#endif

void op_setptrs(struct emu8086 *aCPU)
{
    for (int i = 0; i < 256; i++)
        aCPU->op[i] = &unimp;
    aCPU->op[NOP] = &nop;

    aCPU->op[INT_I8] = &int_;
    aCPU->op[INT3] = &nop;
    aCPU->op[INTO] = &nop;
    aCPU->op[IRET] = &nop;
    // ADD
    aCPU->op[ADD_D8_R8] = &add_addr8_reg8;
    aCPU->op[ADD_D16_R16] = &add_addr16_reg16;
    aCPU->op[ADD_R8_D8] = &add_reg8_addr8;
    aCPU->op[ADD_R16_D16] = &add_reg16_addr16;
    aCPU->op[ADD_AL_I8] = &add_al_i8;
    aCPU->op[ADD_AX_I16] = &add_ax_i16;
    aCPU->op[ADD_D8_I8] = &add_addr8_i8;
    aCPU->op[ADD_D16_S8] = &add_addr16_s8;
    aCPU->op[ADD_D16_I16] = &add_addr16_d16;

    // OR
    aCPU->op[OR_D8_R8] = &or_addr8_reg8;
    aCPU->op[OR_D16_R16] = &or_addr16_reg16;
    aCPU->op[OR_R8_D8] = &or_reg8_addr8;
    aCPU->op[OR_R16_D16] = &or_reg16_addr16;
    aCPU->op[OR_AL_I8] = &or_al_i8;
    aCPU->op[OR_AX_I16] = &or_ax_i16;

    // ADC
    aCPU->op[ADC_D8_R8] = &adc_addr8_reg8;
    aCPU->op[ADC_D16_R16] = &adc_addr16_reg16;
    aCPU->op[ADC_R8_D8] = &adc_reg8_addr8;
    aCPU->op[ADC_R16_D16] = &adc_reg16_addr16;
    aCPU->op[ADC_AL_I8] = &adc_al_i8;
    aCPU->op[ADC_AX_I16] = &adc_ax_i16;

    // SBB
    aCPU->op[SBB_D8_R8] = &sbb_addr8_reg8;
    aCPU->op[SBB_D16_R16] = &sbb_addr16_reg16;
    aCPU->op[SBB_R8_D8] = &sbb_reg8_addr8;
    aCPU->op[SBB_R16_D16] = &sbb_reg16_addr16;
    aCPU->op[SBB_AL_I8] = &sbb_al_i8;
    aCPU->op[SBB_AX_I16] = &sbb_ax_i16;

    // JMP

    aCPU->op[JMP_A8] = &jmp_8;
    aCPU->op[JMP_A16] = &jmp_16;

    aCPU->op[JCXZ_A8] = &jcxz_8;
    aCPU->op[JC_A8] = &jc_8;
    aCPU->op[JNC_A8] = &jnc_8;
    aCPU->op[JO_A8] = &jo_8;
    aCPU->op[JNO_A8] = &jno_8;
    aCPU->op[JS_A8] = &js_8;
    aCPU->op[JNS_A8] = &jns_8;
    aCPU->op[JZ_A8] = &jz_8;
    aCPU->op[JNZ_A8] = &jnz_8;
    aCPU->op[JA_A8] = &ja_8;
    aCPU->op[JBE_A8] = &jbe_8;

    aCPU->op[JL_A8] = &jl_8;
    aCPU->op[JLE_A8] = &jle_8;

    aCPU->op[JG_A8] = &jg_8;
    aCPU->op[JGE_A8] = &jge_8;
    aCPU->op[JPE_A8] = &jpe_8;
    aCPU->op[JPO_A8] = &jpo_8;
    // LOOP
    aCPU->op[LOOP_A8] = &loop_a8;
    aCPU->op[LOOPZ_A8] = &loopz_a8;

    aCPU->op[LOOPNZ_A8] = &loopnz_a8;

    // PUSH
    aCPU->op[PUSH_ES] = &push_reg;
    aCPU->op[PUSH_CS] = &push_reg;
    aCPU->op[PUSH_SS] = &push_reg;
    aCPU->op[PUSH_DS] = &push_reg;
    aCPU->op[PUSH_D16] = &push_addr;

    // POP
    aCPU->op[POP_ES] = &pop_reg;

    aCPU->op[POP_SS] = &pop_reg;
    aCPU->op[POP_DS] = &pop_reg;
    aCPU->op[POP_D16] = &pop_addr;
    // AND
    aCPU->op[AND_D8_R8] = &and_addr8_reg8;
    aCPU->op[AND_D16_R16] = &and_addr16_reg16;
    aCPU->op[AND_R8_D8] = &and_reg8_addr8;
    aCPU->op[AND_R16_D16] = &and_reg16_addr16;
    aCPU->op[AND_AL_I8] = &and_al_i8;
    aCPU->op[AND_AX_I16] = &and_ax_i16;

    // SUB
    aCPU->op[SUB_D8_R8] = &sub_addr8_reg8;
    aCPU->op[SUB_D16_R16] = &sub_addr16_reg16;
    aCPU->op[SUB_R8_D8] = &sub_reg8_addr8;
    aCPU->op[SUB_R16_D16] = &sub_reg16_addr16;
    aCPU->op[SUB_AL_I8] = &sub_al_i8;
    aCPU->op[SUB_AX_I16] = &sub_ax_i16;

    // XOR
    aCPU->op[XOR_D8_R8] = &xor_addr8_reg8;
    aCPU->op[XOR_D16_R16] = &xor_addr16_reg16;
    aCPU->op[XOR_R8_D8] = &xor_reg8_addr8;
    aCPU->op[XOR_R16_D16] = &xor_reg16_addr16;
    aCPU->op[XOR_AL_I8] = &xor_al_i8;
    aCPU->op[XOR_AX_I16] = &xor_ax_i16;

    // MOV

    aCPU->op[MOV_D16_R16] = &mov_addr16_r16;

    aCPU->op[MOV_D8_R8] = &mov_addr8_reg8;
    aCPU->op[MOV_R8_D8] = &mov_reg8_addr8;
    aCPU->op[MOV_R16_D16] = &mov_reg16_addr16;
    aCPU->op[MOV_AL_ADDR] = &mov_al_addr;
    aCPU->op[MOV_AX_ADDR] = &mov_ax_addr;
    aCPU->op[MOV_ADDR_AL] = &mov_addr_al;
    aCPU->op[MOV_ADDR_AX] = &mov_addr_ax;
    aCPU->op[MOV_D16_DS] = &mov_addr_cs;
    aCPU->op[MOV_DS_D16] = &mov_cs_addr;
    aCPU->op[MOV_DB8_I8] = &mov_addr8_i8;

    aCPU->op[MOV_DW16_I16] = &mov_addr16_i16;

    // CMP
    aCPU->op[CMP_D8_R8] = &cmp_addr8_reg8;
    aCPU->op[CMP_D16_R16] = &cmp_addr16_reg16;
    aCPU->op[CMP_R8_D8] = &cmp_reg8_addr8;
    aCPU->op[CMP_R16_D16] = &cmp_reg16_addr16;
    aCPU->op[CMP_AL_I8] = &cmp_al_i8;
    aCPU->op[CMP_AX_I16] = &cmp_ax_i16;

    // CALL
    aCPU->op[CALL_A16] = &call_addr;
    // RET
    aCPU->op[RET] = &ret_addr;
    aCPU->op[INC_DB8] = &inc_addr8;

    // XCHG
    aCPU->op[XCHG_D8_R8] = &xchg_r8_d8;
    aCPU->op[XCHG_D16_R16] = &xchg_r16_d16;
    // AAA
    aCPU->op[AAA] = &aaa;

    // AAD
    aCPU->op[AAD] = &aad;

    // AAM
    aCPU->op[AAM] = &aam;
    // AAS
    aCPU->op[AAS] = &aas;

    // CMPSB
    aCPU->op[CMPSB] = &cmpsb;

    // CMPSW
    aCPU->op[CMPSW] = &cmpsw;

    // LODSB
    aCPU->op[LODSB] = &lodsb;

    // LODSW
    aCPU->op[LODSW] = &lodsw;

    // MOVSB
    aCPU->op[MOVSB] = &movsb;

    // MOVSW
    aCPU->op[MOVSW] = &movsw;

    // STOSB
    aCPU->op[STOSB] = &stosb;

    // STOSW
    aCPU->op[STOSW] = &stosw;
    // SCASB
    aCPU->op[SCASB] = &scasb;

    // SCASW
    aCPU->op[SCASW] = &scasw;
    // REP
    aCPU->op[REP] = &rep;

    // REPNE
    aCPU->op[REPNE] = &repne;

    // CLC
    aCPU->op[CLC] = &clc;

    // STC
    aCPU->op[STC] = &stc;
    // CLD
    aCPU->op[CLD] = &cld;

    // STC
    aCPU->op[STD] = &std;

    // LEA
    aCPU->op[LEA_R16_D16] = &lea;

    // LES
    aCPU->op[LES_R16_D16] = &les;

    // ROL TO SAR
    aCPU->op[ROL_8_SAR_8] = &rol_sar_8;
    aCPU->op[ROL_8_SAR_16] = &rol_sar_16;
    aCPU->op[ROL_8_SAR_8 + 1] = &rol_sar_8;
    aCPU->op[0xd3] = &rol_sar_16;
    // aCPU->op[ROL_8_SAR_8]

    for (int i = 0; i < 8; i++)
    {
        /* code */

        aCPU->op[64 + i] = &inc_reg16;
        aCPU->op[72 + i] = &dec_reg16;
        aCPU->op[80 + i] = &push_reg16;
        aCPU->op[88 + i] = &pop_reg16;
        aCPU->op[176 + i] = &mov_reg8_i8;
        if (i < 7)
            aCPU->op[145 + i] = &xchg_ax_r16;
        aCPU->op[184 + i] = &mov_reg16_i16;
    }

    //  aCPU->op[MOV_DW16_I16] = &mov_
}
