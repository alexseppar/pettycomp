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
    OPEN_BRACKET
};

const unsigned MAX_STR = 20;

id_lexem::id_lexem(const char *name) :
    name_   (name)
    {}
id_lexem::~id_lexem() {
        delete[] name_;
    }
int id_lexem::get_type() const {
        return ID_LEX;
    }
const char* id_lexem::get_name() const {
        return name_;
    }

num_lexem::num_lexem(double data) :
    data_   (data)
    {}
int num_lexem::get_type() const {
        return NUM_LEX;
    }
double num_lexem::get_data() const {
        return data_;
    }

operator_lexem::operator_lexem(int name) :
    name_   (name)
    {}

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

lexer::lexer(const char *file_name) :
    lex_count_  (0),
    id_count_   (0),
    cur_token_ (0)
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
        tokens_ = new lexem*[file_size];
    }
lexer::~lexer()
    {
        for (unsigned i = 0; i < lex_count_; ++i)
            delete tokens_[i];
        delete[] tokens_;
        if (info_)
            delete[] info_;
    }

void lexer::mov_to_next_token() {
    ++cur_token_;
}

lexem* lexer::get_cur_token() const {
    return tokens_[cur_token_];
}

unsigned lexer::get_num_id() const {
    return id_count_;
}
    bool lexer::build_tokens()
    {
        char *tmp = info_;
        while (*tmp)
        {
            if (*tmp == ' '  ||
                *tmp == '\n' ||
                *tmp == '\t')
            {
                ++tmp;
                continue;
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
                        fprintf(stderr, "synthax error (expected digit after '.')\n");
                        return false;
                    }
                    unsigned pow_count = 2;
                    while (isdigit(*tmp))
                        cur_num_2 += (*tmp++ - '0') / pow(10, pow_count++); 
                    cur_num += cur_num_2;
                }
                tokens_[lex_count_++] = new num_lexem(cur_num);
                continue;
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
                        fprintf(stderr, "too large name of variable %s\n", tmp - MAX_STR);
                        delete[] cur_name;
                        return false;
                    }
                    cur_name[len_count++] = *tmp++;
                }
                if (strcmp(cur_name, "sin") == 0)
                {
                    tokens_[lex_count_++] = new operator_lexem(OP_sinus);
                    delete[] cur_name;
                    continue;
                }
                if (strcmp(cur_name, "cos") == 0)
                {
                    tokens_[lex_count_++] = new operator_lexem(OP_cosinus);
                    delete[] cur_name;
                    continue;
                }
                tokens_[lex_count_++] = new id_lexem(cur_name);
                ++id_count_;
                continue;
            }
            if (*tmp == '=')
            {
                tokens_[lex_count_++] = new equality_lexem;
                ++tmp;
                continue;
            }
            if (*tmp == '(')
            {
                tokens_[lex_count_++] = new open_bracket_lexem;
                ++tmp;
                continue;
            }
            if (*tmp == ')')
            {
                tokens_[lex_count_++] = new close_bracket_lexem;
                ++tmp;
                continue;
            }
            if (*tmp == ';')
            {
                tokens_[lex_count_++] = new end_of_expr_lexem;
                ++tmp;
                continue;
            }
            if (*tmp == '+' ||
                *tmp == '-' ||
                *tmp == '*' ||
                *tmp == '/' ||
                *tmp == '^')
            {
                tokens_[lex_count_++] = new operator_lexem(static_cast<int>(*tmp++));
                continue;
            }
            fprintf(stderr, "synthax error (umknown symbol) %s\n", tmp);
            return false;
        }
        tokens_[lex_count_++] = new full_stop_lexem;
        delete[] info_;
        info_ = nullptr;
        return true;
    }

tree_node* parser::get_trig()
{
    if (get_cur_token()->get_type() == OP_LEX)
    {
        tree_node *tmp;
        switch (static_cast<operator_lexem*>(get_cur_token())->get_operator_type())
        {
            case OP_sinus:      mov_to_next_token();
                                if (get_cur_token()->get_type() == OPEN_BRACKET)
                                {
                                    mov_to_next_token();
                                    tree_node *tmp_1 = get_expr();
                                    if (tmp_1 == nullptr)
                                        return nullptr;
                                    tmp = static_cast<tree_node*>(new operator_node(tmp_1, nullptr, OP_sinus));
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "expected ')' after sin-expr \n");
                                        delete tmp;
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "expected '(' after sin \n");
                                    return nullptr;
                                }
            case OP_cosinus:    mov_to_next_token();
                                if (get_cur_token()->get_type() == OPEN_BRACKET)
                                {
                                    mov_to_next_token();
                                    tree_node *tmp_2 = get_expr();
                                    if (tmp_2 == nullptr)
                                        return nullptr;
                                    tmp = static_cast<tree_node*>(new operator_node(tmp_2, nullptr, OP_cosinus));
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "expected ')' after cos-expr \n");
                                        delete tmp; 
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "expected '(' after sin \n");
                                    return nullptr;
                                }
            default:    fprintf(stderr, "expected sin or cos instead of taken operator \n");
                        return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "expected sin or cos");
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
                fprintf(stderr, "expected num after unary minus");
                return nullptr;
            }
        }
        else        
        {
            fprintf(stderr, "synthax error (expected num)\n");
            return nullptr;
        }
    }
tree_node* parser::get_id()
    {
        if (get_cur_token()->get_type() == ID_LEX)
        {
            tree_node *tmp = static_cast<tree_node*>\
                             (new id_node(nullptr, nullptr, static_cast<id_lexem*>(get_cur_token())->get_name()));
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "synthax error (expected id)\n");
            return nullptr;
        }
    }
    tree_node* parser::get_p()
    {
        tree_node *tmp;
        if (get_cur_token()->get_type() == OPEN_BRACKET)
        {
            mov_to_next_token();
            if ((tmp = get_expr()) == nullptr)
                return nullptr;
            if (get_cur_token()->get_type() == CLOSE_BRACKET)
            {
                mov_to_next_token();
                return tmp;
            }
            else
            {
                fprintf(stderr, "synthax error (expected ')' after expr)\n");
                delete tmp;
                return nullptr;
            }
        }
        else if (get_cur_token()->get_type() == ID_LEX)
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
            fprintf(stderr, "synthax error (expected id, num or expr)\n");
            return nullptr;
        }
    }
    tree_node* parser::get_pow()
    {
        tree_node *tmp = get_p();
        if (tmp == nullptr)
            return nullptr;
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
        while (get_cur_token()->get_type() == OP_LEX &&
            (static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '+' ||
             static_cast<operator_lexem*>(get_cur_token())->get_operator_type() == '-'))
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
        }
        return tmp;
    }
    tree_node* parser::get_equality()
    {
        tree_node *tmp = get_id();
        if (tmp == nullptr)
            return nullptr;
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
            if (get_cur_token()->get_type() == END_OF_EXPR)
            {
                mov_to_next_token();
                return tmp;
            }
            else
            {
                fprintf(stderr, "synthax error (expected ';' in the end of expr)\n");
                delete tmp;
                return nullptr;
            }
        }
        else
        {
            fprintf(stderr, "synthax error (expected '=' after variable)\n");
            delete tmp;
            return nullptr;
        }
    }
    tree_node* parser::get_equalities()
    {
        tree_node *tmp = get_equality();
        if (tmp == nullptr)
            return nullptr;
        while (get_cur_token()->get_type() == ID_LEX)
        {
            tree_node *tmp_1 = get_equality();
            if (tmp_1 == nullptr)
            {
                delete tmp;
                return nullptr;
            }
            tmp = new tree_node(tmp, tmp_1);
        }
        if (get_cur_token()->get_type() == FULL_STOP)
            return tmp;
        else
        {
            fprintf(stderr, "expected id after last equality \n");
            delete tmp;
            return nullptr;
        }
    }
bool parser::build_tree()
    {
        if(!lxr_->build_tokens())
            return false;
        if ((tree_ = get_equalities()) == nullptr)
            return false; 
        return true;
    }
tree_node* parser::get_tree() const {
    return tree_;
}

void parser::mov_to_next_token() {
    lxr_->mov_to_next_token();
}

lexem* parser::get_cur_token() const {
    return lxr_->get_cur_token();
}

parser::parser(const char *file_name) :
tree_   (nullptr),
lxr_    (new lexer(file_name))
{}
parser::~parser()
{
    delete lxr_;
    if (tree_)
        delete tree_;
}
