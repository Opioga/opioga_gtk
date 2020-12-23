#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"
#include <emu8086.h>
#include "ins.h"
#include "opcodes.h"
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
        fprintf(stderr, "Out of memory for label\n");
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
    if (line_number == 308 && assembler_step)
        printf("%s\n", name);
    /* Follows a binary tree */
    explore = label_list;

    while (explore != NULL)
    {
        printf("%d %d\n", strlen(name), strlen(explore->name));
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
    int c;

    /* Allocate variable */
    variable = malloc(sizeof(struct variable) + strlen(name));
    if (variable == NULL)
    {
        fprintf(stderr, "Out of memory for variable\n");
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

void _message(char *m, int level)
{
    if (level == ERR)
    {
        fprintf(stderr, m);
        exit(EXIT_FAILURE);
    }
    else if (level == WARN)
    {
        fprintf(stderr, m);
    }
#ifdef DEBUG
    else
        fprintf(stdout, m);
#endif
}

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

    if (is_first)
    {
        _first_instruction = new;
    }
    return new;
}

void emit_byte(char c)
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
        fprintf(stderr, "Error: extra characters at end of line %d\n", line_number);
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
            _message("bad escape inside string\n", ERR);
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

                _message("division by zero", WARN);

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
                    _message("division by zero", WARN);
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
    struct label *label;

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
            _message("Missing apostrophe", ERR);
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
        }
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
    if (p[3] && isalpha(p[3] && p[3] != ','))
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
                if (assembler_step)
                {
                    p = p;
                    int d = 0;
                }
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

                    else
                        p = p;
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
    if (line_number == 308 && assembler_step)
        base = decode;
    else
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
                                _message("short jump too long", ERR);

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
                        fprintf(stderr, "decode: internal error 2\n");
                    }
                }
                else
                {
                    fprintf(stderr, "decode: internal error 1 (%s)\n", base);
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
            fprintf(stderr, "Error: undefined label '%s' at line %d\n", undefined_name, line_number);
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
                    fprintf(stderr, "Error: unterminated string at line %d\n", line_number);
                }
            }
            else
            {
                p2 = match_expression(p, &instruction_value);
                if (p2 == NULL)
                {
                    fprintf(stderr, "Error: bad expression at line %d\n", line_number);
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
                fprintf(stderr, "Error: bad expression at line %d\n", line_number);
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
            char m[25 + MAX_SIZE];

            sprintf(m, "Undefined instruction '%s %s'\n", part, p);
            _message(m, ERR);

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

int get_variables(char *part)
{
    if (part[0])
    {
        char *pn = part;
        if (pn[0] == '.')
        {
            pn++;
            if (pn[0])
            {
                if (!assembler_step)
                    define_variable(pn, address);
                return 1;
            }
            else
            {
                _message("Erroor", ERR);
                return 0;
            }
        }
    }
    return 0;
}

char *check_for_offset()
{
    char line2[256], offs[7], va_arg[20];
    char *d = line;
    int i = 0;
    if (*d == '.')
        return line;
    struct variable *v;
    while (*d)
    {
        if (*d == 'O' && d[1] == 'F')
        {
            strncpy(offs, d, 6);
            int b = strcmp(offs, "OFFSET");
            // printf("%d %s\n", b, offs);

            if (b == 0)
            {
                int bo = 0;
                d += 6;
                d = avoid_spaces(d);
                if (!*d || *d == ';')
                {
                    exit(1);
                }
                while (*d && !isspace(*d))
                {
                    va_arg[bo] = *d++;
                    bo++;
                }
                if (*d)
                {
                    printf("err \n");
                    exit(1);
                }
                va_arg[bo] = '\0';
                line2[i] = '\0';
                printf("%s\n", va_arg);
                v = find_variable(va_arg);
                if (v != NULL)
                {
                    char bu[5];
                    sprintf(bu, "0x%04x", v->offset);
                    char *ff = strcat(line2, bu);
                    printf("%s\n", ff);
                    return ff;
                }
                else
                {
                    printf("err \n");
                    exit(1);
                }
            }
            return line;
        }
        line2[i] = *d;
        i++;
        d++;
    }
    return line;
}

int check_for_vars(struct emu8086 *aCPU)
{
    int addr = 0;
    struct variable *explore = first_variable;
    while (explore != NULL)
    {
        int i = 0;
        while (i < explore->size)
        {
            aCPU->mDataMem[0x3ff0 + addr] = code_mem[addr];
            i++;
            addr++;
        }
        explore = explore->right;
    }
    return 1;
}

void do_assembly(struct emu8086 *aCPU, char *fname)
{
    FILE *input = fopen(fname, "r");
    if (input == NULL)
    {
        _message("cant fin fil", ERR);
    }
    char *p2, *p3;
    int i = 0;
    int starting_address = 0;
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
            if (line_number == 304)
            {
                int bbb = 7;
            }

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

                            sprintf(m, "Redefined label '%s\n'", name);
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
                    char m[18 + MAX_SIZE];

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
        printf("\n\n");
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
            printf("%03x: %02x\n", (CS * 0x10) + start, *((CODE_SEGMENT) + start));
            start++;
        }
        aCPU->instructions_list = _first_instruction;
        aCPU->end_address = end;
    }

    fclose(input);
}

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
    op_setptrs(new);
    //new->sfrread((struct struct emu8086 *)new, REG_DS);
    return new;
}
/*end emu.c */

// ops.c
int is_16 = 0;
extern void _message(char *m, int level);
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

void find_instruction(struct emu8086 *aCPU)
{
    if (_INSTRUCTIONS == NULL)
    {
    }

    char off = *(CODE_SEGMENT + IP);
    // off = off < 0 ? 0 - off : off;
    __uint16_t add = 0;
    // char is_back = off >> 7;
    _current_instruction = _INSTRUCTIONS;
    struct instruction *prev = _current_instruction->prev;
    struct instruction *next = _current_instruction->next;

    if (_current_instruction->starting_address == *(CODE_SEGMENT + IP))
    {

        IP = *(CODE_SEGMENT_IP);
        return;
    }
    if (off >= 0)
        prev = NULL;
    else
        next = NULL;
    int b = IP + off + 1;
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
            sprintf(buf, "Undefined label: on line %d",
                    _current_instruction->line_number);
            _message(buf, ERR);
        }
    }
    // IP++;
    IP = add;
    _INSTRUCTIONS = _current_instruction;
    // if ()
}

void find_instruction_call(struct emu8086 *aCPU)
{
    IP++;
    short value = *(CODE_SEGMENT_IP);
    __uint16_t add = 0;

    IP++;
    value |= *(CODE_SEGMENT_IP) << 8;

    _current_instruction = _INSTRUCTIONS;
    struct instruction *prev = _current_instruction->prev;
    struct instruction *next = _current_instruction->next;
    aCPU->instruction_cache = next;
    int _next = IP + 1;
    if (value >= 0)
        prev = NULL;
    else
        next = NULL;
    int b = IP + value + 1;
    if (SP == 0)
    {
        char buf[15];
        sprintf(buf, "Stack reached maximum: on line %d",
                _current_instruction->line_number);
        _message(buf, ERR);
    }
    if (b == 0xfffd)
    {
        IP = _current_instruction->starting_address;

        push_to_stack(aCPU, _next);

        return;
    }
    if (_current_instruction->starting_address == b)
    {
        add = b;
    }
    else
    {
        while (1)
        {
            if (prev != NULL)
            {

                if (prev->starting_address == b)
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
                if (next->starting_address == b)
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
                char buf[15];
                sprintf(buf, "Undefined label: on line %d",
                        _current_instruction->line_number);
                _message(buf, ERR);
            }
        }
    }
    // IP++;
    IP = add;

    if (_next < aCPU->end_address - 1)
        push_to_stack(aCPU, _next);
    //  }
    else
        IP = aCPU->end_address;
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
            *dest = DATA_SEGMENT + SI;
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
        FLAGS |= ((1 << 11) | (1 << 7));

    else
    {
        CLEAR_FLAG(11);
        CLEAR_FLAG(7);
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
    int b = 0;
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
        setFlags(aCPU, value);
        *op2 = value & 0xffff;

        *handled = 1;
        IP++;
    }

    //  IP += 4;
    return;
}

void add_reg8_addr8(struct emu8086 *aCPU, int *handled)
{
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
    b = get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

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

    int op2, value, *op4;

    unsigned char reg = (opn & 0b00111000) >> 3;

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
    else if (get_ops_reg_8(aCPU, opn, &op3, &op4))
    {
        // opn = opn - 0x20;
        value = 0;
        int width = _INSTRUCTIONS->end_address - IP - 1;
        IP++;
        value = *(CODE_SEGMENT_IP);

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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
            value = (*op2 & 0xff) | (*op3 & 0xff);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) | (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
        {
            value = (*op2 >> 8) | (*op3 & 0xff);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else
        {
            value = (*op2 & 0xff) | (*op3 >> 8);
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

    else if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
            value = (*op2 & 0xff) + (*op3 & 0xff) + (FLAGS & 1);
            *op2 = (*op2 & 0xff00) | (value & 0xff);
        }
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) + (*op3 & 0xff) + (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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
    b = get_ops_reg_8_addr(aCPU, opn, &op2, &op1);

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
    int offset = 0, *op3;

    unsigned char *op1, opn;
    opn = *(CODE_SEGMENT + IP);

    is_16 = 0;

    int op2, value, *op4;
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
    int *op2, value = 0, *op3;
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
    int *op2, value = 0, *op3;
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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 >> 8) - (FLAGS & 1);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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

    int op2, value;
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

    int op2, value;
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

    if (!(CF && ZF))
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
    if (CX < 1)
    {
        *handled = 1;
        aCPU->instruction_cache = NULL;
        IP++;
        return;
    }
    if (aCPU->instruction_cache == NULL)
    {
        find_instruction(aCPU);
        aCPU->instruction_cache = _INSTRUCTIONS;
    }
    else
    {
        IP = aCPU->instruction_cache->starting_address;
        _INSTRUCTIONS = aCPU->instruction_cache;
    }
    CX--;

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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) & (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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

    int op2, value;
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

    int op2, value;
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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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

    int op2, value;
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

    int op2, value;
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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) ^ (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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

    int op2, value;
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

    int op2, value;
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
    unsigned char *op1;
    int *op2, *op3;
    is_16 = 1;
    IP++;
    int opn = *(CODE_SEGMENT_IP);
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
        else if (high_reg == 1)
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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
    int value = *(CODE_SEGMENT + IP);
    IP++;
    int *op1 = aCPU->mSFR + (reg_offset < 4 ? reg_offset : reg_offset - 4);
    if (reg_offset < 4)
        *op1 = (*op1 & 0xff00) | (value & 0xff);
    else
        *op1 = (*op1 & 0xff) | ((value & 0xff) << 8);

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
        else if (high_reg == 1)
        {
            value = (*op3 >> 8);
            *op2 = (*op2 & 0xff) | ((value & 0xff) << 8);
        }
        else if (high_reg == 2)
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
    int b = 0;
    is_16 = 0;
    unsigned char *op1;
    int *op2, *op3;
    IP++;
    unsigned char opn = *(CODE_SEGMENT_IP);

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
    int value = 0, *op2 = NULL, *op3 = NULL;

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
    unsigned char offset = 0;
    unsigned char *op1;
    int value = 0, *op2 = NULL;
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }
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
    unsigned char offset = 0;
    unsigned char *op1;
    int value = 0, *op2 = NULL;
    if (get_ops_reg_8_addr(aCPU, opn, &op2, &op1))
    {

        offset = 0;
    }
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
    int *op1 = NULL;
    op1 = SFRS + reg_offset;
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
    find_instruction_call(aCPU);
    *handled = 1;
    aCPU->skip_next = 1;
    return;
}

void ret_addr(struct emu8086 *aCPU, int *handled)
{
    unsigned int value;
    pop_from_stack(aCPU, &value);
    _INSTRUCTIONS = aCPU->instruction_cache;
    aCPU->instruction_cache = NULL;
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
    int b = 0;
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
    int b = 0;
    is_16 = 0;

    IP++;
    unsigned char *op1;
    int *op2, *op3;
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
        else if (high_reg == 1)
        {
            value = (*op2 >> 8) - (*op3 >> 8);
        }
        else if (high_reg == 2)
        {
            value = (*op2 >> 8) - (*op3 & 0xff);
        }
        else
        {
            value = (*op2 & 0xff) - (*op3 >> 8);
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

    int op2, value;
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
    int op2, value;
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
    int op2, value;
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
    int reg_offset = *(CODE_SEGMENT_IP)-0x90;
    if (reg_offset == 0)
    {
        IP++;
        *handled = 1;
        return;
    }
    int *op = SFRS + reg_offset, value1, value2;
    value1 = AX;
    value2 = *op;
    AX = value2;
    *op = value1;
    IP++;
    *handled = 1;
    return;
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
    unsigned char opn = *(CODE_SEGMENT_IP);
    int *op1, *op2, b;
    unsigned char *op3;
    is_16 = 0;
    b = get_ops_reg_8(aCPU, opn, &op1, &op2);
    if (b)
    {
        int value, value2;

        if (high_reg == 0)
        {
            value = *op1 & 0xff;
            value2 = *op2 & 0xff;

            *op1 = (*op1 & 0xff00) | value2;
            *op2 = (*op2 & 0xff00) | value;
        }
        else if (high_reg == 1)
        {
            value = *op1 >> 8;
            value2 = *op2 >> 8;

            *op1 = (*op1 & 0xff) | (value2 << 8);
            *op2 = (*op2 & 0xff) | (value << 8);
        }
        else if (high_reg == 2)
        {
            value = *op1 >> 8;
            value2 = *op2 & 0xff;

            *op1 = (*op1 & 0xff) | (value2 << 8);
            *op2 = (*op2 & 0xff00) | (value);
        }
        else
        {
            value2 = *op2 >> 8;
            value = *op1 & 0xff;

            *op2 = (*op2 & 0xff) | (value << 8);
            *op1 = (*op1 & 0xff00) | (value2);
        }

        IP += 1;
        *handled = 1;
        is_16 = 0;
    }
    opn = opn > 0x79 ? opn - 0x40 : opn;
    if (get_ops_reg_8_addr(aCPU, opn, &op3, &op1))
    {
        int value = high_reg ? ((*op1 & 0xff00) >> 8) : (*op1 & 0xff);
        //  IP++;
        //  value += *(op1);
        int value2 = *op3;
        *op1 = high_reg ? ((*op1 & 0xff) | (value2 << 8))
                        : ((*op1 & 0xff00) | value2);
        // *(op1) = value & 0xff;
        *op3 = value;
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
// lodsb
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

    *(DATA_SEGMENT + SI) = AX & 0xff;

    SI += incr;
    IP++;
    *handled = 1;
    return;
}

// stosw
void stosw(struct emu8086 *aCPU, int *handled)
{
    int DF = GET_FLAG(10);

    int incr = DF ? -1 : 1;

    *(DATA_SEGMENT + SI) = AX & 0xff;
    SI += incr;
    *(DATA_SEGMENT + SI) = (AX >> 8) & 0xff;
    SI += incr;
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

// end ops
#ifdef DEBUG
void dump_memory(struct emu8086 *aCPU)
{
    int start = DS * 0x10;
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
    aCPU->op[PUSHF] = &push_flags;
    aCPU->op[PUSH_CS] = &push_reg;
    aCPU->op[PUSH_SS] = &push_reg;
    aCPU->op[PUSH_DS] = &push_reg;
    aCPU->op[PUSH_D16] = &push_addr;

    // POP
    aCPU->op[POP_ES] = &pop_reg;
    aCPU->op[POPF] = &pop_flags;
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
    aCPU->op[XCHG_R16_AX] = &xchg_ax_r16;

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
    // SCASB
    aCPU->op[SCASB] = &scasb;

    // SCASW
    aCPU->op[SCASW] = &scasw;
    // STOSB
    aCPU->op[STOSB] = &stosb;

    // STOSW
    aCPU->op[STOSW] = &stosw;

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
    for (int i = 0; i < 8; i++)
    {
        /* code */
        aCPU->op[64 + i] = &inc_reg16;
        aCPU->op[72 + i] = &dec_reg16;
        aCPU->op[80 + i] = &push_reg16;
        aCPU->op[88 + i] = &pop_reg16;
        aCPU->op[176 + i] = &mov_reg8_i8;
        aCPU->op[184 + i] = &mov_reg16_i16;
    }
}

void main()
{
    struct emu8086 *aCPU = emu8086_new();
    op_setptrs(aCPU);
    do_assembly(aCPU, "test.asm");
    assembler_step = 1;
    do_assembly(aCPU, "test.asm");
    CS = aCPU->code_start_addr / 0x10;
    DS = 0x03ff;
    SP = 128;
    *(DATA_SEGMENT + 0x885f) = 0xf;
    *(DATA_SEGMENT + 0x885f + 1) = 0xf;
    _SS = ((CS * 0x10) - 0x20000) / 0x10;
    AX = CX = 0;
    while (IP < aCPU->end_address - 1)
    {
        int op = *(CODE_SEGMENT_IP), handled = 0;

        aCPU->op[op](aCPU, &handled);
        if (!handled)
        {
            char buf[25];
            sprintf(buf, "Unhandled instrution on line %d, opcode: %x", _INSTRUCTIONS->line_number, op); //_message()
            _message(buf, ERR);
        }
        if (aCPU->skip_next)
            aCPU->skip_next = 0;
        else if (_INSTRUCTIONS->next != NULL)
            _INSTRUCTIONS = _INSTRUCTIONS->next;
        if (IP > 0xffff)
        {
            IP = IP - 0xffff;
            CS = CS - 0x10000;
        }
        handled = 0;
    }
    dump_memory(aCPU);
    dump_registers(aCPU);
}