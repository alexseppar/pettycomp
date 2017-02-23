#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <cmath>
#include <algorithm>

#include "lexer.h"

const unsigned MAX_STR = 20;

lexem:: lexem(lex_type type, unsigned line, unsigned pos):
type_   (type),
line_   (line),
pos_    (pos)
{}

lex_type lexem:: get_type() const {
    return type_;
}

unsigned lexem:: get_pos() const {
    return pos_;
}

unsigned lexem:: get_line() const {
    return line_;
}

const char* lexem:: get_name() const
{
    if (type_ == ID_LEX)
        return name_;
    else
        assert(0 && "invalid request for name");
}

double lexem:: get_val() const
{
    if (type_ == NUM_LEX)
        return val_;
    else
        assert(0 && "invalid request for value");
}

OP lexem:: get_operator_type() const
{
    if (type_ == OP_LEX)
        return op_type_;
    else 
        assert(0 && "invalid request for operator type");
}

void lexem:: set_val(double val)
{
    if (type_ == NUM_LEX)
        val_ = val;
    else
        assert(0 && "invalid request for setting val");
}

void lexem:: set_op_type(OP op_type)
{
    if (type_ == OP_LEX)
        op_type_ = op_type;
    else
        assert(0 && "invalid request for setting operator type");
}

void lexem:: set_name(const char* name)
{
    if (type_ == ID_LEX)
        name_ = name;
    else
        assert(0 && "invalid request for setting name");
}

lexer::lexer(const char *file_name) :
cur_token_      (nullptr),
cur_line_       (1),
cur_position_   (0),
lex_count_      (0)
{
    assert(file_name);
    FILE *fp = fopen(file_name, "r");
    assert(fp);
    assert(fseek(fp, 0, SEEK_END) != -1);
    unsigned file_size = ftell(fp);
    assert(file_size != 0);
    assert(fseek(fp, 0, SEEK_SET) != -1);
    char *tmp = new char[file_size];
    fread(tmp, file_size - 1, sizeof(char), fp);
    tmp[file_size - 1] = '\0';
    info_ = tmp;
    cur_str_ = info_;
    while (*cur_str_ != '\n' && *cur_str_ != '\0')
        ++cur_str_;
    unsigned cur_str_size = cur_str_ - info_;
    cur_str_ = new char[cur_str_size]();
    std::copy(info_, info_ + cur_str_size, cur_str_);
    fclose(fp);
    mov_to_next_token();
}

lexer::~lexer()
{
    if (info_)
        delete[] info_;
    if (cur_token_)
        delete cur_token_;
    if (cur_str_)
        delete[] cur_str_;
}

lexem* lexer::get_cur_token() const {
    return cur_token_;
}

const char* lexer:: get_cur_str() const {
    return cur_str_;
}

void lexer::mov_to_next_token()
{
    char *tmp = info_ + cur_position_;
    while ( *tmp == ' '  ||
            *tmp == '\t' ||
            *tmp == '\n')
    {
        if (*tmp == '\n')
        {
            lex_count_ = 0;
            ++cur_line_;
            delete[] cur_str_;
            cur_str_ = tmp + 1;
            while (*cur_str_ != '\n' && *cur_str_ != '\0')
                ++cur_str_;
            unsigned cur_str_size = cur_str_ - tmp;
            if (cur_str_size != 0)
            {
                cur_str_ = new char[cur_str_size]();
                std::copy(tmp + 1, tmp + cur_str_size, cur_str_);
            }
        }
        ++tmp;
    }
    if (isdigit(*tmp))
    {
        double cur_num = *tmp++ - '0';
        while (isdigit(*tmp))
            cur_num = cur_num * 10 - '0' + *tmp++;
        if (*tmp == '.')
        {
            ++tmp;
            double cur_num_2;
            if (isdigit(*tmp))
                cur_num_2 = (*tmp++ - '0') / 10.0;
            else
            {
                fprintf(stderr, "error (expected digit after '.')\nline %u, pos %u: %s\n", cur_line_, lex_count_, cur_str_);
                cur_token_ = nullptr;
                return;
            }
            unsigned pow_count = 2;
            while (isdigit(*tmp))
                cur_num_2 += (*tmp++ - '0') / pow(10, pow_count++); 
            cur_num += cur_num_2;
        }
        delete cur_token_;
        cur_position_ = tmp - info_;
        cur_token_ = new lexem(NUM_LEX, cur_line_, ++lex_count_);
        cur_token_->set_val(cur_num);
        return;
    }
    if (isalpha(*tmp))
    {
        char *cur_name = new char[MAX_STR]();
        unsigned len_count = 0;
        cur_name[len_count++] = *tmp++;
        while (isalpha(*tmp))
        {
            if (len_count == MAX_STR)
            {
                fprintf(stderr, "too large name of variable '%s...'\nline %u, pos %u: %s\n", cur_name, cur_line_, lex_count_, cur_str_);
                delete[] cur_name;
                cur_token_ = nullptr;
                return;
            }
            cur_name[len_count++] = *tmp++;
        }
        #define CMP(name, LEX_TYPE)\
        if (strcmp(cur_name, #name) == 0)\
        {\
            delete[] cur_name;\
            delete cur_token_;\
            cur_token_ = new lexem(LEX_TYPE, cur_line_, ++lex_count_);\
            cur_position_ = tmp - info_;\
            return;\
        }

        if (strcmp(cur_name, "sin") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new lexem(OP_LEX, cur_line_, ++lex_count_);
            cur_token_->set_op_type(OP_sinus);
            cur_position_ = tmp - info_;
            return;
        }
        if (strcmp(cur_name, "cos") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new lexem(OP_LEX, cur_line_, ++lex_count_);
            cur_token_->set_op_type(OP_cosinus);
            cur_position_ = tmp - info_;
            return;
        }
        CMP(if, IF_LEX)
        CMP(endif, ENDIF_LEX)
        CMP(capture, CAPTURE_LEX)
        CMP(while, WHILE_LEX)
        CMP(endwhile, ENDWHILE_LEX)
        #undef CMP 
        delete cur_token_;
        cur_position_ = tmp - info_;
        cur_token_ = new lexem(ID_LEX, cur_line_, ++lex_count_);
        cur_token_->set_name(cur_name);
        return; 
    }
    #define ROUTINE \
    ++tmp;\
    delete cur_token_;\
    cur_position_ = tmp - info_;
    
    switch (static_cast<int>(*tmp))
    {
        case '=':   ROUTINE
                    cur_token_ = new lexem(EQ_LEX, cur_line_, ++lex_count_);
                    return;
        case '(':   ROUTINE
                    cur_token_ = new lexem(OPEN_BRACKET, cur_line_, ++lex_count_);
                    return;
        case ')':   ROUTINE
                    cur_token_ = new lexem(CLOSE_BRACKET, cur_line_, ++lex_count_);
                    return;
        case ';':   ROUTINE
                    cur_token_ = new lexem(END_OF_EXPR, cur_line_, ++lex_count_);
                    return;
        case ',':   ROUTINE
                    cur_token_ = new lexem(COMMA_LEX, cur_line_, ++lex_count_);
                    return;
        default:    break;
    }
    #undef ROUTINE
    if (*tmp == '+' ||
        *tmp == '-' ||
        *tmp == '*' ||
        *tmp == '/' ||
        *tmp == '^' ||
        *tmp == '>' ||
        *tmp == '<' ||
        *tmp == '?' ||
        *tmp == ':')
    {
        delete cur_token_;
        cur_token_ = new lexem(OP_LEX, cur_line_, ++lex_count_);
        if (*(tmp + 1) == '=')
        {
            switch(*tmp)
            {
                case '+':   cur_token_->set_op_type(OP_addeq);
                            tmp += 2;
                            break;
                case '-':   cur_token_->set_op_type(OP_subeq);
                            tmp += 2;
                            break; 
                case '*':   cur_token_->set_op_type(OP_muleq);
                            tmp += 2;
                            break; 
                case '/':   cur_token_->set_op_type(OP_diveq);
                            tmp += 2;
                            break; 
                case '<':   cur_token_->set_op_type(OP_lesseq);
                            tmp += 2;
                            break; 
                case '>':   cur_token_->set_op_type(OP_moreeq);
                            tmp += 2;
                            break; 
                default:    cur_token_->set_op_type(static_cast<OP>(*tmp++));
            }
        }
        else
            cur_token_->set_op_type(static_cast<OP>(*tmp++));
        cur_position_ = tmp - info_;
        return;
    }
    if (*tmp == '\0')
    {
        delete[] info_;
        info_ = nullptr;
        delete cur_token_;
        cur_token_ = new lexem(FULL_STOP, cur_line_, lex_count_);
        return;
    }
    delete cur_token_;
    fprintf(stderr, "error (umknown symbol)\nline %u, pos %u: %s\n", cur_line_, lex_count_, cur_str_);
    cur_token_ = nullptr; 
    exit(1);
}
