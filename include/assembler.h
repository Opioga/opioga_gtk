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
*/



#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#define MAX_SIZE 256


struct emu8086;
struct instruction;

char *match_register(char *p, int width, int *value), *match_expression(char *p, int *value),
    *match_expression_level1(char *p, int *value), *match_expression_level2(char *p, int *value),
    *match_expression_level3(char *p, int *value), *match_expression_level4(char *p, int *value),
    *match_expression_level5(char *p, int *value), *match_expression_level6(char *p, int *value);

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

struct label *define_label(char *name, int value);
struct label *find_label(char *name);

struct errors_list
{
    char message[256];
    struct errors_list *next;
    struct errors_list *prev;
    int line;
};

void message(char *m, int level, int line);

char *avoid_spaces(char *p);
void seperate();
struct instruction *define_instruction(int line);

void do_assembly(struct emu8086 *aCPU, char *fname);

#endif