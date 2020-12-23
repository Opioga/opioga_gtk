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
#include <code.h>

struct _Emu8086AppCode
{
    GtkTextView parent;
};

typedef struct _Emu8086AppCodePrivate Emu8086AppCodePrivate;

struct _Emu8086AppCodePrivate
{
    gint lc;
    gint line;
    GtkWidget *lines;
    GtkWidget *code;
    Emu8086AppWindow *win;
    uint16_t hl;
    gboolean isOpen;
};

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
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCode, emu_8086_app_code, GTK_TYPE_TEXT_VIEW);

Emu8086AppCode *emu_8086_app_code_new(void)
{
    return g_object_new(EMU_8086_APP_CODE_TYPE, NULL);
}

static void emu_8086_app_code_init(Emu8086AppCode *win){

    // gtk_widget_init_template(GTK_WIDGET(win));
};
static void emu_8086_app_code_class_init(Emu8086AppCodeClass *class)
{
}
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
    gtk_text_iter_forward_to_line_end(&iter);
    gchar *line = gtk_text_buffer_get_text(buffer, &start, &iter, FALSE), *p;
    gint v = 0;

    p = line;
    iter = start;
    while (*p)
    {

        // iter = start;
        v = 0;
        // g_print("here %s %d %d\n", gtk_text_buffer_get_text(buffer, &start, &mend, FALSE), v, i + 1);
        gchar *p2, buf[20];
        int sep = 0, o = 0;
        p2 = buf;

        while (*p && isspace(*p))
        {
            p++;
            v++;
        }
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
        p2 = gtk_text_buffer_get_text(buffer, &start, &iter, FALSE);

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

static void highlight(GtkWidget *co)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);
    PRIV_CODE;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gint lc = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer));
    gint i = 0, t = 0;

    while (i < lc)
    {
        t = 0;

        _highlight(buffer, i);
        i++;
        // g_free(line);
    }
    priv->hl = lc;
}

void select_line(GtkWidget *co, gint line)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);
    GtkTextMark *mark;
    gboolean ret = TRUE;
    // line = line ? line : 1;
    GtkTextIter iter, start;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gint lc = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer));
    PRIV_CODE;

    if (line == priv->line && line > 1)
        return;
    if (priv->line)
    {
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         priv->line);
        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);

        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }
    else
    {
        // g_print("lin: %d\n", priv->line);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         0);
        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }

    if (line >= lc)
    {
        ret = FALSE;
        priv->line = lc;
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         lc);
    }
    else
    {
        priv->line = line;

        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         line);
    }
    start = iter;
    // gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &iter);
    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(code), mark);
    gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
}
void reset_code(GtkWidget *co)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);

    PRIV_CODE;
    if (priv->line)
    {
        GtkTextIter iter, start;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         priv->line);
        gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &start,
                                         priv->line);
        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }
}
void update(GtkTextBuffer *textbuffer, Emu8086AppCode *code)
{
    PRIV_CODE;
    gint lc = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(textbuffer));
    if (priv->lc == lc)
    {
        GtkTextMark *mark;
        GtkTextIter iter;
        GtkTextBuffer *buffer = GTK_TEXT_BUFFER(textbuffer);
        mark = gtk_text_buffer_get_mark(buffer, "insert");
        gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark);
        gint i = gtk_text_iter_get_line(&iter);
        _highlight(buffer, i);

        return;
    }
    priv->lc = lc;
    int size, index;

    if (lc < 10)
        size = lc * 2;
    else if (lc < 100)
    {
        size = 9 * 2 + (90 * 3);
    }
    else
        size = (900 * 4) + (9 * 2) + (90 * 3);

    char *ln = (char *)malloc(sizeof(char) * size + 2);
    ln[0] = '\0';
    for (int i = 1; i < lc + 1; i++)

    {
        if (i < 10)
        {
            char buf[3];
            sprintf(buf, "%d\n", i);
            ln = strcat(ln, buf);
        }
        else if (i < 100)
        {
            char buf[4];
            sprintf(buf, "%d\n", i);
            ln = strcat(ln, buf);
        }
        else
        {
            char buf[5];
            sprintf(buf, "%d\n", i);
            ln = strcat(ln, buf);
        }

        /* code */
    }

    gtk_label_set_text(GTK_LABEL(priv->lines), ln);
    free(ln);
    highlight(code);
}

void user_function(GtkTextBuffer *textbuffer,
                   GtkTextIter *location,
                   gchar *text,
                   gint len,
                   gpointer user_data)
{

    Emu8086AppCode *code = EMU_8086_APP_CODE(user_data);
    PRIV_CODE;
    update(textbuffer, code);
    if (!priv->isOpen)
        upd(priv->win);
    else
        priv->isOpen = FALSE;
}

void user_function2(GtkTextBuffer *textbuffer,
                    GtkTextIter *start,
                    GtkTextIter *end,
                    gpointer user_data)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(user_data);
    PRIV_CODE;
    update(textbuffer, code);
    if (!priv->isOpen)
        upd(priv->win);
    else
        priv->isOpen = FALSE;
}

static void create_tags(GtkTextBuffer *buffer)
{
    gtk_text_buffer_create_tag(buffer, "step", "background", "#B7B73B", "foreground", "#FF0000", NULL);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground", "#96CBFE", NULL);
    gtk_text_buffer_create_tag(buffer, "reg", "foreground", "#B5CAE8", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buffer, "string", "foreground", "#CE9178", NULL);
    gtk_text_buffer_create_tag(buffer, "label_def", "foreground", "#DCDCAA", NULL);
    gtk_text_buffer_create_tag(buffer, "num", "foreground", "#B5CEA8", NULL);
    gtk_text_buffer_create_tag(buffer, "special", "foreground", "#C586C0", "weight", PANGO_WEIGHT_BOLD, NULL);
    // #c586c0
    // #b5cea8
    gtk_text_buffer_create_tag(buffer, "comment", "foreground", "#6A9955", "style", PANGO_STYLE_ITALIC, NULL);
}

Emu8086AppCode *create_new(GtkWidget *box, GtkWidget *box2, Emu8086AppWindow *win)
{
    GtkWidget *lines;
    Emu8086AppCode *code;
    PangoFontDescription *font_desc;
    lines = gtk_label_new("1\n");
    gtk_widget_show(lines);
    code = emu_8086_app_code_new();
    font_desc = pango_font_description_from_string("mono 12");
    gtk_widget_modify_font(code, font_desc);
    pango_font_description_free(font_desc);
    gtk_container_add(GTK_CONTAINER(box2), lines);
    gtk_container_add(GTK_CONTAINER(box), box2);

    gtk_container_add(GTK_CONTAINER(box), code);
    PRIV_CODE;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    create_tags(buffer);
    // priv->code = code;
    priv->isOpen = FALSE;
    priv->lines = lines;
    priv->line = 0;
    priv->win = win;
    priv->hl = 0;
    g_signal_connect(buffer, "insert-text", G_CALLBACK(user_function), code);
    g_signal_connect(buffer, "delete-range", G_CALLBACK(user_function2), code);
    //

    return code;

    //gtk_widget_show(lines);
}
