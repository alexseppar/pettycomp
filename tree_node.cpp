#include "tree_node.h"
#include <cstdio>
#include <cstdlib>

tree_node::tree_node(tree_node *lhs, tree_node *rhs) :
    lhs_    (lhs),
    rhs_    (rhs)
    {}

void tree_node::clean_up()
{
    if (lhs_)
    {
        delete lhs_;
        lhs_ = nullptr;
    }
    if (rhs_)
    {
        delete rhs_;
        rhs_ = nullptr;
    }
}

tree_node::~tree_node()
    {
       clean_up(); 
    }

tree_node* tree_node::get_lhs() const {
        return lhs_;
    }

tree_node* tree_node::get_rhs() const {
        return rhs_;
    }

num_node::num_node(tree_node *lhs, tree_node *rhs, double data) :
    tree_node::tree_node(lhs, rhs),
    data_   (data)
    {}

double num_node::get_data() const {
        return data_;
    }

num_node::~num_node()
{
    clean_up();
}

id_node::id_node(tree_node *lhs, tree_node *rhs, const char *name, unsigned line, unsigned pos) :
    tree_node::tree_node(lhs, rhs),
    name_   (name),
    line_   (line),
    pos_    (pos)
    {}

const char* id_node::get_name() const {
        return name_;
    }

void    id_node::set_rhs(tree_node *new_rhs) {
    rhs_ = new_rhs;
}

unsigned    id_node::get_line() const {
    return line_;
}

unsigned id_node:: get_pos() const {
    return pos_;
}

id_node::~id_node()
{
    delete[] name_;
    clean_up(); 
}

operator_node::operator_node(tree_node *lhs, tree_node *rhs, int type) :
    tree_node(lhs, rhs),
    type_   (type)
    {
        switch (type)
        {
            case '+':   priority_ = 0;
                        break;
            case '-':   priority_ = 0;
                        break;
            case '*':   priority_ = 1;
                        break;
            case '/':   priority_ = 1;
                        break;
            case '^':   priority_ = 3;
                        break;
            case 18:    priority_ = 2;
                        break;
            case 19:    priority_ = 2;
                        break;
            case '?':   priority_ = 0;
                        break;
            case ':':   priority_ = 0;
                        break;
            case '<':   priority_ = 0;
                        break;
            case '>':   priority_ = 0;
                        break;
            default:    fprintf(stderr, "unknown error \n");
                        exit(1);
        }
    }
int operator_node::get_operator_type() const {
        return type_;
    }
int operator_node::get_priority() const {
        return priority_;
    }
operator_node::~operator_node()
{
   clean_up(); 
}

eq_node::eq_node(tree_node *lhs, tree_node *rhs) :
    tree_node::tree_node(lhs, rhs)
    {}
eq_node::~eq_node()
{
   clean_up(); 
}

if_node::if_node(tree_node *lhs, tree_node *rhs) :
    tree_node(lhs, rhs)
    {}

if_node::~if_node()
{
    clean_up();
}

node_type id_node:: get_type() const {
    return ID_NODE;
}

node_type if_node:: get_type() const {
    return IF_NODE;
}

node_type num_node:: get_type() const {
    return NUM_NODE;
}

node_type eq_node:: get_type() const { 
    return EQ_NODE;
}

node_type operator_node:: get_type() const {
    return OP_NODE;
}

node_type while_node:: get_type() const {
    return WHILE_NODE;
}

while_node:: while_node(tree_node *lhs, tree_node *rhs):
tree_node(lhs, rhs)
{}

while_node:: ~while_node()
{
    clean_up();
}

connection_node:: connection_node(tree_node *lhs, tree_node *rhs):
tree_node(lhs, rhs)
{}

connection_node:: ~connection_node()
{
    clean_up();
}

node_type connection_node:: get_type() const {
    return CONNECTION_NODE;
}
