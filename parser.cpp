#include "parser.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <cmath>
#include <algorithm>

#define LINE get_cur_token()->get_line()

#define POS get_cur_token()->get_pos()

#define STR get_cur_str()

struct node_holder {
    private:
    tree_node *node_;
    public:
    node_holder(tree_node *node = nullptr): node_ (node) {}
    node_holder(node_holder &&rhs): node_ (rhs.node_) { rhs.node_ = nullptr; }
    ~node_holder() { if (node_ != nullptr) delete node_; }
    node_holder& operator= (node_holder &&rhs) { node_ = rhs.node_; rhs.node_ = nullptr; return *this; }
    tree_node* release() { tree_node *tmp = node_; node_ = nullptr; return tmp; }
    bool is_null() const { return node_ ? false : true; }
};

tree_node* parser::get_trig()
{
    if (get_cur_token()->get_type() == OP_LEX)
    {
        tree_node *tmp;
        switch (get_cur_token()->get_operator_type())
        {
            case OP_sinus:      mov_to_next_token();
                                assert(get_cur_token());
                                if (get_cur_token()->get_type() == OPEN_BRACKET)
                                {
                                    mov_to_next_token();
                                    tree_node *tmp_1 = get_expr();
                                    if (tmp_1 == nullptr)
                                        return nullptr;
                                    tmp = new operator_node(tmp_1, nullptr, OP_sinus);
                                    assert(get_cur_token());
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "error: expected ')' after sin-expr\nline %u, pos %u: %s\n", LINE, POS, STR);
                                        delete tmp;
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "error: expected '(' after sin\nline %u, pos %u: %s\n", LINE, POS, STR);
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
                                    tmp = new operator_node(tmp_2, nullptr, OP_cosinus);
                                    assert(get_cur_token());
                                    if (get_cur_token()->get_type() == CLOSE_BRACKET)
                                    {
                                        mov_to_next_token();
                                        return tmp;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "error: expected ')' after cos-expr\nline %u, pos %u: %s\n", LINE, POS, STR);
                                        delete tmp; 
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "error: expected '(' after sin\nline %u, pos %u: %s\n", LINE, POS, STR);
                                    return nullptr;
                                }
            default:    fprintf(stderr, "error: expected sin or cos instead of taken operator\nline %u, pos %u: %s\n", LINE, POS, STR);
                        return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "error: expected sin or cos\nline %u, pos %u: %s", LINE, POS, STR);
        return nullptr;
    }
}

tree_node* parser::get_num()
{
    if (get_cur_token()->get_type() == NUM_LEX)
    {
        tree_node *tmp = new num_node(nullptr, nullptr, get_cur_token()->get_val());
        mov_to_next_token();
        return tmp;
    }
    else if (get_cur_token()->get_type() == OP_LEX &&
             get_cur_token()->get_operator_type() == '-')
    {
        mov_to_next_token();
        if (get_cur_token()->get_type() == NUM_LEX)
        {
            tree_node *tmp = new num_node(nullptr, nullptr, get_cur_token()->get_val() * -1);
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "error: expected num after unary minus\nline %u, pos %u: %s", LINE, POS, STR);
            return nullptr;
        }
    }
    else        
    {
        fprintf(stderr, "error: expected num\nline %u, pos %u: %s\n", LINE, POS, STR);
        return nullptr;
    }
}

tree_node* parser::get_id()
{
    if (get_cur_token()->get_type() == ID_LEX)
    {
        tree_node *tmp = new id_node(nullptr, nullptr, get_cur_token()->get_name(), LINE, POS);
        mov_to_next_token();
        return tmp;
    }
    else
    {
        fprintf(stderr, "error: expected id\nline %u, pos %u: %s\n", LINE, POS, STR);
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
            fprintf(stderr, "error: expected ')' after expr\nline %u, pos %u: %s\n", LINE, POS, STR);
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
            if (get_cur_token()->get_operator_type() == '-')
                tmp = get_num();
            else
                tmp = get_trig();
            if (tmp == nullptr)
                return nullptr;
            return tmp;
        }
        else
        {
            fprintf(stderr, "error: expected id, num or expr\nline %u, pos %u: %s\n", LINE, POS, STR);
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
           get_cur_token()->get_operator_type() == '^')
    {
        mov_to_next_token();
        tree_node *tmp_1 = get_p();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = new operator_node(tmp, tmp_1, '^');
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
          (get_cur_token()->get_operator_type() == '*' ||
           get_cur_token()->get_operator_type() == '/'))
    {
        char type = get_cur_token()->get_operator_type();
        mov_to_next_token();
        tree_node *tmp_1 = get_pow();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = new operator_node(tmp, tmp_1, type);
    }
    return tmp;
}

tree_node* parser::get_expr()
{
    node_holder term = get_term();
    if (term.is_null())
        return nullptr;
    assert(get_cur_token());
    while (get_cur_token()->get_type() == OP_LEX &&
        (get_cur_token()->get_operator_type() == '+' ||
         get_cur_token()->get_operator_type() == '-' ||
         get_cur_token()->get_operator_type() == '<' ||
         get_cur_token()->get_operator_type() == '>'))
    {
        char type = get_cur_token()->get_operator_type();
        mov_to_next_token();
        node_holder newterm = get_term();
        if (newterm.is_null())
            return nullptr;
        node_holder newop = new operator_node(term.release(), newterm.release(), type); 
        term = std::move(newop);
        assert(get_cur_token());
    }
    if (get_cur_token()->get_type() == OP_LEX &&
        get_cur_token()->get_operator_type() == '?')
    {
        mov_to_next_token();
        node_holder expr = get_expr();
        if (expr.is_null())
            return nullptr;
        assert(get_cur_token());
        if (get_cur_token()->get_type() == OP_LEX &&
            get_cur_token()->get_operator_type() == ':')
        {
            mov_to_next_token();
            node_holder expr_2 = get_expr();
            if (expr_2.is_null())
                return nullptr;
            node_holder connect = new connection_node(expr.release(), expr_2.release());
            node_holder newop = new operator_node(term.release(), connect.release(), '?');
            return newop.release();
        }
        else
        {
            fprintf(stderr, "error: expected ':' after '?' with following expr\nline %u, pos %u: %s\n", LINE, POS, STR);
            return nullptr;
        }
    }
    return term.release();
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
                    fprintf(stderr, "error: expected comma after id in \"capture\" block\nline %u, pos %u: %s\n", LINE, POS, STR);
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
                    fprintf(stderr, "error: expected comma after id in varlist\nline %u, pos %u: %s\n", LINE, POS, STR);
                    delete tmp;
                    return nullptr;
                }
            }
        }
        else
        {
            if (get_cur_token()->get_type() == OP_LEX &&
                 get_cur_token()->get_operator_type() == '*')
            {
                mov_to_next_token();
                assert(get_cur_token());
                if (get_cur_token()->get_type() == CLOSE_BRACKET)
                {
                    mov_to_next_token();
                    assert(get_cur_token());
                    return new id_node(nullptr, nullptr, nullptr, 0, 0);
                }
                else
                {
                    fprintf(stderr, "error: expected ')' at the end of \"capture\" block\nline %u, pos %u: %s\n", LINE, POS, STR);
                    return nullptr;
                }
            }
            else
            {
                fprintf(stderr, "error: expected varlist in \"capture\" block\nline %u, pos %u: %s\n", LINE, POS, STR);
                return nullptr;
            }
        }
    }
    else
    {
        fprintf(stderr, "error: expected open-bracket after \"capture\"\nline %u, pos %u: %s\n", LINE, POS, STR);
        return nullptr;
    }
}

tree_node* parser::get_if()
{ 
    if (get_cur_token()->get_type() == OPEN_BRACKET)
    {
        mov_to_next_token();
        assert(get_cur_token());
        node_holder expr = get_expr();
        if (expr.is_null())
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
                node_holder capture = get_capture();
                if (capture.is_null())
                    return nullptr;
                assert(get_cur_token());
                node_holder equalities = get_equalities(false);
                if (equalities.is_null())
                    return nullptr;
                assert(get_cur_token());
                if (get_cur_token()->get_type() == ENDIF_LEX)
                {
                    mov_to_next_token();
                    node_holder connect = new connection_node(equalities.release(), capture.release());
                    node_holder newif = new if_node(expr.release(), connect.release());
                    return newif.release();
                }
                else
                { 
                    fprintf(stderr, "error: no endif operator\nline %u, pos %u:  %s\n", LINE, POS, STR);
                    return nullptr;
                }
            }
            else
            {
                node_holder equalities = get_equalities(false);
                if (equalities.is_null())
                    return nullptr;
                assert(get_cur_token());
                if (get_cur_token()->get_type() == ENDIF_LEX)
                {
                    mov_to_next_token();
                    node_holder connect = new connection_node(equalities.release(), nullptr);
                    node_holder newif = new if_node(expr.release(), connect.release());
                    return newif.release();
                }
                else
                {
                    fprintf(stderr, "error: no endif operator\nline %u, pos %u: %s\n", LINE, POS, STR);
                    return nullptr;
                }
            }
        }
        else
        {
            fprintf(stderr, "error: expected ')' after if-expr\nline %u, pos %u: %s\n", LINE, POS, STR);
            return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "error: expected '(' after \"if\"\nline %u, pos %u: %s\n", LINE, POS, STR);
        return nullptr;
    }
}

tree_node*  parser::get_while()
{
    if (get_cur_token()->get_type() == OPEN_BRACKET)
    {
        mov_to_next_token();       
        node_holder expr = get_expr();
        if (expr.is_null())
            return nullptr;
        if (get_cur_token()->get_type() == CLOSE_BRACKET)
        {
            mov_to_next_token();
            if (get_cur_token()->get_type() == CAPTURE_LEX)
            {
                mov_to_next_token();
                node_holder capture = get_capture();
                if (capture.is_null())
                    return nullptr;
                node_holder equalities = get_equalities(false);
                if (equalities.is_null())
                    return nullptr;
                if (get_cur_token()->get_type() == ENDWHILE_LEX)
                {
                    mov_to_next_token();
                    node_holder connect = new connection_node(equalities.release(), capture.release());
                    node_holder newwhile = new while_node(expr.release(), connect.release());
                    return newwhile.release();
                }
                else
                {
                    fprintf(stdout, "error: expected 'endwhile' before\nline %u, pos %u: %s\n", LINE, POS, STR);
                    return nullptr;
                }
            }
            else
            {
                node_holder equalities = get_equalities(false);
                if (equalities.is_null())
                    return nullptr;
                if (get_cur_token()->get_type() == ENDWHILE_LEX)
                {
                    mov_to_next_token();
                    node_holder connect = new connection_node(equalities.release(), nullptr);
                    node_holder newwhile = new while_node(expr.release(), connect.release());
                    return newwhile.release();
                }
                else
                {
                    fprintf(stdout, "error: expected 'endwhile' before\nline %u, pos %u: %s\n", LINE, POS, STR);
                    return nullptr;
                }
            }
        }
        else
        {
            fprintf(stdout, "error: expected ')' after while-expr\nline %u, pos %u: %s\n", LINE, POS, STR);
            return nullptr;
        }
    }
    else
    {
        fprintf(stdout, "error: expected '(' after 'while'\nline %u, pos %u: %s\n", LINE, POS, STR);
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
    else if (get_cur_token()->get_type() == WHILE_LEX)
    {
        mov_to_next_token();
        tmp = get_while();
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
        tmp = new eq_node(tmp, tmp_1);
        assert(get_cur_token());
        if (get_cur_token()->get_type() == END_OF_EXPR)
        {
            mov_to_next_token();
            return tmp;
        }
        else
        {
            fprintf(stderr, "error: expected ';' in the end of expr\nline %u, pos %u: %s\n", LINE, POS, STR);
            delete tmp;
            return nullptr;
        }
    }
    else
    {
        fprintf(stderr, "error: expected '=' after variable\nline %u, pos %u: %s\n", LINE, POS, STR);
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
            get_cur_token()->get_type() == IF_LEX ||
            get_cur_token()->get_type() == WHILE_LEX)
    {
        tree_node *tmp_1 = get_equality();
        if (tmp_1 == nullptr)
        {
            delete tmp;
            return nullptr;
        }
        tmp = new connection_node(tmp, tmp_1);
    }
    assert(get_cur_token());
    if (get_cur_token()->get_type() == FULL_STOP)
        return tmp;
    else
    {
        if (open_scope)
        {
            fprintf(stderr, "error: expected id, if or while after last equality\nline %u, pos %u: %s\n", LINE, POS, STR);
            delete tmp;
            return nullptr;
        }
        else
        {
            if (get_cur_token()->get_type() == ENDIF_LEX ||
                get_cur_token()->get_type() == ENDWHILE_LEX)
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

const char* parser:: get_cur_str() const {
    return lxr_.get_cur_str();
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
