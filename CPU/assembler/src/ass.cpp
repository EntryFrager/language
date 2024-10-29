#ifndef ASS_CPP
#define ASS_CPP

#include "../inc/ass.h"

static void split_commands (Spu *spu, int *code_error);
static void skip_nul_str (Spu *spu, int *code_error);
static void process_label (Spu *spu, size_t ip, size_t counter_ip, int *code_error);
static void pars_command (Spu *spu, int *code_error);
static int write_buf (Commands *cmd, double *buf, size_t counter, int *code_error);
static void process_param (Spu *spu, size_t ip, size_t cmd_len, int *code_error);
static void get_param (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error);
static void print_listing (Spu *spu, int *code_error);
static void get_param_ram (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error);
static int check_process_argc (Commands *cmd, size_t cmd_len, size_t len, int *code_error);
static int check_process_reg  (Commands *cmd, size_t cmd_len, size_t len, int *code_error);
static void find_process_label  (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error);

void input_text (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    fopen_init_(spu->fp_input, spu->file_name_input, "r")

    spu->size_file = get_file_size(spu->fp_input, code_error);

    calloc_init_(spu->buf_input, char *, spu->size_file + 1, sizeof(char));

    size_t read_size = fread(spu->buf_input, sizeof (char), spu->size_file, spu->fp_input);
    my_assert(read_size == spu->size_file, ERR_FREAD);

    *(spu->buf_input + spu->size_file) = '\0';

    fclose_(spu->fp_input);

    split_commands(spu, code_error);

    skip_nul_str(spu, code_error);
}

void split_commands (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    number_of_commands(spu, code_error);

    calloc_init_(spu->cmd, Commands *, spu->n_cmd, sizeof(Commands));

    size_t n_cmd = 0;

    char comment = 0;

    size_t buf_pos_count = 0;

    for (size_t ip = 0; ip < spu->n_cmd; ip++)
    {
        if (buf_pos_count < spu->size_file)
        {
            n_cmd++;

            while (buf_pos_count < spu->size_file && isspace(*(spu->buf_input + buf_pos_count)) != 0)
            {
                buf_pos_count++;
            }

            spu->cmd[ip].command = spu->buf_input + buf_pos_count;

            while (buf_pos_count < spu->size_file && *(spu->buf_input + buf_pos_count) != ';' && *(spu->buf_input + buf_pos_count) != '\n')
            {
                buf_pos_count++;
                spu->cmd[ip].size_str++;
            }

            comment = *(spu->buf_input + buf_pos_count);

            *(spu->buf_input + buf_pos_count) = '\0';
            buf_pos_count++;

            if (comment == ';')
            {
                while (buf_pos_count < spu->size_file && *(spu->buf_input + buf_pos_count) != '\n')
                {
                    buf_pos_count++;
                }
            }
        }
    }
}

void skip_nul_str (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    calloc_(new_cmd, Commands *, spu->n_cmd, sizeof(Commands));

    int count_cmd = 0;

    for (size_t i = 0; i < spu->n_cmd; i++)
    {
        if (spu->cmd[i].size_str != 0)
        {
            new_cmd[count_cmd].command = spu->cmd[i].command;
            new_cmd[count_cmd].size_str = spu->cmd[i].size_str;

            count_cmd++;
        }
    }

    realloc_(new_cmd, Commands *, count_cmd * sizeof(Commands));

    spu->n_cmd = count_cmd;

    free(spu->cmd);
    spu->cmd = new_cmd;
}

void code_compilation (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    pars_command(spu, code_error);

    spu->n_words -= spu->n_label;

    calloc_init_(spu->buf_output, double *, spu->n_words, sizeof(double));

    pars_command(spu, code_error);

    fwrite(spu->buf_output, sizeof(double), spu->n_words, spu->fp_print_bin);

    print_listing(spu, code_error);
}

#define DEF_CMD(name, num, have_arg, ...)                                                   \
    if (strncasecmp(spu->cmd[ip].command, name, sizeof(name) - 1) == 0)                     \
    {                                                                                       \
        spu->cmd[ip].cmd_code = num;                                                        \
        if (have_arg)                                                                       \
        {                                                                                   \
            counter_ip++;                                                                   \
            process_param(spu, ip, sizeof(name) - 1, code_error);                           \
        }                                                                                   \
    }                                                                                       \
    else

void pars_command (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    size_t counter_buf = 0;
    size_t counter_ip  = 0;

    for (size_t ip = 0; ip < spu->n_cmd; ip++)
    {
        if (spu->n_label <= LABEL_CNT)
        {
            if (*(spu->cmd[ip].command + spu->cmd[ip].size_str - 1) == ':' && (spu->buf_output == NULL))
            {
                process_label(spu, ip, counter_ip, code_error);
            }
            else if (*(spu->cmd[ip].command + spu->cmd[ip].size_str - 1) != ':')
            {
                #include "../../include/commands.h"
                #include "../../include/jump_cmd.h"

                {
                    *code_error |= ERR_COMMAND;
                }

                if ((spu->cmd[ip].cmd_code & HAVE_ARG) && (spu->cmd[ip].cmd_code & HAVE_REG))
                {
                    counter_ip++;
                }

                if (spu->buf_output != NULL)
                {
                    counter_buf = write_buf(&spu->cmd[ip], spu->buf_output, counter_buf, code_error);
                }
            }
        }
        else
        {
            *code_error |= ERR_LABEL;
            return;
        }

        counter_ip++;
    }
}

#undef DEF_CMD

int write_buf (Commands *cmd, double *buf, size_t counter, int *code_error)
{
    my_assert(cmd != NULL, ERR_PTR);
    my_assert(buf != NULL, ERR_PTR);

    buf[counter++] = (double) cmd->cmd_code;

    if ((cmd->cmd_code & HAVE_REG) && (cmd->cmd_code & HAVE_ARG))
    {
        buf[counter++] = (double) cmd->reg;
        buf[counter++] = (double) cmd->argc;
    }
    else if (cmd->cmd_code & HAVE_REG)
    {
        buf[counter++] = (double) cmd->reg;
    }
    else if (cmd->cmd_code & HAVE_ARG)
    {
        buf[counter++] = (double) cmd->argc;
    }

    return counter;
}

void process_label (Spu *spu, size_t ip, size_t counter_ip, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    spu->label[spu->n_label].name_label = spu->cmd[ip].command;
    spu->label[spu->n_label].label_ip   = counter_ip - spu->n_label;
    spu->label[spu->n_label].size_label = spu->cmd[ip].size_str - 1;
    spu->n_label++;
}

void process_param (Spu *spu, size_t ip, size_t cmd_len, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    if (*(spu->cmd[ip].command + cmd_len + 1) == '[' && *(spu->cmd[ip].command + spu->cmd[ip].size_str - 1) == ']')
    {
        get_param_ram(spu, ip, cmd_len, 2, code_error);

        spu->cmd[ip].cmd_code |= HAVE_RAM;
    }
    else
    {
        get_param(spu, ip, cmd_len, 1, code_error);
    }
}

void get_param (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    if (check_process_argc(&spu->cmd[ip], cmd_len, len, code_error) == NO_ARGUMENT)
    {
        if (check_process_reg(&spu->cmd[ip], cmd_len, len, code_error) == NO_ARGUMENT)
        {
            find_process_label(spu, ip, cmd_len, len, code_error);
        }
    }
}

void get_param_ram (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    check_process_reg(&spu->cmd[ip], cmd_len, len, code_error);
    check_process_argc(&spu->cmd[ip], cmd_len, len + 5, code_error);
}

static int check_process_argc (Commands *cmd, size_t cmd_len, size_t len, int *code_error)
{
    my_assert(cmd != NULL, ERR_PTR);

    char *str = cmd->command + cmd_len + len;

    double value = strtod(cmd->command + cmd_len + len, &str);

    if (str != cmd->command + cmd_len + len)
    {
        cmd->argc = value;
        cmd->cmd_code |= HAVE_ARG;
        *(cmd->command + cmd_len) = '\0';

        return YES_ARGUMENT;
    }

    return NO_ARGUMENT;
}

static int check_process_reg (Commands *cmd, size_t cmd_len, size_t len, int *code_error)
{
    my_assert(cmd != NULL, ERR_PTR);

    if (toupper(*(cmd->command + cmd_len + len)) == 'R' && toupper(*(cmd->command + cmd_len + len + 2)) == 'X')
    {
        if (toupper(*(cmd->command + cmd_len + len + 1)) >= 'A' || toupper(*(cmd->command + cmd_len + len + 1)) <= 'D')
        {
            cmd->reg = toupper(*(cmd->command + cmd_len + len + 1)) - 'A' + 1;
            cmd->cmd_code |= HAVE_REG;
            *(cmd->command + cmd_len) = '\0';

            return YES_ARGUMENT;
        }
        else
        {
            *code_error |= ERR_REG;
            return ERR_REG;
        }
    }

    return NO_ARGUMENT;
}

static void find_process_label (Spu *spu, size_t ip, size_t cmd_len, size_t len, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    for (size_t pos_label = 0; pos_label <= spu->n_label; pos_label++)
    {
        if (spu->label[pos_label].name_label != NULL)
        {
            if (strncasecmp(spu->cmd[ip].command + cmd_len + len, spu->label[pos_label].name_label, spu->label[pos_label].size_label) == 0 &&
                (*(spu->cmd[ip].command + cmd_len + len + spu->label[pos_label].size_label) == '\0' ||
                 *(spu->cmd[ip].command + cmd_len + len + spu->label[pos_label].size_label) == ' '))
            {
                spu->cmd[ip].argc = spu->label[pos_label].label_ip;
                spu->cmd[ip].cmd_code |= HAVE_ARG;
                *(spu->cmd[ip].command + cmd_len) = '\0';

                return;
            }
        }
        else
        {
            spu->cmd[ip].argc = 0;

            return;
        }
    }
}

void print_listing (Spu *spu, int *code_error)
{
    my_assert(spu != NULL, ERR_PTR);

    fprintf(spu->fp_print_txt, "+------------+------------+------------+------------+\n");
    fprintf(spu->fp_print_txt, "|  NAME_CMD  |  HEX_SPEAK |  VALUE_ARG |  VALUE_REG |\n");
    fprintf(spu->fp_print_txt, "+------------+------------+------------+------------+\n");

    for (size_t ip = 0; ip < spu->n_cmd; ip++)
    {
        if (*(spu->cmd[ip].command + spu->cmd[ip].size_str - 1) != ':')
        {
            fprintf(spu->fp_print_txt, "|%12s|%12x|%12lg|%12d|\n", spu->cmd[ip].command, spu->cmd[ip].cmd_code, spu->cmd[ip].argc, spu->cmd[ip].reg);
            fprintf(spu->fp_print_txt, "+------------+------------+------------+------------+\n");
        }
    }
}

#endif // ASS_CPP
