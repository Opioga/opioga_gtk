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

#include <code_buffer.h>

static gchar *keywords[] = {
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
    "wait",
    "xchg",
    "xlat",
    "xor"};
static char *reg1[] = {
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

static gboolean getkeyword(gchar *keyword)
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
static gboolean getreg(gchar *keyword)
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
static gboolean getsp_(gchar *keyword)
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
    return ret;
}
static gboolean getstr_(gchar *keyword)
{
    gint i = strlen(keyword);
    gboolean ret = FALSE;

    if (*keyword == '"' && keyword[i - 1] == '"')
        ret = TRUE;
    else if (*keyword == '\'' && keyword[i - 1] == '\'')
        ret = TRUE;
    return ret;
}

static gboolean getlab_(gchar *keyword)
{
    gint i = strlen(keyword);
    gboolean ret = FALSE;

    if (*keyword != '\0' && keyword[i - 1] == ':')
        ret = TRUE;

    return ret;
}

static gboolean getnum_(gchar *keyword)
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

static void _highlight(GtkTextBuffer *buffer, gint i)
{
    GtkTextIter iter, start;
    // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                     &iter,
                                     i);
    start = iter;
    g_return_if_fail(gtk_text_iter_get_buffer(&start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(&iter) == buffer);
    gtk_text_iter_forward_to_line_end(&iter);
    g_return_if_fail(gtk_text_iter_get_buffer(&start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(&iter) == buffer);
    gchar *line = gtk_text_buffer_get_text(buffer, &start, &iter, FALSE), *p;
    gint v = 0;
    if (gtk_text_iter_get_buffer(&iter) == buffer)
    {
        p = line;
        iter = start;
        while (*p)
        {

            // iter = start;
            v = 0;
            // g_print("here %s %d %d\n", gtk_text_buffer_get_text(buffer, &start, &mend, FALSE), v, i + 1);
            gchar *p2, buf[256];
            int sep = 0, o = 0;
            p2 = buf;

            while (*p && isspace(*p))
            {
                p++;
                v++;
            }
            if (!*p)
                return;
            sep = v;
            if (sep > 0)
                gtk_text_iter_forward_chars(&start, sep - 1);
            if (*p == ';')
            {

                gtk_text_iter_forward_to_line_end(&iter);
                gtk_text_buffer_apply_tag_by_name(buffer, "comment", &start, &iter);

                break;
            }
            iter = start;
            while (*p && !isspace(*p) && *p != ';')
            {
                if (*p == ',')
                {

                    break;
                }
                v++;
                *p2++ = *p++;
            }
            o = v;
            gtk_text_iter_forward_chars(&iter, o);
            *p2 = '\0';
            if (*p == ',')
            {
                p++;
                v++;
            }
            while (*p && isspace(*p))
            {
                p++;
                v++;
            }
            // g_print("lio\n");

            if (gtk_text_iter_get_buffer(&iter) == buffer)
                p2 = gtk_text_buffer_get_text(buffer, &start, &iter, FALSE);
            else
                return;
            sep = v;
            if (strlen(buf) > 0)
            {
                gint len = strlen(buf) - strlen(p2);

                if (len <= -2)
                {
                    // printf("%s, %s, %d\n", p2, buf, len);
                    gtk_text_iter_backward_chars(&start, -1 - len);

                    gtk_text_iter_backward_chars(&iter, -1 - len);
                }

                // g_print("heren %s %d %d\n", p2, v, i + 1);
                if (getnum_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "num", &start, &iter);
                else if (getsp_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "special", &start, &iter);
                else if (getstr_(buf))

                    gtk_text_buffer_apply_tag_by_name(buffer, "string", &start, &iter);
                else if (getlab_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "label_def", &start, &iter);
                else if (getreg(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "reg", &start, &iter);

                else if (getkeyword(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "keyword", &start, &iter);
            }

            if (sep > 0)
                gtk_text_iter_forward_chars(&start, sep);
            // t++;
        }
        // i++;
        g_free(line);
    }
}

typedef struct _Emu8086AppCodeBufferPrivate Emu8086AppCodeBufferPrivate;

struct _Emu8086AppCodeBuffer
{
    GtkTextBuffer parent;
};

struct _Emu8086AppCodeBufferPrivate
{

    gint lc;
    gint line;
    GtkTextTagTable *table;
    GSettings *settings;
    Emu8086AppCode *code;
    gint timeout;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCodeBuffer, emu_8086_app_code_buffer, GTK_TYPE_TEXT_BUFFER);

static void emu_8086_app_code_buffer_init(Emu8086AppCodeBuffer *buffer)
{

    GtkTextTagTable *tag;
    //  gtk_text_buffer

    PRIV_CODE_BUFFER;
    priv->settings = g_settings_new("com.krc.emu8086app");
    priv->lc = 0;
    priv->line = 0;
    priv->timeout = 0;
}

static void highlight(Emu8086AppCodeBuffer *buffer, gint line)
{

    PRIV_CODE_BUFFER;
    gint i = 0, t = 0;
    // i = line;

    //  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), m, &iter);
    while (i < (line + 1))
    {
        t = 0;
        // g_print("herrre \n");
        _highlight(buffer, i);
        i++;
        // g_free(line);
    }
    priv->lc = i;
}

gboolean hl(gpointer user_data)
{
    Emu8086AppCodeBuffer *buffer;
    buffer = EMU_8086_APP_CODE_BUFFER(user_data);
    GtkTextMark *mark;
    GtkTextIter iter2;
    PRIV_CODE_BUFFER;

    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_buffer_get_iter_at_mark(buffer, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    _highlight(buffer, i);
    priv->timeout = 0;
    return G_SOURCE_REMOVE;
}

void queue_highlight(Emu8086AppCodeBuffer *buffer)
{

    //  buffer = EMU_8086_APP_CODE_BUFFER(user_data);
    PRIV_CODE_BUFFER;
    if (priv->timeout != 0)
    {
        g_source_remove(priv->timeout);
    }
    priv->timeout = gdk_threads_add_timeout_full(G_PRIORITY_LOW,
                                                 100,
                                                 hl,
                                                 buffer,
                                                 NULL);
}

static void emu_8086_app_code_buffer_insert_text_real(GtkTextBuffer *buffer,
                                                      GtkTextIter *iter,
                                                      const gchar *text,
                                                      gint len)
{
    gint start_offset;

    g_return_if_fail(EMU_8086_IS_APP_CODE_BUFFER(buffer));

    g_return_if_fail(iter != NULL);
    g_return_if_fail(text != NULL);
    g_return_if_fail(gtk_text_iter_get_buffer(iter) == buffer);

    GTK_TEXT_BUFFER_CLASS(emu_8086_app_code_buffer_parent_class)->insert_text(buffer, iter, text, len);
    buffer = EMU_8086_APP_CODE_BUFFER(buffer);
    PRIV_CODE_BUFFER;
    GtkTextMark *mark;
    GtkTextIter iter2;
    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_buffer_get_iter_at_mark(buffer, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    start_offset = i - priv->lc;

    if (start_offset > 0)
    {
        // g_print("lion\n");
        highlight(buffer, i);
    }
    else
    {
        queue_highlight(buffer);
    }
}

static emu_8086_app_code_buffer_delete_range(GtkTextBuffer *buffer,
                                             GtkTextIter *start,
                                             GtkTextIter *end)
{
    g_print("herel\n");
    gint start_offset;
    g_return_if_fail(EMU_8086_IS_APP_CODE_BUFFER(buffer));
    g_return_if_fail(start != NULL);
    g_return_if_fail(end != NULL);
    g_return_if_fail(gtk_text_iter_get_buffer(start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(end) == buffer);
    //
    PRIV_CODE_BUFFER;
    GtkTextMark *mark;
    GtkTextIter iter2;
    GTK_TEXT_BUFFER_CLASS(emu_8086_app_code_buffer_parent_class)->delete_range(buffer, start, end);
    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_buffer_get_iter_at_mark(buffer, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    queue_highlight(buffer);
}
static void emu_8086_app_code_buffer_class_init(Emu8086AppCodeBufferClass *klass)
{

    GtkTextBufferClass *text_buffer_class;

    text_buffer_class = GTK_TEXT_BUFFER_CLASS(klass);
    text_buffer_class->insert_text = emu_8086_app_code_buffer_insert_text_real;
    text_buffer_class->delete_range = emu_8086_app_code_buffer_delete_range;
}

Emu8086AppCodeBuffer *emu_8086_app_code_buffer_new(GtkTextTagTable *table)
{
    return g_object_new(EMU_8086_APP_CODE_BUFFER_TYPE,
                        "tag-table", table,
                        NULL);
}

void refreshLines(Emu8086AppCodeBuffer *buffer)
{
    PRIV_CODE_BUFFER;
    priv->lc = 0;
}

void setCode(Emu8086AppCodeBuffer *buffer, Emu8086AppCode *code)
{
    PRIV_CODE_BUFFER;
    priv->code = code;
}