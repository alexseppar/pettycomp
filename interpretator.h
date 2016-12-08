#ifndef _INTERPRET__
#define _INTERPRET__

#include "tree_node.h"
#include "hash_table.h"


class interpretator final {
    tree_node   *tree_;
    hash_table  *ht_;
    double      answer_;
    public:
    interpretator(tree_node *tree);
    ~interpretator();
    bool    compute();
    double  get_answer() const;
};

#endif
