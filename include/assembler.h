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

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#define MAX_SIZE 256
struct emu8086;
struct instruction;

char *match_register(), *match_expression(),
    *match_expression_level1(), *match_expression_level2(),
    *match_expression_level3(), *match_expression_level4(),
    *match_expression_level5(), *match_expression_level6();
int undefined, instruction_addr,
    instruction_register, instruction_value2;
unsigned char code_mem[0xffff];
char global_label[MAX_SIZE];
char name[MAX_SIZE];
char expr_name[MAX_SIZE];
char undefined_name[MAX_SIZE];

int line_number, assembler_step, size, data_mem_offset;
struct label
{
    struct label *left;
    struct label *right;
    int value;
    char name[25];
    int label_identifier;
    int is_addr;
    int is_defined;
    int line_number;
};

/* 
    assm.c
*/
int label_identifier, is_offset;
struct label *label_list, *explore;
struct label *last_label, *current_label, *_current_label;
int is_first;
struct label *define_label(char *name, int value);
struct label *find_label(char *name);
char *p;
char line[256];
char part[MAX_SIZE];
int errors;
struct errors_list
{
    char message[256];
    struct errors_list *next;
    struct errors_list *prev;
    int line;
};
struct errors_list *define_errors_list(char *name, int line);
struct errors_list *first_err, *list_err;
int instruction_addressing, ic;
int instruction_offset;
int instruction_offset_width, start_address, address, assembler_step,
    instruction_value;

void message(char *m, int level, int line);

char *avoid_spaces(char *p);
void seperate();
struct instruction *define_instruction(int line);

void do_assembly(struct emu8086 *aCPU, char *fname);

#endif