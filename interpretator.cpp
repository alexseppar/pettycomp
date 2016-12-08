#include "interpretator.h"
#include <typeinfo>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>

static double calculate(tree_node *tree, hash_table *ht)
{
    if (typeid(*tree) == typeid(tree_node))
    {
        calculate(tree->get_lhs(), ht);
        calculate(tree->get_rhs(), ht);
        return 0;
    }
    else if (typeid(*tree) == typeid(eq_node))
    {
        if (ht->find_mem(static_cast<id_node*>(tree->get_lhs())->get_name()))
            fprintf(stderr, "warning: redeclaration of '%s' \n", static_cast<id_node*>(tree->get_lhs())->get_name());
        ht->add(static_cast<id_node*>(tree->get_lhs())->get_name(), calculate(tree->get_rhs(), ht));
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
        return static_cast<num_node*>(tree)->get_data();
    else if (typeid(*tree) == typeid(id_node))
    {
        list_mem *tmp = ht->find_mem(static_cast<id_node*>(tree)->get_name());
        if (tmp == nullptr)
        {
            fprintf(stderr, "variable %s was used but not defined yet \n", static_cast<id_node*>(tree)->get_name());
            return -1;
        }
        else
            return tmp->get_data();
    }
    else
    {
        fprintf(stderr, "unknown error \n");
        return -1;
    }
}

bool interpretator::compute()
{
    ht_ = new hash_table(100);
    if (calculate(tree_, ht_) == 0)
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
