#include "interpretator.h"
#include <typeinfo>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

static double calculate(tree_node *tree, hash_table *ht, bool *err)
{
    if (typeid(*tree) == typeid(tree_node))
    {
        calculate(tree->get_lhs(), ht, err);
        calculate(tree->get_rhs(), ht, err);
        return 0;
    }
    else if (typeid(*tree) == typeid(eq_node))
    { 
        const char *tmp_name = static_cast<id_node*>(tree->get_lhs())->get_name();
        #ifdef DEBUG__
            fprintf(stdout, "%s = ", tmp_name);
        #endif

        ht->add(tmp_name, calculate(tree->get_rhs(), ht, err));
        
        #ifdef DEBUG__
            fprintf(stdout, ";\nvarlist:\n");
            ht->print_info();
        #endif
        return 0;
    }
    else if (typeid(*tree) == typeid(operator_node))
    {
        #define DEF_OP(name, num, code) \
        case num:   code\
                    break;

        switch(static_cast<operator_node*>(tree)->get_type())
        {
            #include "cmd.h"

            default:    fprintf(stderr, "unknown error \n");
                        exit(1);
        }
        #undef DEF_OP
    }
    else if (typeid(*tree) == typeid(num_node))
    {
        double tmp_data = static_cast<num_node*>(tree)->get_data();
        #ifdef DEBUG__
            fprintf(stdout, "%lg ", tmp_data);
        #endif
        return tmp_data;
    }
    else if (typeid(*tree) == typeid(id_node))
    {
        list_mem *tmp = ht->find_mem(static_cast<id_node*>(tree)->get_name());
        if (tmp == nullptr)
        {
            fprintf(stderr, "error: variable `%s' was used but not defined yet \n", static_cast<id_node*>(tree)->get_name());
            *err = true;
            return -1;
        }
        else
        {
            #ifdef DEBUG__
                fprintf(stdout, "%s ", tmp->get_name());
            #endif
            return tmp->get_data();
        }
    }
    else
    {
        fprintf(stderr, "unknown error \n");
        *err = true;
        return -1;
    }
}

bool interpretator::compute()
{
    ht_ = new hash_table(100);
    bool err = false;
    calculate(tree_, ht_, &err);
    if (!err)
    {
        list_mem *tmp = ht_->find_mem("result");
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
    if (typeid(*tree) == typeid(tree_node))
        return new tree_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()));
    else if (typeid(*tree) == typeid(eq_node))
        return static_cast<tree_node*>(new eq_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs())));
    else if (typeid(*tree) == typeid(num_node))
        return static_cast<tree_node*>(new num_node(nullptr, nullptr, static_cast<num_node*>(tree)->get_data()));
    else if (typeid(*tree) == typeid(id_node))
        return static_cast<tree_node*>(new id_node(nullptr, nullptr, static_cast<id_node*>(tree)->get_name()));
    else if (typeid(*tree) == typeid(operator_node))
        return static_cast<tree_node*>\
        (new operator_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()), static_cast<operator_node*>(tree)->get_type()));
    return nullptr;
}

interpretator::interpretator(tree_node *tree) :
ht_     (nullptr),
answer_ (0)
{
    assert(tree);
    tree_ = copy_tree(tree);
    assert(tree_);
}

interpretator::~interpretator()
{
    if (ht_)
        delete ht_;
    delete tree_;
}
