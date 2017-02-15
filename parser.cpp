#include "op_enum.h"
#include "parser.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <cmath>

enum lexems {
    NUM_LEX,
    ID_LEX,
    OP_LEX,
    FULL_STOP,
    END_OF_EXPR,
    EQ_LEX,
    CLOSE_BRACKET,
    OPEN_BRACKET,
    IF_LEX,
    ENDIF_LEX,
    CAPTURE_LEX,
    COMMA_LEX
};

const unsigned MAX_STR = 20;

unsigned lexem::get_line() const {
    return line_;
}

id_lexem::id_lexem(const char *name, unsigned line) :
    name_       (name)
    {
        line_ = line;
    }

id_lexem::~id_lexem() {}

int id_lexem::get_type() const {
        return ID_LEX;
    }

const char* id_lexem::get_name() const {
        return name_;
    }

num_lexem::num_lexem(double data, unsigned line) :
    data_       (data)
    {
        line_ = line;    
    }

int num_lexem::get_type() const {
        return NUM_LEX;
    }

double num_lexem::get_data() const {
        return data_;
    }

int capture_lexem:: get_type() const {
    return CAPTURE_LEX;
}

int if_lexem:: get_type() const {
    return IF_LEX;
}

int endif_lexem:: get_type() const {
    return ENDIF_LEX;
}

capture_lexem::capture_lexem(unsigned line)
{
    line_ = line;
}

if_lexem::if_lexem(unsigned line)
{
    line_ = line;
}

endif_lexem::endif_lexem(unsigned line)
{
    line_ = line;
}

equality_lexem::equality_lexem(unsigned line)
{
    line_ = line;
}

comma_lexem::comma_lexem(unsigned line)
{
    line_ = line;
}

open_bracket_lexem::open_bracket_lexem(unsigned line)
{
    line_ = line;
}

close_bracket_lexem::close_bracket_lexem(unsigned line)
{
    line_ = line;
}

end_of_expr_lexem::end_of_expr_lexem(unsigned line)
{
    line_ = line;
}

full_stop_lexem::full_stop_lexem(unsigned line)
{
    line_ = line;
}

operator_lexem::operator_lexem(int name, unsigned line) :
name_       (name)
{
     line_ = line;   
}

int operator_lexem::get_type() const {
        return OP_LEX;
    }
int operator_lexem::get_operator_type() const {
        return name_;
    }

int equality_lexem::get_type() const {
        return EQ_LEX;
    }

int open_bracket_lexem::get_type() const {
        return OPEN_BRACKET;
    }

int close_bracket_lexem::get_type() const {
        return CLOSE_BRACKET;
    }

int end_of_expr_lexem::get_type() const {
        return END_OF_EXPR;
    }

int full_stop_lexem::get_type() const {
        return FULL_STOP;
    }

int comma_lexem::get_type() const {
        return COMMA_LEX;
    }

lexer::lexer(const char *file_name) :
cur_token_      (nullptr),
cur_line_       (1),
cur_position_   (0)
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
    fclose(fp);
    mov_to_next_token();
}

lexer::~lexer()
{
    if (info_)
        delete[] info_;
    if (cur_token_)
        delete cur_token_;
}

lexem* lexer::get_cur_token() const {
    return cur_token_;
}

void lexer::mov_to_next_token()
{
    char *tmp = info_ + cur_position_;
    while ( *tmp == ' '  ||
            *tmp == '\t' ||
            *tmp == '\n')
    {
        if (*tmp == '\n')
            ++cur_line_;
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
                fprintf(stderr, "line %u: synthax error (expected digit after '.')\n", cur_line_);
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
        cur_token_ = new num_lexem(cur_num, cur_line_);
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
                fprintf(stderr, "line %u: too large name of variable %s\n", cur_line_, tmp - MAX_STR);
                delete[] cur_name;
                cur_token_ = nullptr;
                return;
            }
            cur_name[len_count++] = *tmp++;
        }
        if (strcmp(cur_name, "sin") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new operator_lexem(OP_sinus, cur_line_);
            cur_position_ = tmp - info_;
            return;
        }
        if (strcmp(cur_name, "cos") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new operator_lexem(OP_cosinus, cur_line_);
            cur_position_ = tmp - info_;
            return;
        }
        if (strcmp(cur_name, "if") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new if_lexem(cur_line_);
            cur_position_ = tmp - info_;
            return;
        }
        if (strcmp(cur_name, "endif") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new endif_lexem(cur_line_);
            cur_position_ = tmp - info_;
            return;
        }
        if (strcmp(cur_name, "capture") == 0)
        {
            delete[] cur_name;
            delete cur_token_;
            cur_token_ = new capture_lexem(cur_line_);
            cur_position_ = tmp - info_;
            return;
        }
        delete cur_token_;
        cur_position_ = tmp - info_;
        cur_token_ = new id_lexem(cur_name, cur_line_);
        return;
    }
    #define ROUTINE \
    ++tmp;\
    delete cur_token_;\
    cur_position_ = tmp - info_;
    
    switch (static_cast<int>(*tmp))
    {
        case '=':   ROUTINE
                    cur_token_ = new equality_lexem(cur_line_);
                    return;
        case '(':   ROUTINE
                    cur_token_ = new open_bracket_lexem(cur_line_);
                    return;
        case ')':   ROUTINE
                    cur_token_ = new close_bracket_lexem(cur_line_);
                    return;
        case ';':   ROUTINE
                    cur_token_ = new end_of_expr_lexem(cur_line_);
                    return;
        case ',':   ROUTINE
                    cur_token_ = new comma_lexem(cur_line_);
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
        cur_token_ = new operator_lexem(static_cast<int>(*tmp++), cur_line_);
        cur_position_ = tmp - info_;
        return;
    }
    if (*tmp == '\0')
    {
        delete[] info_;
        info_ = nullptr;
        delete cur_token_;
        cur_token_ = new full_stop_lexem(cur_line_);
        return;
    }
    fprintf(stderr, "line %u: synthax error (umknown symbol) %s\n", cur_line_, tmp);
    cur_token_ = nullptr; 
}

tree_node* parser::get_trig()
{
    if (get_cur_token()->get_type() == OP_LEX)
    {
        tree_node *tmp;
        switch (static_cast<operator_lexem*>(get_cur_token())->get_operator_type())
        {
            case OP_sinus:      mov_to_next_token();
                                assert(get_cur_token());
                                if (get_cur_token()->get_type() == OPEN_BRACKET)
                                {
                                    mov_to_next_token();
                                    tree_node *tmp_1 = get_expr();
                                    if (tmp_1 == nullptr)
                                        return nullptr;
                                    tmp = static_cast<tree_node*>(new operator_node(tmp_1, nullptr, OP_sinus));
                                    assert(get_cur_token());
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "line %u: expected ')' after sin-expr \n", get_cur_token()->get_line());
                                        delete tmp;
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "line %u: expected '(' after sin \n", get_cur_token()->get_line());
                                    return nullptr;
                                }
            case OP_cosinus:    mov_to_next_token();
                                assert(get_cur_token());
                                if (get_cur_token()->get_type() == OPEN_BRACKET)
                                {
                                    mov_to_next_token();
                                    tree_node *tmp_2 = get_expr();
                                    if (tmp_2 == nullptr)
                                        return nullptr;
                                    tmp = static_cast<tree_node*>(new operator_node(tmp_2, nullptr, OP_cosinus));
                                    assert(get_cur_token());
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "line %u: expected ')' after cos-expr \n", get_cur_token()->get_line());
                                        delete tmp; 
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "line %u: expected '(' after sin \n", get_cur_token()->get_line());
                                    return nullptr;
                                }
            default:    fprintf(stderr, "line %u: expected sin or cos instead of taken operator \n", get_cur_token()->get_line());
                        return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "line %u: expected sin or cos", get_cur_token()->get_line());
        return nullptr;
    }
}

tree_node* parser::get_num()
{
    if (get_cur_token()->get_type() == NUM_LEX)
    {
        tree_node *tmp = static_cast<tree_node*>\
                        (new num_node(nullptr, nullptr, static_cast<num_lexem*>(get_cur_token())->get_data()));
        mov_to_next_token();
        return tmp;
    }
    else if (get_cur_token()->get_type() == OP_LEX &&
             static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '-')
    {
        mov_to_next_token();
        if (get_cur_token()->get_type() == NUM_LEX)
        {
            tree_node *tmp = static_cast<tree_node*>\
            (new num_node(nullptr, nullptr, static_cast<num_lexem*>(get_cur_token())->get_data() * -1));
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "line %u: expected num after unary minus", get_cur_token()->get_line());
            return nullptr;
        }
    }
    else        
    {
        fprintf(stderr, "line %u: expected num \n", get_cur_token()->get_line());
        return nullptr;
    }
}

tree_node* parser::get_id()
{
    if (get_cur_token()->get_type() == ID_LEX)
    {
        tree_node *tmp = static_cast<tree_node*>\
                         (new id_node(nullptr, nullptr, static_cast<id_lexem*>(get_cur_token())->get_name(),\
                                        static_cast<id_lexem*>(get_cur_token())->get_line()));
        mov_to_next_token();
        return tmp;
    }
    else
    {
        fprintf(stderr, "line %u: expected id\n", get_cur_token()->get_line());
        return nullptr;
    }
}

tree_node* parser::get_p()
{
    tree_node *tmp;
    assert(get_cur_token());
    if (get_cur_token()->get_type() == OPEN_BRACKET)
    {
        mov_to_next_token();
        if ((tmp = get_expr()) == nullptr)
            return nullptr;
        assert(get_cur_token());
        if (get_cur_token()->get_type() == CLOSE_BRACKET)
        {
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "line %u: expected ')' after expr\n", get_cur_token()->get_line());
            delete tmp;
            return nullptr;
        }
    }
    else
    {
        assert(get_cur_token());
        if (get_cur_token()->get_type() == ID_LEX)
        {
            if ((tmp = get_id()) == nullptr)
                return nullptr;
            return tmp;
        }
        else if (get_cur_token()->get_type() == NUM_LEX)
        {
            if ((tmp = get_num()) == nullptr)
                return nullptr;
            return tmp;
        }
        else if (get_cur_token()->get_type() == OP_LEX)
        {
            if (static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '-')
                tmp = get_num();
            else
                tmp = get_trig();
            if (tmp == nullptr)
                return nullptr;
            return tmp;
        }
        else
        {
            fprintf(stderr, "line %u: expected id, num or expr\n", get_cur_token()->get_line());
            return nullptr;
        }
    }
}

tree_node* parser::get_pow()
{
    tree_node *tmp = get_p();
    if (tmp == nullptr)
        return nullptr;
    assert(get_cur_token());
    while (get_cur_token()->get_type() == OP_LEX  &&
           static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '^')
    {
        mov_to_next_token();
        tree_node *tmp_1 = get_p();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = static_cast<tree_node*>(new operator_node(tmp, tmp_1, '^'));
    }
    return tmp;
}

tree_node* parser::get_term()
{
    tree_node* tmp = get_pow();
    if (tmp == nullptr)
        return nullptr;
    assert(get_cur_token());
    while (get_cur_token()->get_type() == OP_LEX  &&
          (static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '*'      ||
           static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '/'))
    {
        char type = static_cast<operator_lexem*>(get_cur_token())->get_operator_type();
        mov_to_next_token();
        tree_node *tmp_1 = get_pow();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = static_cast<tree_node*>(new operator_node(tmp, tmp_1, type));
    }
    return tmp;
}

tree_node* parser::get_expr()
{
    tree_node *tmp = get_term();
    if (tmp == nullptr)
        return nullptr;
    assert(get_cur_token());
    while (get_cur_token()->get_type() == OP_LEX &&
        (static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '+' ||
         static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '-' ||
         static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '<' ||
         static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '>'))
    {
        char type = static_cast<operator_lexem*>(get_cur_token())->get_operator_type();
        mov_to_next_token();
        tree_node *tmp_1 = get_term();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = static_cast<tree_node*>(new operator_node(tmp, tmp_1, type));
        assert(get_cur_token());
    }
    if (get_cur_token()->get_type() == OP_LEX &&
        static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '?')
    {
        mov_to_next_token();
        tree_node *tmp_1 = get_expr();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        assert(get_cur_token());
        if (get_cur_token()->get_type() == OP_LEX &&
            static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == ':')
        {
            mov_to_next_token();
            tree_node *tmp_2 = get_expr();
            if (tmp_2 == nullptr)
            {
                delete tmp;
                delete tmp_1;
                return nullptr;
            }
            tmp_1 = static_cast<tree_node*>(new operator_node(tmp_1, tmp_2, ':'));
            tmp = static_cast<tree_node*>(new operator_node(tmp, tmp_1, '?'));
            return tmp;
        }
        else
        {
            delete tmp_1;
            delete tmp;
            fprintf(stderr, "line %u: expected ':' after '?' with following expr \n", get_cur_token()->get_line());
            return nullptr;
        }
    }
    return tmp;
}

tree_node* parser::get_capture()
{
    tree_node *tmp, *tmp_1;
    if (get_cur_token()->get_type() == OPEN_BRACKET)
    {
        mov_to_next_token();
        assert(get_cur_token());
        if (get_cur_token()->get_type() == ID_LEX)
        {
            tmp = get_id();
            assert(get_cur_token());
            if (get_cur_token()->get_type() == COMMA_LEX)
            {
                mov_to_next_token();
                assert(get_cur_token()); 
                while (get_cur_token()->get_type() == ID_LEX)
                {
                    tmp_1 = get_id();
                    static_cast<id_node*>(tmp_1)->set_rhs(tmp);
                    tmp = tmp_1;
                    assert(get_cur_token());
                    if (get_cur_token()->get_type() == COMMA_LEX)
                    {
                        mov_to_next_token();
                        assert(get_cur_token());
                    }
                }
                if (get_cur_token()->get_type() == CLOSE_BRACKET)
                {
                    mov_to_next_token();
                    assert(get_cur_token());
                    return tmp;
                }
                else
                {
                    fprintf(stderr, "line %u: expected comma after id in \"capture\" block\n", get_cur_token()->get_line());
                    delete tmp;
                    return nullptr;
                }
            }
            else
            {
                if (get_cur_token()->get_type() == CLOSE_BRACKET)
                {
                    mov_to_next_token();
                    assert(get_cur_token());
                    return tmp;
                }
                else
                {
                    fprintf(stderr, "line %u: expected comma after id in varlist \n", get_cur_token()->get_line());
                    delete tmp;
                    return nullptr;
                }
            }
        }
        else
        {
            if (get_cur_token()->get_type() == OP_LEX &&
                 static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '*')
            {
                mov_to_next_token();
                assert(get_cur_token());
                if (get_cur_token()->get_type() == CLOSE_BRACKET)
                {
                    mov_to_next_token();
                    assert(get_cur_token());
                    return static_cast<tree_node*>(new id_node(nullptr, nullptr, nullptr, 0));
                }
                else
                {
                    fprintf(stderr, "line %u: expected ')' at the end of \"capture\" block \n", get_cur_token()->get_line());
                    return nullptr;
                }
            }
            else
            {
                fprintf(stderr, "line %u: expected varlist in \"capture\" block \n", get_cur_token()->get_line());
                return nullptr;
            }
        }
    }
    else
    {
        fprintf(stderr, "line %u: expected open-bracket after \"capture\" \n", get_cur_token()->get_line());
        return nullptr;
    }
}

tree_node* parser::get_if()
{
    tree_node *tmp, *tmp_1, *tmp_2;
    if (get_cur_token()->get_type() == OPEN_BRACKET)
    {
        mov_to_next_token();
        assert(get_cur_token());
        tmp = get_expr();
        if (tmp == nullptr)
            return nullptr;
        assert(get_cur_token());
        if (get_cur_token()->get_type() == CLOSE_BRACKET)
        {
            mov_to_next_token();
            assert(get_cur_token());
            if (get_cur_token()->get_type() == CAPTURE_LEX)
            {
                mov_to_next_token();
                assert(get_cur_token());
                tmp_1 = get_capture();
                if (tmp_1 == nullptr)
                {
                    delete tmp;
                    return nullptr;
                }
                if (static_cast<id_node*>(tmp_1)->get_name() == nullptr)
                    tmp_1 = nullptr;
                assert(get_cur_token());
                tmp_2 = get_equalities(false);
                if (tmp_2 == nullptr)
                {
                    delete tmp;
                    delete tmp_1;
                    return nullptr;
                }
                assert(get_cur_token());
                if (get_cur_token()->get_type() == ENDIF_LEX)
                {
                    mov_to_next_token();
                    tmp_2 = new tree_node(tmp_2, tmp_1);
                    tmp = static_cast<tree_node*>(new if_node(tmp, tmp_2));
                    return tmp;
                }
                else
                { 
                    fprintf(stderr, "line %u: no endif operator \n", get_cur_token()->get_line());
                    delete tmp;
                    delete tmp_1;
                    delete tmp_2;
                    return nullptr;
                }
            }
            else
            {
                tmp_1 = get_equalities(false);
                if (tmp_1 == nullptr)
                {
                    delete tmp;
                    return nullptr;
                }
                assert(get_cur_token());
                if (get_cur_token()->get_type() == ENDIF_LEX)
                {
                    mov_to_next_token();
                    tmp_1 = new tree_node(tmp_1, nullptr);
                    tmp = static_cast<tree_node*>(new if_node(tmp, tmp_1));
                    return tmp;
                }
                else
                {
                    fprintf(stderr, "line %u: no endif operator \n", get_cur_token()->get_line());
                    delete tmp; 
                    delete tmp_1;
                    return nullptr;
                }
            }
        }
        else
        {
            fprintf(stderr, "line %u: expected ')' after if-expr \n", get_cur_token()->get_line());
            delete tmp;
            return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "line %u: expected '(' after \"if\" \n", get_cur_token()->get_line());
        return nullptr;
    }
}

tree_node* parser::get_equality()
{
    tree_node *tmp;
    if (get_cur_token()->get_type() == ID_LEX)
        tmp = get_id();
    else if (get_cur_token()->get_type() == IF_LEX)
    {
        mov_to_next_token();
        assert(get_cur_token());
        tmp = get_if();
        if (tmp == nullptr)
            return nullptr;
        return tmp;
    }
    if (tmp == nullptr)
        return nullptr;
    assert(get_cur_token());
    if (get_cur_token()->get_type() == EQ_LEX)
    {
        mov_to_next_token();
        tree_node *tmp_1 = get_expr();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = static_cast<tree_node*>(new eq_node(tmp, tmp_1));
        assert(get_cur_token());
        if (get_cur_token()->get_type() == END_OF_EXPR)
        {
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "line %u: synthax error (expected ';' in the end of expr)\n", get_cur_token()->get_line());
            delete tmp;
            return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "line %u: synthax error (expected '=' after variable)\n", get_cur_token()->get_line());
        delete tmp;
        return nullptr;
    }
}

tree_node* parser::get_equalities(bool open_scope)
{
    tree_node *tmp = get_equality();
    if (tmp == nullptr)
        return nullptr;
    assert(get_cur_token());
    while ( get_cur_token()->get_type() == ID_LEX ||
            get_cur_token()->get_type() == IF_LEX)
    {
        tree_node *tmp_1 = get_equality();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = new tree_node(tmp, tmp_1);
    }
    assert(get_cur_token());
    if (get_cur_token()->get_type() == FULL_STOP)
        return tmp;
    else
    {
        if (open_scope)
        {
            fprintf(stderr, "line %u: expected id after last equality \n", get_cur_token()->get_line());
            delete tmp;
            return nullptr;
        }
        else
        {
            if (get_cur_token()->get_type() == ENDIF_LEX)
                return tmp;
            else
            {
                fprintf(stderr, "UNKNOWN! \n");
                delete tmp;
                return nullptr;
            }
        }
    }
}

bool parser::build_tree()
{
    if ((tree_ = get_equalities(true)) == nullptr)
        return false; 
    return true;
}

tree_node* parser::get_tree() const {
    return tree_;
}

void parser::mov_to_next_token() {
    last_line_ = lxr_.get_cur_token()->get_line();
    lxr_.mov_to_next_token();
}

lexem* parser::get_cur_token() const {
    return lxr_.get_cur_token();
}

parser::parser(const char *file_name) :
lxr_        (file_name),
tree_       (nullptr),
last_line_  (1)
{}

parser::~parser()
{
    if (tree_)
        delete tree_;
}
