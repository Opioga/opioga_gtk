/*

Copyright (c) 2019 Oscar Toledo G. http://nanochess.org/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <emu8086.h>
#include <ins.h>
#include <assembler.h>

char *reg1[16] = {
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH",
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"};

int label_identifier, is_offset;
struct label *label_list, *explore;
struct label *last_label, *current_label, *_current_label;
int is_first;

char *p;
char line[256];
char part[MAX_SIZE];
int errors;
struct instruction *_instruction_list;
struct instruction *_last_instruction;
struct instruction *_current_instruction, *_first_instruction;
struct variable *variable_list, *first_variable, *v_ordered_list;
int undefined, instruction_addr,
    instruction_register, instruction_value2;
unsigned char code_mem[0xffff];
char global_label[MAX_SIZE];
char name[MAX_SIZE];
char expr_name[MAX_SIZE];
char undefined_name[MAX_SIZE];
struct errors_list *define_errors_list(char *name, int line);
struct errors_list *first_err, *list_err;
int instruction_addressing, ic;
int instruction_offset;
int instruction_offset_width, start_address, address, assembler_step,
    instruction_value;

int line_number, assembler_step, size, data_mem_offset;

struct errors_list *define_errors_list(char *name, int line)
{
    struct errors_list *error;
    error = malloc(sizeof(struct errors_list) + strlen(name));
    if (error == NULL)
    {
        printf("Out of memory for label\n");
        exit(1);
        return NULL;
    }

    strcpy(error->message, name);
    error->prev = NULL;
    error->next = NULL;
    error->line = line;
    if (first_err == NULL)
        first_err = error;
    if (list_err == NULL)
        list_err = error;
    else
    {
        error->prev = list_err;
        list_err->next = error;
        list_err = error;
    }
    return error;
};
struct label *define_label(name, value) char *name;
int value;
{
    struct label *label;
    struct label *explore;
    int c;

    /* Allocate label */
    label = malloc(sizeof(struct label) + strlen(name));
    if (label == NULL)
    {
        printf("Out of memory for label\n");
        exit(1);
        return NULL;
    }

    /* Fill label */
    label->left = NULL;
    label->right = NULL;
    label->value = value;

    strcpy(label->name, name);
    label->label_identifier = label_identifier;
    label_identifier++;

    /* Populate binary tree */
    if (label_list == NULL)
    {
        label_list = label;
    }
    else
    {

        explore = label_list;
        while (1)
        {
            c = strcmp(label->name, explore->name);
            if (c < 0)
            {
                if (explore->left == NULL)
                {
                    explore->left = label;
                    break;
                }
                explore = explore->left;
            }
            else if (c > 0)
            {
                if (explore->right == NULL)
                {
                    explore->right = label;
                    break;
                }
                explore = explore->right;
            }
        }
    }
    current_label = label;
    return label;
}
struct label *find_label(name) char *name;
{
    struct label *explore;
    int c;

    /* Follows a binary tree */
    explore = label_list;

    while (explore != NULL)
    {

        c = strcmp(name, explore->name);
        if (c == 0)
        {
            return explore;
        }
        if (c < 0)
            explore = explore->left;
        else
            explore = explore->right;
    }
    return NULL;
}

struct variable *define_variable(name, value) char *name;
int value;
{
    size = 0;
    struct variable *variable;
    //   struct variable *explore;
    // int c;

    /* Allocate variable */
    variable = malloc(sizeof(struct variable) + strlen(name));
    if (variable == NULL)
    {
        printf("Out of memory for variable\n");
        exit(1);
        return NULL;
    }
    printf("%s\n", name);
    /* Fill variable */
    variable->left = NULL;
    variable->right = NULL;
    variable->starting_address = value;
    variable->offset = data_mem_offset;
    strcpy(variable->name, name);
    //label->label_identifier = label_identifier;
    //label_identifier++;

    /* Populate binary tree */
    if (variable_list == NULL)
    {
        variable_list = variable;
    }

    else
    {
        variable->left = variable_list;
        variable_list->right = variable;
        variable_list = variable;
    }
    if (first_variable == NULL)
        first_variable = variable;
    return variable;
}
struct variable *find_variable(name) char *name;
{
    struct variable *explore;
    int c;

    /* Follows a binary tree */
    explore = first_variable;

    while (explore != NULL)
    {

        c = strcmp(name, explore->name);
        if (c == 0)
        {
            return explore;
        }
        else
            explore = explore->right;
    }
    return NULL;
}

void message(char *m, int level, int line)
{
    if (level == ERR)
    {
        define_errors_list(m, line);
        errors++;
        // printf( m);
    }
    else if (level == WARN)
    {
        printf(m);
    }
#ifdef DEBUG
    else
        fprintf(stdout, m);
#endif
}
void _message(char *m, int level)
{
    fprintf(stdout, m);
};

/*
 ** Avoid spaces in input
 */
char *avoid_spaces(p) char *p;

{
    while (isspace(*p))
        p++;
    return p;
}

void seperate()
{
    char *p2;

    while (*p && isspace(*p))
        p++;
    p2 = part;

    while (*p && !isspace(*p) && *p != ';')
        *p2++ = *p++;
    *p2 = '\0';
    while (*p && isspace(*p))
        p++;
    p2 = part;
}
struct instruction *define_instruction(int line)
{
    struct instruction *new = (struct instruction *)malloc(sizeof(struct instruction));
    new->line_number = line_number;
    new->next = NULL;
    new->prev = NULL;
    if (is_first)
    {
        _first_instruction = new;
    }
    ic++;
    return new;
}

void emit_byte(int c)
{
    // printf("%03x: %02x\n", address, c);
    if (assembler_step == 1)
        *(code_mem + address) = c;
    address++;
}

int _check__end(p) char *p;
{
    int b = 0;
    p = avoid_spaces(p);
    b = !(*p);
    return b;
}
void check_end(p) char *p;
{
    p = avoid_spaces(p);
    if (*p && *p != ';')
    {
        char buf[45];
        sprintf(buf, "Error: extra characters at end of line %d\n", line_number);
        message(buf, ERR, line_number);
        errors++;
    }
}
/*
 ** Read character for string or character literal
 */
char *read_character(p, c) char *p;
int *c;
{
    if (*p == '\\')
    {
        p++;
        if (*p == '\'')
        {
            *c = '\'';
            p++;
        }
        else if (*p == '\"')
        {
            *c = '"';
            p++;
        }
        else if (*p == '\\')
        {
            *c = '\\';
            p++;
        }
        else if (*p == 'a')
        {
            *c = 0x07;
            p++;
        }
        else if (*p == 'b')
        {
            *c = 0x08;
            p++;
        }
        else if (*p == 't')
        {
            *c = 0x09;
            p++;
        }
        else if (*p == 'n')
        {
            *c = 0x0a;
            p++;
        }
        else if (*p == 'v')
        {
            *c = 0x0b;
            p++;
        }
        else if (*p == 'f')
        {
            *c = 0x0c;
            p++;
        }
        else if (*p == 'r')
        {
            *c = 0x0d;
            p++;
        }
        else if (*p == 'e')
        {
            *c = 0x1b;
            p++;
        }
        else if (*p >= '0' && *p <= '7')
        {
            *c = 0;
            while (*p >= '0' && *p <= '7')
            {
                *c = *c * 8 + (*p - '0');
                p++;
            }
        }
        else
        {
            p--;
            message("bad escape inside string\n", ERR, line_number);
        }
    }
    else
    {
        *c = *p;
        p++;
    }
    return p;
}

/*
 ** Match expression (top tier)
 */
char *match_expression(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level1(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '|')
        { /* Binary OR */
            p++;
            value1 = *value;
            p = match_expression_level1(p, value);
            if (p == NULL)
                return NULL;
            *value |= value1;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *match_expression_level1(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level2(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '^')
        { /* Binary XOR */
            p++;
            value1 = *value;
            p = match_expression_level2(p, value);
            if (p == NULL)
                return NULL;
            *value ^= value1;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *match_expression_level2(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level3(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '&')
        { /* Binary AND */
            p++;
            value1 = *value;
            p = match_expression_level3(p, value);
            if (p == NULL)
                return NULL;
            *value &= value1;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *match_expression_level3(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level4(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '<' && p[1] == '<')
        { /* Shift to left */
            p += 2;
            value1 = *value;
            p = match_expression_level4(p, value);
            if (p == NULL)
                return NULL;
            *value = value1 << *value;
        }
        else if (*p == '>' && p[1] == '>')
        { /* Shift to right */
            p += 2;
            value1 = *value;
            p = match_expression_level4(p, value);
            if (p == NULL)
                return NULL;
            *value = value1 >> *value;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *match_expression_level4(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level5(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '+')
        { /* Add operator */
            p++;
            value1 = *value;
            p = match_expression_level5(p, value);
            if (p == NULL)
                return NULL;
            *value = value1 + *value;
        }
        else if (*p == '-')
        { /* Subtract operator */
            p++;
            value1 = *value;
            p = match_expression_level5(p, value);
            if (p == NULL)
                return NULL;
            *value = value1 - *value;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *match_expression_level5(p, value) char *p;
int *value;
{
    int value1;

    p = match_expression_level6(p, value);
    if (p == NULL)
        return NULL;
    while (1)
    {
        p = avoid_spaces(p);
        if (*p == '*')
        { /* Multiply operator */
            p++;
            value1 = *value;
            p = match_expression_level6(p, value);
            if (p == NULL)
                return NULL;
            *value = value1 * *value;
        }
        else if (*p == '/')
        { /* Division operator */
            p++;
            value1 = *value;
            p = match_expression_level6(p, value);
            if (p == NULL)
                return NULL;
            if (*value == 0)
            {

                message("division by zero", WARN, line_number);

                *value = 1;
            }
            *value = (unsigned)value1 / *value;
        }
        else if (*p == '%')
        { /* Modulo operator */
            p++;
            value1 = *value;
            p = match_expression_level6(p, value);
            if (p == NULL)
                return NULL;
            if (*value == 0)
            {
                if (assembler_step == 2)
                    message("division by zero", ERR, line_number);
            }
            *value = value1 % *value;
        }
        else
        {
            return p;
        }
    }
}

/*
 ** Match expression (bottom tier)
 */
char *match_expression_level6(p, value) char *p;
int *value;
{
    int number;
    int c;
    char *p2;
    struct label *label = NULL;

    p = avoid_spaces(p);
    if (*p == '(')
    { /* Handle parenthesized expressions */
        p++;
        p = match_expression(p, value);
        if (p == NULL)
            return NULL;
        p = avoid_spaces(p);
        if (*p != ')')
            return NULL;
        p++;
        return p;
    }
    if (*p == '-')
    { /* Simple negation */
        p++;
        p = match_expression_level6(p, value);
        if (p == NULL)
            return NULL;
        *value = -*value;
        return p;
    }
    if (*p == '+')
    { /* Unary */
        p++;
        p = match_expression_level6(p, value);
        if (p == NULL)
            return NULL;
        return p;
    }
    if (p[0] == '0' && tolower(p[1]) == 'b')
    { /* Binary */
        p += 2;
        number = 0;
        while (p[0] == '0' || p[0] == '1' || p[0] == '_')
        {
            if (p[0] != '_')
            {
                number <<= 1;
                if (p[0] == '1')
                    number |= 1;
            }
            p++;
        }
        *value = number;
        return p;
    }
    if (p[0] == '0' && tolower(p[1]) == 'x' && isxdigit(p[2]))
    { /* Hexadecimal */
        p += 2;
        number = 0;
        while (isxdigit(p[0]))
        {
            c = toupper(p[0]);
            c = c - '0';
            if (c > 9)
                c -= 7;
            number = (number << 4) | c;
            p++;
        }
        *value = number;
        return p;
    }
    if (p[0] == '$' && isdigit(p[1]))
    { /* Hexadecimal */
        /* This is nasm syntax, notice no letter is allowed after $ */
        /* So it's preferrable to use prefix 0x for hexadecimal */
        p += 1;
        number = 0;
        while (isxdigit(p[0]))
        {
            c = toupper(p[0]);
            c = c - '0';
            if (c > 9)
                c -= 7;
            number = (number << 4) | c;
            p++;
        }
        *value = number;
        return p;
    }
    if (p[0] == '\'')
    { /* Character constant */
        p++;
        p = read_character(p, value);
        if (p[0] != '\'')
        {
            message("Missing apostrophe", ERR, line_number);
        }
        else
        {
            p++;
        }
        return p;
    }
    if (isdigit(*p))
    { /* Decimal */
        number = 0;
        while (isdigit(p[0]))
        {
            c = p[0] - '0';
            number = number * 10 + c;
            p++;
        }
        *value = number;
        return p;
    }
    if (*p == '$' && p[1] == '$')
    { /* Start address */
        p += 2;
        *value = start_address;
        return p;
    }
    if (*p == '$')
    { /* Current address */
        p++;
        *value = address;
        return p;
    }
    if (isalpha(*p) || *p == '_' || *p == '.')
    { /* Label */
        if (*p == '.')
        {
            strcpy(expr_name, global_label);
            p2 = expr_name;
            while (*p2)
                p2++;
        }
        else
        {
            p2 = expr_name;
        }
        while (isalpha(*p) || isdigit(*p) || *p == '_' || *p == '.')
            *p2++ = *p++;
        *p2 = '\0';
        for (c = 0; c < 16; c++)
            if (strcmp(expr_name, reg1[c]) == 0)
                return NULL;
        label = find_label(expr_name);
        if (label == NULL)
        {
            *value = 0;
            undefined++;
            strcpy(undefined_name, expr_name);
            // if (assembler_step)
            // {
            //     //     // if (!nu->is_defined)

            //     if ((!strcmp(expr_name, "WORD") == 0))
            //     {
            //         char m[25 + MAX_SIZE];
            //         sprintf(m, "Undefined label '%s' on line %d\n", expr_name, line_number);
            //         message(m, ERR, line_number);
            //     }
            // }
        }
        // (() )
        else
        {
            *value = label->value;
        }
        return p;
    }
    return NULL;
}

char *match_register(p, width, value) char *p;
int width;
int *value;
{
    char reg[3];
    int c;

    p = avoid_spaces(p);
    if (!isalpha(p[0]) || !isalpha(p[1]))
        return NULL;
    reg[0] = p[0];
    reg[1] = p[1];
    reg[2] = '\0';
    if (width == 8)
    { /* 8-bit */
        for (c = 0; c < 8; c++)
            if (strcmp(reg, reg1[c]) == 0)
                break;
        if (c < 8)
        {
            *value = c;
            return p + 2;
        }
    }
    else
    { /* 16-bit */
        for (c = 0; c < 8; c++)
            if (strcmp(reg, reg1[c + 8]) == 0)
                break;
        if (c < 8)
        {
            *value = c;
            return p + 2;
        }
    }
    return NULL;
}

char *match_addressing(p, width) char *p;
int width;
{
    int reg;
    int reg2;
    char *p2;
    int *bits;

    bits = &instruction_addressing;
    instruction_offset = 0;
    instruction_offset_width = 0;

    p = avoid_spaces(p);
    if (*p == '[')
    {
        p = avoid_spaces(p + 1);
        p2 = match_register(p, 16, &reg);
        if (p2 != NULL)
        {
            p = avoid_spaces(p2);
            if (*p == ']')
            {
                p++;
                if (reg == 3)
                { /* BX */
                    *bits = 0x07;
                }
                else if (reg == 5)
                { /* BP */
                    *bits = 0x46;
                    instruction_offset = 0;
                    instruction_offset_width = 1;
                }
                else if (reg == 6)
                { /* SI */
                    *bits = 0x04;
                }
                else if (reg == 7)
                { /* DI */
                    *bits = 0x05;
                }
                else
                { /* Not valid */
                    return NULL;
                }
            }
            else if (*p == '+' || *p == '-')
            {
                if (*p == '+')
                {
                    p = avoid_spaces(p + 1);
                    is_offset = 1;
                    p2 = match_register(p, 16, &reg2);
                }
                else
                {
                    p2 = NULL;
                }
                if (p2 != NULL)
                {
                    if ((reg == 3 && reg2 == 6) || (reg == 6 && reg2 == 3))
                    { /* BX+SI / SI+BX */
                        *bits = 0x00;
                    }
                    else if ((reg == 3 && reg2 == 7) || (reg == 7 && reg2 == 3))
                    { /* BX+DI / DI+BX */
                        *bits = 0x01;
                    }
                    else if ((reg == 5 && reg2 == 6) || (reg == 6 && reg2 == 5))
                    { /* BP+SI / SI+BP */
                        *bits = 0x02;
                    }
                    else if ((reg == 5 && reg2 == 7) || (reg == 7 && reg2 == 5))
                    { /* BP+DI / DI+BP */
                        *bits = 0x03;
                    }
                    else
                    { /* Not valid */
                        return NULL;
                    }
                    p = avoid_spaces(p2);
                    if (*p == ']')
                    {
                        p++;
                    }
                    else if (*p == '+' || *p == '-')
                    {
                        p2 = match_expression(p, &instruction_offset);
                        is_offset = 1;
                        if (p2 == NULL)
                            return NULL;
                        p = avoid_spaces(p2);
                        if (*p != ']')
                            return NULL;
                        p++;
                        if (instruction_offset >= -0x80 && instruction_offset <= 0x7f)
                        {
                            instruction_offset_width = 1;
                            *bits |= 0x40;
                        }
                        else
                        {
                            instruction_offset_width = 2;
                            *bits |= 0x80;
                        }
                    }
                    else
                    { /* Syntax error */
                        return NULL;
                    }
                }
                else
                {
                    if (reg == 3)
                    { /* BX */
                        *bits = 0x07;
                    }
                    else if (reg == 5)
                    { /* BP */
                        *bits = 0x06;
                    }
                    else if (reg == 6)
                    { /* SI */
                        *bits = 0x04;
                    }
                    else if (reg == 7)
                    { /* DI */
                        *bits = 0x05;
                    }
                    else
                    { /* Not valid */
                        return NULL;
                    }
                    p2 = match_expression(p, &instruction_offset);
                    if (p2 == NULL)
                        return NULL;
                    p = avoid_spaces(p2);
                    if (*p != ']')
                        return NULL;
                    p++;
                    if (instruction_offset >= -0x80 && instruction_offset <= 0x7f)
                    {
                        instruction_offset_width = 1;
                        *bits |= 0x40;
                    }
                    else
                    {
                        instruction_offset_width = 2;
                        *bits |= 0x80;
                    }
                }
            }
            else
            { /* Syntax error */
                return NULL;
            }
        }
        else
        { /* No valid register, try expression (absolute addressing) */
            p2 = match_expression(p, &instruction_offset);
            if (p2 == NULL)
                return NULL;
            p = avoid_spaces(p2);
            if (*p != ']')
                return NULL;
            else
                is_offset = 1;
            p++;
            *bits = 0x06;
            instruction_offset_width = 2;
        }
    }
    else
    { /* Register */
        p = match_register(p, width, &reg);
        if (p == NULL)
            return NULL;
        *bits = 0xc0 | reg;
    }
    return p;
}
/*
 ** Search for a match with instruction
 */
char *match(p, pattern, decode) char *p;
char *pattern;
char *decode;
{
    char *p2;
    int c;
    int d;
    int bit;
    int qualifier;
    char *base;

    undefined = 0;
    while (*pattern)
    {
        /*        fputc(*pattern, stdout);*/
        if (*pattern == '%')
        { /* Special */
            pattern++;
            if (*pattern == 'd')
            { /* Addressing */
                pattern++;
                qualifier = 0;
                if (memcmp(p, "WORD", 4) == 0 && !isalpha(p[4]))
                {
                    p = avoid_spaces(p + 4);
                    if (*p != '[')
                        return NULL;
                    qualifier = 16;
                }
                else if (memcmp(p, "BYTE", 4) == 0 && !isalpha(p[4]))
                {
                    p = avoid_spaces(p + 4);
                    if (*p != '[')
                        return NULL;
                    qualifier = 8;
                }
                if (*pattern == 'w')
                {
                    pattern++;
                    if (qualifier != 16 && match_register(p, 16, &d) == 0)
                        return NULL;
                }
                else if (*pattern == 'b')
                {
                    pattern++;
                    if (qualifier != 8 && match_register(p, 8, &d) == 0)
                        return NULL;
                }
                else
                {
                    if (qualifier == 8 && *pattern != '8')
                        return NULL;
                    if (qualifier == 16 && *pattern != '1')
                        return NULL;
                }
                if (*pattern == '8')
                {
                    pattern++;
                    p2 = match_addressing(p, 8);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else if (*pattern == '1' && pattern[1] == '6')
                {
                    pattern += 2;
                    p2 = match_addressing(p, 16);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else if (*pattern == 'r')
            { /* Register */
                pattern++;
                if (*pattern == '8')
                {
                    pattern++;
                    p2 = match_register(p, 8, &instruction_register);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else if (*pattern == '1' && pattern[1] == '6')
                {
                    pattern += 2;
                    p2 = match_register(p, 16, &instruction_register);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else if (*pattern == 'i')
            { /* Immediate */
                pattern++;
                if (*pattern == '8')
                {
                    pattern++;
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else if (*pattern == '1' && pattern[1] == '6')
                {
                    pattern += 2;
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else if (*pattern == 'a')
            { /* Address for jump */
                pattern++;
                if (*pattern == '8')
                {
                    pattern++;
                    p = avoid_spaces(p);
                    qualifier = 0;
                    if (memcmp(p, "SHORT", 5) == 0 && isspace(p[5]))
                    {
                        p += 5;
                        qualifier = 1;
                    }
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    if (qualifier == 0)
                    {
                        c = instruction_value - (address + 2);
                        if (undefined == 0 && (c < -128 || c > 127) && memcmp(decode, "xeb", 3) == 0)
                            return NULL;
                    }
                    p = p2;
                }
                else if (*pattern == '1' && pattern[1] == '6')
                {
                    pattern += 2;
                    p = avoid_spaces(p);
                    if (memcmp(p, "SHORT", 5) == 0 && isspace(p[5]))
                        p2 = NULL;
                    else
                        p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else if (*pattern == 's')
            { /* Signed immediate */
                pattern++;
                if (*pattern == '8')
                {
                    pattern++;
                    p = avoid_spaces(p);
                    qualifier = 0;
                    if (memcmp(p, "BYTE", 4) == 0 && isspace(p[4]))
                    {
                        p += 4;
                        qualifier = 1;
                    }
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    if (qualifier == 0)
                    {
                        c = instruction_value;
                        if (undefined != 0)
                            return NULL;
                        if (undefined == 0 && (c < -128 || c > 127))
                            return NULL;
                    }
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else if (*pattern == 'f')
            { /* FAR pointer */
                pattern++;
                if (*pattern == '3' && pattern[1] == '2')
                {
                    pattern += 2;
                    p2 = match_expression(p, &instruction_value2);
                    if (p2 == NULL)
                        return NULL;
                    if (*p2 != ':')
                        return NULL;
                    p = p2 + 1;
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
            continue;
        }
        if (toupper(*p) != *pattern)
            return NULL;
        p++;
        if (*pattern == ',') /* Allow spaces after comma */
            p = avoid_spaces(p);
        pattern++;
    }

    /*
     ** Instruction properly matched, now generate binary
     */
    base = decode;
    while (*decode)
    {
        decode = avoid_spaces(decode);
        if (decode[0] == 'x')
        { /* Byte */
            c = toupper(decode[1]);
            c -= '0';
            if (c > 9)
                c -= 7;
            d = toupper(decode[2]);
            d -= '0';
            if (d > 9)
                d -= 7;
            c = (c << 4) | d;
            emit_byte(c);
            decode += 3;
        }
        else
        { /* Binary */
            if (*decode == 'b')
                decode++;
            bit = 0;
            c = 0;
            d = 0;
            while (bit < 8)
            {
                if (decode[0] == '0')
                { /* Zero */
                    decode++;
                    bit++;
                }
                else if (decode[0] == '1')
                { /* One */
                    c |= 0x80 >> bit;
                    decode++;
                    bit++;
                }
                else if (decode[0] == '%')
                { /* Special */
                    decode++;
                    if (decode[0] == 'r')
                    { /* Register field */
                        decode++;
                        if (decode[0] == '8')
                            decode++;
                        else if (decode[0] == '1' && decode[1] == '6')
                            decode += 2;
                        c |= instruction_register << (5 - bit);
                        bit += 3;
                    }
                    else if (decode[0] == 'd')
                    { /* Addressing field */
                        if (decode[1] == '8')
                            decode += 2;
                        else
                            decode += 3;
                        if (bit == 0)
                        {
                            c |= instruction_addressing & 0xc0;
                            bit += 2;
                        }
                        else
                        {
                            c |= instruction_addressing & 0x07;
                            bit += 3;
                            d = 1;
                        }
                    }
                    else if (decode[0] == 'i' || decode[0] == 's')
                    {
                        if (decode[1] == '8')
                        {
                            decode += 2;
                            c = instruction_value;
                            break;
                        }
                        else
                        {
                            decode += 3;
                            c = instruction_value;
                            instruction_offset = instruction_value >> 8;
                            instruction_offset_width = 1;
                            d = 1;
                            break;
                        }
                    }
                    else if (decode[0] == 'a')
                    {
                        if (decode[1] == '8')
                        {
                            decode += 2;
                            c = instruction_value - (address + 1);
                            if (assembler_step == 1 && (c < -128 || c > 127))

                            {
                                printf("c = %d l = %d a = %d v = %d\n", c, line_number, address, instruction_value);
                                message("short jump too long ", ERR, line_number);
                            }
                            break;
                        }
                        else
                        {
                            decode += 3;
                            c = instruction_value - (address + 2);
                            instruction_offset = c >> 8;
                            instruction_offset_width = 1;
                            d = 1;
                            break;
                        }
                    }
                    else if (decode[0] == 'f')
                    {
                        decode += 3;
                        emit_byte(instruction_value);
                        c = instruction_value >> 8;
                        instruction_offset = instruction_value2;
                        instruction_offset_width = 2;
                        d = 1;
                        break;
                    }
                    else
                    {
                        printf("decode: internal error 2\n");
                    }
                }
                else
                {
                    printf("decode: internal error 1 (%s)\n", base);
                    break;
                }
            }
            emit_byte(c);
            if (d == 1)
            {
                d = 0;
                if (instruction_offset_width >= 1)
                {
                    emit_byte(instruction_offset);
                }
                if (instruction_offset_width >= 2)
                {
                    emit_byte(instruction_offset >> 8);
                }
            }
        }
    }
    if (assembler_step == 2)
    {
        if (undefined)
        {
            printf("Error: undefined label '%s' at line %d\n", undefined_name, line_number);
        }
    }
    return p;
}
void process_instr()
{
    char *p2 = NULL;
    char *p3;
    int c;

    //  while (part[0]) {   /* Match against instruction set */
    c = 0;
    if (strcmp(part, "DB") == 0)
    { /* Define byte */
        while (1)
        {
            p = avoid_spaces(p);
            if (*p == '"')
            { /* ASCII text */
                p++;
                while (*p && *p != '"')
                {
                    p = read_character(p, &c);
                    emit_byte(c);
                }
                if (*p)
                {
                    p++;
                }
                else
                {

                    printf("Error: unterminated string at line %d\n", line_number);
                }
            }
            else
            {
                p2 = match_expression(p, &instruction_value);
                if (p2 == NULL)
                {
                    printf("Error: bad expression at line %d\n", line_number);
                    break;
                }
                emit_byte(instruction_value);

                size++;
                p = p2;
            }
            p = avoid_spaces(p);
            if (*p == ',')
            {
                p++;
                continue;
            }
            check_end(p);
            break;
        }
        return;
    }
    if (strcmp(part, "DW") == 0)
    { /* Define word */
        while (1)
        {
            p2 = match_expression(p, &instruction_value);
            if (p2 == NULL)
            {
                printf("Error: bad expression at line %d\n", line_number);
                break;
            }
            // emit_byte(-1);
            emit_byte(instruction_value & 0xff);
            emit_byte(instruction_value >> 8);

            if (!assembler_step && variable_list != NULL)
                variable_list->end_address = address;
            p = avoid_spaces(p2);
            if (*p == ',')
            {
                p++;
                continue;
            }
            check_end(p);
            break;
        }
        return;
    }
    while (part[0])
    { /* Match against instruction set */
        c = 0;
        while (instruction_set[c] != NULL)
        {
            if (strcmp(part, instruction_set[c]) == 0)
            {
                p2 = instruction_set[c];
                while (*p2++)
                    ;
                p3 = p2;
                while (*p3++)
                    ;

                p2 = match(p, p2, p3);
                if (p2 != NULL)
                {

                    p = p2;
                    break;
                }
            }
            c++;
        }
        if (instruction_set[c] == NULL)
        {
            char m[MAX_SIZE + 31];

            sprintf(m, "Undefined instruction '%s %s'\n", part, p);
            message(m, ERR, line_number);

            break;
        }
        else
        {
            p = p2;
            seperate();
        }
    }
    //  }
}

void do_assembly(struct emu8086 *aCPU, char *fname)
{
    FILE *input = fopen(fname, "r");
    if (input == NULL)
    {
        _message("cant fin fil", ERR);
    }
    char *p2;
    int i = 0;
    int starting_address = 0;
    _INSTRUCTIONS = NULL;
    starting_address = 0x7F000;
    address = 0;
    data_mem_offset = 0;
    CS = 0x7F00;
    is_first = 1;
    line_number = 0;
    aCPU->code_start_addr = starting_address;
    while (fgets(line, sizeof(line), input))
    {

        line_number += 1;

        i++;
        p = line;
        while (*p)
        {
            if (*p == '\'' && *(p - 1) != '\\')
            {
                p++;
                while (*p && *p != '\'' && *(p - 1) != '\\')
                    p++;
            }
            else if (*p == '"' && *(p - 1) != '\\')
            {
                p++;
                while (*p && *p != '"' && *(p - 1) != '\\')
                    p++;
            }
            else if (*p == ';')
            {
                while (*p)
                    p++;
                break;
            }
            *p = toupper(*p);
            p++;
        }
        if (p > line && *(p - 1) == '\n')
            p--;
        *p = '\0';

        while (1)
        {
            p = line;
            seperate();

            if (part[0] != '\0' && part[strlen(part) - 1] == ':')
            { /* Label */
                part[strlen(part) - 1] = '\0';
                //   assembler_step = 0;
                if (part[0] == '.')
                {
                    strcpy(name, global_label);
                    strcat(name, part);
                }
                else
                {
                    strcpy(name, part);
                    strcpy(global_label, name);
                }
                seperate();

                if (strcmp(part, "EQU") == 0)
                {
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                    {
                        _message("bad expression", ERR);
                        //   exit(1);
                    }
                    else
                    {

                        struct label *_nu = find_label(name);

                        if (_nu)
                        {
                            char m[18 + MAX_SIZE];

                            sprintf(m, "Redefined label '%s'", name);
                            _message(m, WARN);
                            _nu->is_defined = 1;
                            // exit(1);
                        }
                        else
                        {
                            last_label = define_label(name, instruction_value);
                            last_label->is_defined = 1;
                        }

                        check_end(p2);
                    }
                    break;
                }
                struct label *nu = find_label(name);

                if (nu != NULL && assembler_step == 0)
                {
                    char m[30 + MAX_SIZE];

                    sprintf(m, "Warning Redefined label '%s'", name);

                    _message(m, WARN);
                    nu->value = address;
                    if (!_check__end(p))
                        nu->line_number = line_number;
                    else
                    {
                        _current_label = nu;
                        break;
                    }
                    nu->is_defined = 1;
                    nu->is_addr = 1;
                    // exit(1);
                }
                else if (assembler_step == 0)
                {
                    last_label = define_label(name, address);
                    last_label->is_addr = 1;
                    last_label->is_defined = 1;
                    last_label->value = address;
                }
            }

            if (part[0])
            {

                if (!assembler_step)
                    _current_instruction = define_instruction(line_number);
                if (!assembler_step)
                    _current_instruction->starting_address = address;

                process_instr();
                if (!assembler_step)
                    _current_instruction->end_address = address;
                if (!assembler_step)
                {
                    if (is_first)
                    {
                        _instruction_list = _current_instruction;
                        is_first = 0;
                    }
                    else
                    {
                        _instruction_list->next = _current_instruction;
                        _current_instruction->prev = _instruction_list;
                        _instruction_list = _current_instruction;
                    }
                }

                // _current_instruction = _current_instruction->next;
            }
            break;
        }
        // has_label = 0;
    }
    if (assembler_step)
    {
        unsigned char *h;
        h = CODE_SEGMENT;

        int end = address + 1, start = 0;
        // if (data_mem_offset > 0)
        // {
        //     check_for_vars(aCPU);
        // }
        // data_mem_offset = data_mem_offset > 0 ? data_mem_offset : 0;
        // end = end - data_mem_offset;
        while (start < end)
        {

            *(h + start) = *(code_mem + data_mem_offset + start);

#ifdef DEBUG
            printf("%03x: %02x\n", (CS * 0x10) + start, *((CODE_SEGMENT) + start));
#endif
            start++;
        }
        aCPU->instructions_list = _first_instruction;
        aCPU->end_address = end;
    }

    fclose(input);
}

/* 
   end assm.c
*/
