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
 * emu_8086_app_utils.h
 * Runner class
 */

#ifndef EMU_8086_APP_UTILS_H
#define EMU_8086_APP_UTILS_H
#include <ctype.h>

#include <glib/gtypes.h>

gchar *keywords[] = {
    "aaa",
    "aad",
    "aam",
    "aas",
    "adc",
    "add",
    "and",
    "call",
    "cbw",
    "clc",
    "cld",
    "cli",
    "cmc",
    "cmp",
    "cmpsb",
    "cmpsw",
    "cwd",
    "daa",
    "das",
    "dec",
    "div",
    "esc",
    "hlt",
    "idiv",
    "imul",
    "in",
    "inc",
    "int",
    "into",
    "iret",
    "ja",
    "jae",
    "jb",
    "jbe",
    "jc",
    "jcxz",
    "je",
    "jg",
    "jge",
    "jl",
    "jle",
    "jna",
    "jnae",
    "jnb",
    "jnbe",
    "jnc",
    "jne",
    "jng",
    "jnge",
    "jnl",
    "jnle",
    "jno",
    "jnp",
    "jns",
    "jnz",
    "jo",
    "jp",
    "jpe",
    "jpo",
    "js",
    "jz",
    "jmp",
    "lahf",
    "lds",
    "lea",
    "les",
    "lock",
    "lodsb",
    "lodsw",
    "loop",
    "loope",
    "loopne",
    "loopnz",
    "loopz",
    "mov",
    "movsb",
    "movsw",
    "mul",
    "neg",
    "nop",
    "not",
    "or",
    "out",
    "pop",
    "popf",
    "push",
    "pushf",
    "rcl",
    "rcr",
    "rep",
    "repe",
    "repne",
    "repnz",
    "repz",
    "ret",
    "retn",
    "retf",
    "rol",
    "ror",
    "sahf",
    "sal",
    "sar",
    "sbb",
    "scasb",
    "scasw",
    "shl",
    "shr",
    "stc",
    "std",
    "sti",
    "stosb",
    "stosw",
    "sub",
    "test",
    "org",
    "wait",
    "xchg",
    "xlat",
    "xor"};
gchar *reg1[] = {
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
    "DI",
    "CS",
    "DS",
    "SS",
    "ES"};

gchar *tag_names[7] = {
    "keyword",
    "reg",
    "string",
    "label_def",
    "num",
    "special",
    "comment"};

gchar *theme_colors[24] = {

    /* keyword */ "#96CBFE",
    "#B5CAE8",
    "#CE9178",
    "#ebeb8d",
    "#B5CEA8",
    "#C586C0",
    "#6A9955",
    NULL,
    "#80ffbb",
    "#ffee80",
    "#3ad900",
    "#ff9d00",
    "#ffffff",
    "#ffdd00",
    "#0088ff",
    NULL,
    "#000000",
    "#0057AE",
    "#FF0000",
    "#808080",
    "#FF0000",
    "#B07E00",
    "#808080",
    NULL

};

gboolean getkeyword(gchar *keyword)
{
    char i = 0, *p;
    gboolean ret = FALSE;
    p = keyword;
    while (*p)
    {
        *p = tolower(*p);
        p++;
    }
    p = keyword;
    while (i < 118)
    {
        if (strcmp(p, keywords[i]) == 0)
        {
            ret = TRUE;
            break;
        }
        i++;
    }
    return ret;
}
gboolean getreg(gchar *keyword)
{
    char i = 0, *p;

    gboolean ret = FALSE;
    p = keyword;
    int len = 0;
    while (*p)
    {
        *p = toupper(*p);
        p++;
        len++;
    }
    if (len > 2)
        return ret;
    p = keyword;
    while (i < 20)
    {
        if (strcmp(p, reg1[i]) == 0)
        {
            ret = TRUE;
            break;
        }
        i++;
    }
    return ret;
}
gboolean getsp_(gchar *keyword)
{
    char i = 0, *p;
    gboolean ret = FALSE;
    p = keyword;
    while (*p)
    {
        *p = toupper(*p);
        p++;
    }
    p = keyword;

    if (strcmp(p, "DB") == 0)
    {
        ret = TRUE;
    }
    else if (strcmp(p, "DW") == 0)
    {
        ret = TRUE;
    }

    else if (strcmp(p, "EQU") == 0)
    {
        ret = TRUE;
    }
    else if (strcmp(p, "BYTE") == 0)
    {
        ret = TRUE;
    }
    else if (strcmp(p, "WORD") == 0)
    {
        ret = TRUE;
    }
    else if (strcmp(p, "OFFSET") == 0)
    {
        ret = TRUE;
    }

    else if (strcmp(p, "DUP") == 0)
    {
        ret = TRUE;
    }
    return ret;
}
gboolean getstr_(gchar *keyword)
{
    gint i = strlen(keyword);
    gboolean ret = FALSE;

    if (*keyword == '"' && keyword[i - 1] == '"')
        ret = TRUE;
    else if (*keyword == '\'' && keyword[i - 1] == '\'')
        ret = TRUE;
    return ret;
}

gboolean getlab_(gchar *keyword)
{
    gint i = strlen(keyword);
    gboolean ret = FALSE;

    if (*keyword != '\0' && keyword[i - 1] == ':')
        ret = TRUE;

    return ret;
}

gboolean getnum_(gchar *keyword)
{
    gint i = 50;
    gchar *p = keyword;
    gboolean ret = FALSE;
    if (p[0] == '0' && tolower(p[1]) == 'b')
    { /* Binary */
        ret = TRUE;
    }
    else if (p[0] == '0' && tolower(p[1]) == 'x' && isxdigit(p[2]))
        ret = TRUE;
    else if (isdigit(p[0]))
        ret = TRUE;
    else if (p[0] == '$' && isdigit(p[1]))
    { /* Hexadecimal */
        ret = TRUE;
    }
    return ret;
}

#endif