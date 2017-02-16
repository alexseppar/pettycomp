#include "interpretator.h"
#include <typeinfo>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

static double calculate(tree_node *tree, scope &scope_, bool *err)
{
    if (tree->get_type() == CONNECTION_NODE)
    {
        calculate(tree->get_lhs(), scope_, err);
        calculate(tree->get_rhs(), scope_, err);
        return 0;
    }
    else if (tree->get_type() == EQ_NODE)
    { 
        const char *tmp_name = static_cast<id_node*>(tree->get_lhs())->get_name();
        #ifdef DEBUG__
            fprintf(stderr, "%s = ", tmp_name);
        #endif

        scope_[scope_.get_last()]->add(tmp_name, calculate(tree->get_rhs(), scope_, err));
        
        #ifdef DEBUG__
            fprintf(stderr, ";\nvarlist:\n");
            scope_[scope_.get_last()]->print_info();
        #endif
        return 0;
    }
    else if (tree->get_type() == OP_NODE)
    {
        #define DEF_OP(name, num, code) \
        case num:   code\
                    break;

        switch(static_cast<operator_node*>(tree)->get_operator_type())
        {
            #include "cmd.h"

            default:    fprintf(stdout, "unknown error\n");
                        exit(1);
        }
        #undef DEF_OP
    }
    else if (tree->get_type() == NUM_NODE)
    {
        double tmp_data = static_cast<num_node*>(tree)->get_data();
        #ifdef DEBUG__
            fprintf(stderr, "%lg ", tmp_data);
        #endif
        return tmp_data;
    }
    else if (tree->get_type() == ID_NODE)
    {
        list_mem *tmp = scope_[scope_.get_last()]->find_mem(static_cast<id_node*>(tree)->get_name());
        #ifdef DEBUG__
        fprintf(stderr, "%s ", static_cast<id_node*>(tree)->get_name());
        #endif
        if (tmp == nullptr)
        {
            fprintf(stdout, "error\nline %u: variable '%s' was not defined in this scope\n", static_cast<id_node*>(tree)->get_line(),\
                                static_cast<id_node*>(tree)->get_name());
            exit(1);
        }
        else
            return tmp->get_data();
    }
    else if (tree->get_type() == IF_NODE)
    {
        #ifdef DEBUG__
        fprintf(stderr, "if (");
        #endif
        if (calculate(tree->get_lhs(), scope_, err))
        {
            #ifdef DEBUG__
            fprintf(stderr, ") ");
            #endif
            tree_node *tmp = tree->get_rhs()->get_rhs();
            if (tmp)
            {
                scope_.add_like_prev();
                #ifdef DEBUG__
                tree_node *tmp_ = tmp;
                fprintf(stderr, "capture (");
                while (tmp_)
                {
                    fprintf(stderr, "%s", static_cast<id_node*>(tmp_)->get_name());
                    tmp_ = tmp_->get_rhs();
                    if (tmp_)
                        fprintf(stderr, ", ");
                    else
                    {
                        fprintf(stderr, ")\n");
                        break;
                    }
                }
                #endif
            }
            calculate(tree->get_rhs()->get_lhs(), scope_, err);
            if (tmp)
            {
                list_mem *tmp_1;
                const char *cur_name;
                while (tmp)
                { 
                    cur_name = static_cast<id_node*>(tmp)->get_name();
                    tmp_1 = scope_[scope_.get_last() - 1]->find_mem(cur_name);
                    if (tmp_1)
                        tmp_1->set_data(scope_[scope_.get_last()]->find_mem(cur_name)->get_data());
                    else
                    {
                        fprintf(stdout, "error\nline %u: variable '%s' was not defined in this scope (in capture block)\n", \
                                    static_cast<id_node*>(tmp)->get_line(), static_cast<id_node*>(tmp)->get_name());
                        exit(1);
                    }
                    tmp = tmp->get_rhs();
                }
                scope_.delete_last();
            }
            #ifdef DEBUG__
            fprintf(stderr, "endif\n");
            #endif
            return 0;
        }
        return 0;
    }
    else
    {
        fprintf(stderr, "unknown error \n");
        exit(1);
    }
    fprintf(stderr, "unknown error \n");
    exit(1);
}

bool interpretator::compute()
{
    bool err = false;
    scope scope_(10);
    calculate(tree_, scope_, &err);
    if (!err)
    {
        list_mem *tmp = scope_[0]->find_mem("result");
        if (tmp)
        {
            answer_ = tmp->get_data();
            return true;
        }
        else
        {
            fprintf(stderr, "no key-word 'result' \n");
            return false;
        }
    }
    else
        return false;
}

double interpretator::get_answer() const {
    return answer_;
}

static tree_node* copy_tree(tree_node *tree)
{
    if (!tree)
        return nullptr;
    if (tree->get_type() == CONNECTION_NODE)
        return new connection_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()));
    else if (tree->get_type() == EQ_NODE)
        return static_cast<tree_node*>(new eq_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs())));
    else if (tree->get_type() == NUM_NODE)
        return static_cast<tree_node*>(new num_node(nullptr, nullptr, static_cast<num_node*>(tree)->get_data()));
    else if (tree->get_type() == ID_NODE)
        return static_cast<tree_node*>(new id_node(nullptr, copy_tree(tree->get_rhs()),\
                                            static_cast<id_node*>(tree)->get_name(), static_cast<id_node*>(tree)->get_line()));
    else if (tree->get_type() == OP_NODE)
        return static_cast<tree_node*>\
        (new operator_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()), static_cast<operator_node*>(tree)->get_operator_type()));
    else if (tree->get_type() == IF_NODE)
        return static_cast<tree_node*>(new if_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()))); 
    return nullptr;
}

interpretator::interpretator(tree_node *tree) :
answer_     (0)
{
    assert(tree);
    tree_ = copy_tree(tree);
    assert(tree_);
}

interpretator::~interpretator()
{
    delete tree_;
}

scope:: scope(unsigned size):
size_   (size),
last_   (0),
tables_ (new hash_table*[size_]())
{
    tables_[0] = new hash_table(100);
}

scope:: ~scope()
{
    while (last_ != 0)
        delete tables_[last_--];
    delete tables_[last_--];
    delete tables_;
}

void scope:: add_like_prev()
{
    if (last_ >= size_)
    {
        fprintf(stderr, "stack overflow \n");
        exit(1);
    }
    else
    {
        tables_[last_ + 1] = new hash_table(*(tables_[last_]));
        ++last_;
    }
}

void scope:: add_new()
{
    if (last_ >= size_)
    {
        fprintf(stderr, "stack overflow \n");
        exit(1);
    }
    else
        tables_[++last_] = new hash_table(100);
}

void scope:: delete_last()
{
    delete tables_[last_];
    tables_[last_--] = nullptr;
}

unsigned scope:: get_last() const
{
    return last_;
}

hash_table* scope:: operator[] (unsigned idx)
{
    assert(idx < size_);
    return tables_[idx];
}
