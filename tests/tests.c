#include <glib.h>
#include <assembler.h>
#include <emu8086.h>
#define EMU_RESET_OFFSET 12
#define EMU_TEST_OFFSET 3
extern struct instruction *_instruction_list;
extern struct instruction *_current_instruction, *_first_instruction;
extern struct label *label_list, *explore;
extern struct errors_list *first_err, *list_err;
extern int errors, assembler_step;
extern char *reg1[16];
int test_index;

const gchar *ins_msgs[][3] = {
    {"Testing 'AL + CL'\n", "AL == 20H", "\x1b[38;2;55;255;10m SUCCESS '%s == %xH' ON LINE %d \x1b[38;2;255;255;255m \n"},
    {"Testing 'AL + CH'\n", "AL == 20H", "\x1b[38;2;55;255;10m SUCCESS '%s == %xH' ON LINE %d \x1b[38;2;255;255;255m \n"}

};
int get_test_reg(struct emu8086 *aCPU, int **test_arg, int *test_val, int *_width, int *r)
{
    int reg = 0, width;
    gchar *test_assert_string;
    test_assert_string = ins_msgs[test_index][1];
    gchar buf[3];

    strncpy(buf, test_assert_string, 2);
    width = (buf[1] == 'L' ? 8 : 16);
    *_width = width;
    test_assert_string = match_register(test_assert_string, width, &reg);
    test_assert_string += 4;
    test_assert_string = match_expression_level6(test_assert_string, test_val);
    *r = reg;
    if (width == 8 && reg >= 4)
    {
        reg -= 4;
        *test_arg = SFRS + reg;

        return 1;
    }
    else
    {

        *test_arg = SFRS + reg;

        return 0;
    }
}
void test_instruction(struct emu8086 *aCPU)
{
    struct instruction *_test_instruction;
    _test_instruction = _INSTRUCTIONS;
    gboolean isTestable = FALSE;
    if (_test_instruction != NULL && _test_instruction->line_number < EMU_RESET_OFFSET && _test_instruction->line_number > EMU_TEST_OFFSET && !(_test_instruction->line_number % 2))
    {
        isTestable = TRUE;
        g_print(ins_msgs[test_index][0]);
    }
    if (IP < aCPU->end_address - 1)
    {
        int op = *(CODE_SEGMENT_IP), handled = 0;
        aCPU->op[op](aCPU, &handled);
        if (!handled)
        {
            char buf[15];
            sprintf(buf, "Unhandled instrution on line %d, opcode: %x", _INSTRUCTIONS->line_number, op); //message()
            message(buf, ERR, _INSTRUCTIONS->line_number);
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

    if (isTestable)
    {
        if (test_index < 128)
        {
            int *op1, reg = 0;
            int value;
            op1 = NULL;
            value = 0;

            int width = 0;
            gboolean high_reg = get_test_reg(aCPU, &op1, &value,
                                             &width, &reg);
            if (width < 9)
            {
                gint value2 = high_reg ? *op1 >> 8 : *op1 & 0xff;
                g_assert_cmpint(value, ==, value2);
            }

            g_print(ins_msgs[test_index][2], reg1[reg], value, _INSTRUCTIONS->line_number);
        }
        test_index++;
    }
}

int main()
{
    struct emu8086 *aCPU;
    test_index = 0;
    aCPU = emu8086_new();
    errors = 0;
    assembler_step = 0;

    do_assembly(aCPU, "/home/kosy/Desktop/kosyWork/Desktop/gtkemu8086/tests/test.asm");
    if (errors > 0)
    {
        while (first_err != NULL)
        {
            g_printerr(first_err->message);
        }
        exit(EXIT_FAILURE);
    }
    errors = 0;
    assembler_step = 1;
    do_assembly(aCPU, "/home/kosy/Desktop/kosyWork/Desktop/gtkemu8086/tests/test.asm");
    if (errors > 0)
    {
        while (first_err != NULL)
        {
            g_printerr(first_err->message);
        }
        exit(EXIT_FAILURE);
    }

    while (IP < aCPU->end_address - 1 && test_index < 2)
    {
        test_instruction(aCPU);
    }
}