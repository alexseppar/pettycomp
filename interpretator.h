#ifndef _INTERPRET__
#define _INTERPRET__

#include "tree_node.h"
#include "hash_table.h"

class scope final {
    unsigned    size_;
    unsigned    last_;
    hash_table  **tables_;
    public:
    scope(unsigned size);
    ~scope();
    void    add_like_prev();
    void    add_new();
    void    delete_last();
    unsigned    get_last() const;
    hash_table* operator[] (unsigned idx);
};
            

class interpretator final {
    tree_node   *tree_;
    double      answer_;
    public:
    interpretator(tree_node *tree);
    ~interpretator();
    bool    compute();
    double  get_answer() const;
};

#endif
