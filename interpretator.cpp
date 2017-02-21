#include "interpretator.h"
#include <typeinfo>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

class id_list final {
    const char  *name_;
    unsigned    line_,
                pos_;
    id_list     *next_;
    public:
    explicit id_list(const char *name, unsigned line, unsigned pos, id_list *next) :
    name_   (name),
    line_   (line),
    pos_    (pos),
    next_   (next)
    {}
    id_list():
    name_   (nullptr),
    line_   (0),
    pos_    (0),
    next_   (nullptr)
    {}
    unsigned get_pos() const { return pos_; }
    unsigned get_line() const { return line_; }
    const char* get_name() const { return name_; }
    id_list* get_next() const { return next_; }
    id_list* push(const char *name, unsigned line, unsigned pos)
    {
        return new id_list(name, line, pos, this);
    }
};

static id_list* get_id_list(tree_node *tree, id_list *list)
{
    assert(tree);
    assert(list);
    if (tree->get_lhs())
        list = get_id_list(tree->get_lhs(), list);
    if (tree->get_rhs())
        list = get_id_list(tree->get_rhs(), list);
    if (tree->get_type() == ID_NODE)
    {
        id_node *tmp = static_cast<id_node*>(tree);
        return list->push(tmp->get_name(), tmp->get_line(), tmp->get_pos());
    }
    else 
        return list;
}

static double calculate(tree_node *tree, hash_table &scope_, bool *err)
{
    assert(tree);
    assert(err);
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

        scope_.add(tmp_name, calculate(tree->get_rhs(), scope_, err));
        
        #ifdef DEBUG__
            fprintf(stderr, ";\nvarlist:\n");
            scope_.print_info();
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
        list_mem *tmp = scope_.find_mem(static_cast<id_node*>(tree)->get_name());
        #ifdef DEBUG__
        fprintf(stderr, "%s ", static_cast<id_node*>(tree)->get_name());
        #endif
        if (tmp == nullptr)
        {
            id_node *tmp_id = static_cast<id_node*>(tree);
            fprintf(stdout, "error\nline %u, pos %u: variable '%s' was not defined in this scope\n", tmp_id->get_line(),\
                                                                                    tmp_id->get_pos(), tmp_id->get_name());
            exit(1);
        }
        else
            return tmp->get_data();
    }
    else if (tree->get_type() == WHILE_NODE)
    {
        #ifdef DEBUG__
        fprintf(stderr, "while (");
        #endif
        if (tree->get_rhs()->get_rhs() == nullptr || static_cast<id_node*>(tree->get_rhs()->get_rhs())->get_name() == nullptr)
        {
            while (calculate(tree->get_lhs(), scope_, err))
            {   
                #ifdef DEBUG__
                fprintf(stderr, ")\n");
                #endif
                calculate(tree->get_rhs()->get_lhs(), scope_, err); 
                #ifdef DEBUG__
                fprintf(stderr, "while (");
                #endif
            }
            #ifdef DEBUG__
            fprintf(stderr, ")\n");
            #endif
            return 0;
        }
        else
        {
            id_list empty;
            id_list *LIST = get_id_list(tree->get_lhs(), &empty);
            hash_table new_scope_(100);
            id_list *tmp = LIST;
            while (tmp->get_next())
            {
                list_mem *cur_id = scope_.find_mem(tmp->get_name());
                if (cur_id == nullptr)
                {
                    fprintf(stdout, "error\nline %u, pos %u: variable '%s' was not defined in this scope\n", tmp->get_line(), \
                                                                                    tmp->get_pos(), tmp->get_name());
                    exit(1);
                }
                new_scope_.add(tmp->get_name(), cur_id->get_data());
                tmp = tmp->get_next();
            }
            id_node *tmp_1 = static_cast<id_node*>(tree->get_rhs()->get_rhs());
            while (tmp_1)
            {
                list_mem *cur_id = scope_.find_mem(tmp_1->get_name());
                if (cur_id == nullptr)
                {
                    fprintf(stdout, "error\nline %u, pos %u: variable '%s' was not defined in this scope\n", tmp_1->get_line(), \
                                                                                tmp_1->get_pos(), tmp_1->get_name());
                    exit(1);
                }
                new_scope_.add(tmp_1->get_name(), cur_id->get_data());
                tmp_1 = static_cast<id_node*>(tmp_1->get_rhs());
            }
            while (calculate(tree->get_lhs(), scope_, err))
            {
                #ifdef DEBUG__
                fprintf(stderr, ") capture (");
                tmp_1 = static_cast<id_node*>(tree->get_rhs()->get_rhs());
                while (tmp_1)
                {
                    fprintf(stderr, "%s", tmp_1->get_name());
                    tmp_1 = static_cast<id_node*>(tmp_1->get_rhs());
                    if (tmp_1)
                        fprintf(stderr, ", ");
                    else
                        fprintf(stderr, ")\n");
                }
                #endif
                calculate(tree->get_rhs()->get_lhs(), new_scope_, err);
                #ifdef DEBUG__
                fprintf(stderr, "while (");
                #endif
                tmp = LIST; 
                while (tmp->get_next())
                {
                    list_mem *cur_id = new_scope_.find_mem(tmp->get_name());
                    scope_.add(tmp->get_name(), cur_id->get_data());
                    tmp = tmp->get_next();
                } 
            }
            #ifdef DEBUG__
            fprintf(stderr, ") capture (");
            tmp_1 = static_cast<id_node*>(tree->get_rhs()->get_rhs());
            while (tmp_1)
            {
                fprintf(stderr, "%s", tmp_1->get_name());
                tmp_1 = static_cast<id_node*>(tmp_1->get_rhs());
                if (tmp_1)
                    fprintf(stderr, ", ");
                else
                    fprintf(stderr, ")\n");
            }
            #endif
            tmp_1 = static_cast<id_node*>(tree->get_rhs()->get_rhs());
            while (tmp_1)
            {
                list_mem *cur_id = new_scope_.find_mem(tmp_1->get_name()); 
                scope_.add(tmp_1->get_name(), cur_id->get_data());
                tmp_1 = static_cast<id_node*>(tmp_1->get_rhs());
            }
            return 0;
        }
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
            if (tmp && static_cast<id_node*>(tmp)->get_name() != nullptr)
            { 
                id_node* tmp_ = static_cast<id_node*>(tmp);
                hash_table new_scope_(100);
                #ifdef DEBUG__
                fprintf(stderr, "capture (");
                #endif
                while (tmp_)
                {
                    #ifdef DEBUG__
                    fprintf(stderr, "%s", tmp_->get_name());
                    #endif
                    list_mem *tmp_list_mem = scope_.find_mem(tmp_->get_name());
                    if (tmp_list_mem == nullptr)
                    {
                        fprintf(stdout, "error\nline %u, pos %u: variable '%s' was not defined in this scope (in capture block)\n",\
                                    tmp_->get_line(), tmp_->get_pos(), tmp_->get_name());
                        exit(1);
                    }
                    else
                    {
                        new_scope_.add(tmp_->get_name(), tmp_list_mem->get_data());
                        tmp_ = static_cast<id_node*>(tmp_->get_rhs());
                        #ifdef DEBUG__
                        if (tmp_)
                            fprintf(stderr, ", ");
                        else
                        {
                            fprintf(stderr, ")\n");
                            break;
                        }
                        #endif
                    }
                } 
                calculate(tree->get_rhs()->get_lhs(), new_scope_, err);
                const char *cur_name;
                while (tmp)
                {
                    cur_name = static_cast<id_node*>(tmp)->get_name();
                    scope_.find_mem(cur_name)->set_data(new_scope_.find_mem(cur_name)->get_data()); 
                    tmp = tmp->get_rhs();
                }
                return 0;
            }
            else
            {
                calculate(tree->get_rhs()->get_lhs(), scope_, err);
                return 0;
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
    hash_table scope_(100);
    calculate(tree_, scope_, &err);
    if (!err)
    {
        list_mem *tmp = scope_.find_mem("result");
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
        return new eq_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()));
    else if (tree->get_type() == NUM_NODE)
        return new num_node(nullptr, nullptr, static_cast<num_node*>(tree)->get_data());
    else if (tree->get_type() == ID_NODE)
    {
        id_node *tmp = static_cast<id_node*>(tree);
        return new id_node(nullptr, copy_tree(tree->get_rhs()), tmp->get_name(), tmp->get_line(), tmp->get_pos());
    }
    else if (tree->get_type() == WHILE_NODE)
        return new while_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()));
    else if (tree->get_type() == OP_NODE)
        return \
        new operator_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs()), static_cast<operator_node*>(tree)->get_operator_type());
    else if (tree->get_type() == IF_NODE)
        return new if_node(copy_tree(tree->get_lhs()), copy_tree(tree->get_rhs())); 
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
