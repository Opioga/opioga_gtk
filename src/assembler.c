/*
*
* Copyright (c) 2019 Oscar Toledo G. http://nanochess.org/
* All rights reserved.
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "emu8086.h"
#include "ins.h"
#include "assembler.h"

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
unsigned char *code_mem = NULL;
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

int line_number, size, data_mem_offset;

struct errors_list *define_errors_list(char *name, int line)
{
    struct errors_list *error;
    error = (struct errors_list *)malloc(sizeof(struct errors_list) + strlen(name));
    if (error == NULL)
    {
        // printf("Out of memory for label\n");
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
struct label *define_label(char *name,
                           int value)
{
    struct label *label;
    struct label *explore;
    int c;

    /* Allocate label */
    label = (struct label *)malloc(sizeof(struct label) + strlen(name));
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
struct label *find_label(char *name)
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

struct variable *define_variable(char *name,
                                 int value)
{
    size = 0;
    struct variable *variable;
    //   struct variable *explore;
    // int c;

    /* Allocate variable */
    variable = (struct variable *)malloc(sizeof(struct variable) + strlen(name));
    if (variable == NULL)
    {
        printf("Out of memory for variable\n");
        exit(1);
        return NULL;
    }
    // printf("%s\n", name);
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
struct variable *find_variable(char *name)
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
#ifdef DEBUG
    fprintf(stdout, m);
#endif
};

/*
 ** Avoid spaces in input
 */
char *avoid_spaces(char *p)

{
    while (isspace(*p))
        p++;
    return p;
}

void separate()
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
    struct instruction *_new = (struct instruction *)malloc(sizeof(struct instruction));
    _new->line_number = line_number;
    _new->next = NULL;
    _new->prev = NULL;
    _new->cache = NULL;
    if (is_first)
    {
        _first_instruction = _new;
    }
    ic++;
    return _new;
}

void emit_byte(int c)
{
    // printf("%03x: %02x\n", address, c);
    if (assembler_step == 2)
        *(code_mem + address) = c;
    address += 1;
}

int _check__end(char *p)
{

    int b = 0;
    p = avoid_spaces(p);
    b = !(*p);
    return b;
}
void check_end(char *p)
{
    p = avoid_spaces(p);
    if (*p && *p != ';')
    {
        char buf[45];
        sprintf(buf, "Error: extra characters at end of line %d", line_number);
        message(buf, ERR, line_number);
        errors++;
    }
}
/*
 ** Read character for string or character literal
 */
char *read_character(char *p, int *c)
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
char *match_expression(char *p, int *value)
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
char *match_expression_level1(char *p, int *value)
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
char *match_expression_level2(char *p, int *value)
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
char *match_expression_level3(char *p, int *value)
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
char *match_expression_level4(char *p, int *value)
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
char *match_expression_level5(char *p, int *value)
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
char *match_expression_level6(char *p, int *value)
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
    if (isdigit(*p) || isxdigit(*p))
    { /* Decimal */
        number = 0;
        char buf[15];
        char *p4 = buf;
        char *former_p = p;
        int isLabel = 0;
        while (isalpha(*p) || *p == '_' || *p == '.' || isdigit(*p))
        {
            *p4 = p[0];
            // c = p[0] - '0';

            // number = number * 10 + c;
            p++;
            p4++;
            // if (isalpha(p4[0]))
            //     isLabel = 1;
        }

        *p4 = '\0';
        p4--;

        if (*p4 == 'H' && (isxdigit(*(p4 - 1)) || isdigit(*(p4 - 1))))
        {

            // p++;
            number = 0;
            p4 = buf;
            while (isxdigit(p4[0]) || isdigit(p4[0]))
            {
                c = toupper(p4[0]);
                c = c - '0';
                if (c > 9)
                    c -= 7;
                number = (number << 4) | c;
                p4++;
            }
        }
        else if (*p4 == 'O' && isdigit(*(p4 - 1)))
        {
            // p++;
            number = 0;
            p4 = buf;
            while (isdigit(p4[0]))
            {
                c = p4[0];
                c = c - '0';
                number = (number << 3) | c;
                p4++;
            }
        }

        else if (*p4 == 'B' && isdigit(*(p4 - 1)))
        {

            *p4 = '\0';
            number = 0;
            p4 = buf;
            while (p4[0] == '0' || p4[0] == '1' || p[0] == '_')
            {
                if (p4[0] != '_')
                {
                    number <<= 1;
                    if (p4[0] == '1')
                        number |= 1;
                }
                p4++;
            }

            if (isalpha(p4[0]))
                return NULL;
        }
        else if (isdigit(former_p[0]))
        {
            p4 = buf;
            number = 0;

            // if(buf)
            while (isdigit(p4[0]))
            {
                c = p4[0] - '0';
                number = number * 10 + c;
                //p++;
                p4++;
            }
            if (isalpha(p4[0]))
                return NULL;
            //
        }
        else
        {
            isLabel = 1;
        }
        p = avoid_spaces(p);
        if (!isLabel)
        {
            *value = number;
            return p;
        }
        else
        {
            p = former_p;
            if (isdigit(*p))
                return NULL;
        }
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
        if (strlen(expr_name) > 0 && strcmp(expr_name, "OFFSET") == 0)
        {
            p = avoid_spaces(p);
            char *check;
            check = p;
            p2 = expr_name;
            while (isalpha(*check) || isdigit(*check) || *check == '_' || *check == '.')
                *p2++ = *check++;
            *p2 = '\0';
            if (strlen(p) == 0 || isdigit(p[0]))
            {
                return NULL;
            }
            if (strcmp(expr_name, "OFFSET") == 0)
                return NULL;
            return match_expression_level6(p, value);
        }

        for (c = 0; c < 16; c++)
            if (strcmp(expr_name, reg1[c]) == 0)
                return NULL;

        label = find_label(expr_name);
        if (label == NULL)
        {
            *value = 0;
            undefined++;
            strcpy(undefined_name, expr_name);
            if (assembler_step)
            {

                if ((!strcmp(expr_name, "WORD") == 0))
                {
                    char m[25 + MAX_SIZE];
                    sprintf(m, "Undefined label '%s' on line %d", expr_name, line_number);
                    message(m, ERR, line_number);
                }
            }
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

char *match_register(char *p, int width, int *value)
{
    char reg[3];
    int c;

    p = avoid_spaces(p);
    if (!isalpha(p[0]) || !isalpha(p[1]))
    {   

        return NULL;
    }

    if (p[2] && (!isspace(p[2]) && p[2] != ',' &&p[2] !=']' && p[2] != '+' && p[2] != '-'))
    {
     

        return NULL;
    }
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
            return avoid_spaces(p + 2);
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
            return avoid_spaces(p + 2);
        }
    }
    //printf("%s\n", p);
    return NULL;
}

char *match_addressing(char *p, int width)
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
                        //printf("%d", instruction_value);

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
                                // printf("c = %d l = %d a = %d v = %d", c, line_number, address, instruction_value);
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
            printf("Error: undefined label '%s' at line %d", undefined_name, line_number);
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

                    printf("Error: unterminated string at line %d", line_number);
                }
            }
            else
            {
                p2 = match_expression(p, &instruction_value);
                if (p2 == NULL)
                {
                    printf("Error: bad expression at line %d", line_number);
                    break;
                }

                p = p2;
                p = avoid_spaces(p);
                char buf[4];
                strncpy(buf, p, 3);
                buf[3] = '\0';
                if (strcmp(buf, "DUP") == 0)
                {
                    p += 3;
                    p = avoid_spaces(p);
                    char *ip = p;
                    if (*p != '(')
                    {
                        char m[100];
                        sprintf(m, "Bad expression  '%s' on line %d", ip, line_number);
                        message(m, ERR, line_number);
                        break;
                    }
                    p++;
                    p = avoid_spaces(p);

                    if (*p && *p == '\'')
                    {
                        p++;
                        p = read_character(p, &c);

                        if (!*p || *p != '\'')
                        {
                            char m[100];
                            sprintf(m, "Extra Characters in char literal '%s %s' on line %d", part, ip, line_number);
                            message(m, ERR, line_number);
                            break;
                        }
                        p++;

                        while (instruction_value > 0)
                        {
                            emit_byte(c);
                            instruction_value--;
                        }
                        //  continue;
                    }
                    else if (*p && *p == '"')
                    {
                        p++;
                        char *lp = p;
                        // printf("%s\n", p);

                        while (instruction_value > 0)
                        {
                            p = lp;
                            while (*p && *p != '"')
                            {
                                p = read_character(p, &c);
                                emit_byte(c);
                            }
                            instruction_value--;
                        }
                        if (!*p || *p != '"')
                        {
                            char m[100];
                            sprintf(m, "Unterminated String '%s %s' on line %d", part, ip, line_number);
                            message(m, ERR, line_number);
                            break;
                        }
                        p = avoid_spaces(p);
                        p++;

                        // printf("%s\n", p);

                        // exit(1);
                    }

                    else if (*p && *p == '?')
                    {
                        p++;
                        // printf("p: %s\n", p);

                        // exit(1);

                        while (instruction_value > 0)
                        {
                            emit_byte(0);
                            instruction_value--;
                        }
                    }
                    else if (*p)
                    {
                        int v = instruction_value;
                        // printf("pen: %s\n", p);

                        p2 = match_expression_level6(p, &instruction_value);
                        while (v > 0)
                        {
                            emit_byte(instruction_value);
                            v--;
                        }
                        p = p2;

                        // exit(1);
                    }
                    else
                    {
                        char m[100];
                        sprintf(m, "Bad expression  '%s' on line %d", p, line_number);
                        message(m, ERR, line_number);
                        break;
                    }
                    p = avoid_spaces(p);
                    // p++;
                    if (*p && *p == ')')
                        p++;
                    else
                    {
                        char m[100];
                        sprintf(m, "Bad expression  '%s %s' on line %d", part, ip, line_number);
                        message(m, ERR, line_number);
                        break;
                    }
                }
                else
                    emit_byte(instruction_value);
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
                printf("Error: bad expression at line %d", line_number);
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

            sprintf(m, "Undefined instruction '%s %s' on line %d", part, p, line_number);
            message(m, ERR, line_number);

            break;
        }
        else
        {

            p = p2;

            separate();
        }
    }
    //  }
}
void reset_address()
{
    address = start_address;
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
    int starting_address = 0, first_time = 0;
    _INSTRUCTIONS = NULL;
    starting_address = start_address = 0;
    address = 0;
    data_mem_offset = 0;
    CS = 0;
    is_first = 1;
    line_number = 0;
    first_time = 1;
    code_mem = aCPU->mDataMem;
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

            separate();
            if (part[0] == '\0' && (*p == '\0' || *p == ';')) /* Empty line */
                break;

            if (part[0] != '\0' && part[strlen(part) - 1] == ':')
            { /* Label */
                part[strlen(part) - 1] = '\0';
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
                separate();
                // if (avoid_level == -1 || level < avoid_level) {
                if (strcmp(part, "EQU") == 0)
                {
                    p2 = match_expression(p, &instruction_value);
                    if (p2 == NULL)
                    {
                        message("bad expression\n", ERR, line_number);
                    }
                    else
                    {
                        if (assembler_step == 0)
                        {
                            if (find_label(name))
                            {
                                char m[18 + MAX_SIZE];

                                sprintf(m, "Redefined label '%s'", name);
                                message(m, ERR, line_number);
                            }
                            else
                            {
                                last_label = define_label(name, instruction_value);
                            }
                        }
                        else
                        {
                            last_label = find_label(name);
                            if (last_label == NULL)
                            {
                                char m[33 + MAX_SIZE];

                                sprintf(m, "Inconsistency, label '%s' not found", name);
                                message(m, ERR, line_number);
                            }
                            else
                            {
                                if (last_label->value != instruction_value)
                                {

                                    int change = 1;
                                }
                                last_label->value = instruction_value;
                            }
                        }
                        check_end(p2);
                    }
                    break;
                }
                if (first_time == 1)
                {

                    first_time = 0;
                    reset_address();
                }
                if (assembler_step == 0)
                {

                    if (find_label(name))
                    {
                        char m[18 + MAX_SIZE];

                        sprintf(m, "Redefined label '%s'", name);
                        message(m, ERR, line_number);
                    }
                    else
                    {
                        last_label = define_label(name, address);
                    }
                }
                else
                {
                    last_label = find_label(name);
                    if (last_label == NULL)
                    {
                        char m[33 + MAX_SIZE];

                        sprintf(m, "Inconsistency, label '%s' not found", name);
                        message(m, ERR, line_number);
                    }
                    else
                    {
                        if (last_label->value != address)
                        {
#ifdef DEBUG
/*                                fprintf(stderr, "Woops: label '%s' changed value from %04x to %04x\n", last_label->name, last_label->value, address);*/
#endif
                            int change = 1;
                        }
                        last_label->value = address;
                    }
                }
            }
            if (strcmp(part, "ORG") == 0)
            {
                p = avoid_spaces(p);
                undefined = 0;
                p2 = match_expression(p, &instruction_value);
                if (p2 == NULL)
                {

                    char buf[50];
                    sprintf(buf, "Bad expression on line %d", line_number);
                    message(buf, ERR, line_number);
                }
                else if (undefined)
                {
                    char buf[150];
                    sprintf("Cannot use undefined labels %s on line %d", last_label->name, line_number);
                    message(buf, ERR, line_number);
                    // message(1, "");
                }
                else
                {
                    if (first_time == 1)
                    {
                        first_time = 0;
                        address = instruction_value;
                        start_address = instruction_value;
                        starting_address = aCPU->code_start_addr = address;

                        CS = address / 0x10;
                    }
                    else
                    {
                        if (instruction_value < address)
                        {
                            char buf[50];
                            sprintf("Backward Address on line %d", line_number);
                            message(buf, ERR, line_number);
                        }
                        else
                        {
                            while (address < instruction_value)
                                emit_byte(0);
                        }
                    }
                    check_end(p2);
                }
                break;
            }
            if (first_time == 1)
            {

                first_time = 0;
                reset_address();
            }
            if (part[0])
            {
                if (strcmp(part, "DB") == 0 || strcmp(part, "DW") == 0)
                {
                    process_instr();
                }

                else
                {
                    if (assembler_step == 2)
                        _current_instruction = define_instruction(line_number);
                    if (assembler_step == 2)
                        _current_instruction->starting_address = address - starting_address;

                    process_instr();
                    if (assembler_step == 2)
                        _current_instruction->end_address = address - starting_address;
                    if (assembler_step == 2)
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
                }
                // _current_instruction = _current_instruction->next;
            }
            break;
        }
        // has_label = 0;
    }
    if (assembler_step == 2)
    {
        unsigned char *h;
        h = CODE_SEGMENT;

        int end = address + 1, start = start_address;
        // if (data_mem_offset > 0)
        // {
        //     check_for_vars(aCPU);
        // }
        int vn = 0;
        // data_mem_offset = data_mem_offset > 0 ? data_mem_offset : 0;
#ifdef DEBUG // end = end - data_mem_offset;
        while (start < end)
        {

            // *(h + start) = *(code_mem + data_mem_offset + start);

            printf("%03x: %02x\n", (CS * 0x10) + vn, *(code_mem + start));
            vn++;

            start++;
        }
#endif
        aCPU->instructions_list = _first_instruction;
        aCPU->end_address = end;
    }

    fclose(input);
}

/* 
   end assm.c
*/
