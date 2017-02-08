#ifndef __TREE_NODE__
#define __TREE_NODE__

class tree_node {
    protected:
    tree_node   *lhs_,
                *rhs_;
    public:
    tree_node(tree_node *lhs, tree_node *rhs);
    virtual     ~tree_node();
    tree_node*  get_lhs() const;
    tree_node*  get_rhs() const;
};

class num_node : public tree_node {
    double data_;
    public:
    num_node(tree_node *lhs, tree_node *rhs, double data);
    ~num_node();
    double get_data() const;
    
};

class id_node : public tree_node {
    const char *name_;
    public:
    id_node(tree_node *lhs, tree_node *rhs, const char *name);
    ~id_node();
    const char* get_name() const;
};

class operator_node : public tree_node {
    int type_;
    int priority_;
    public:
    operator_node(tree_node *lhs, tree_node *rhs, int type);
    ~operator_node();
    int get_type() const;
    int get_priority() const;
};

class eq_node : public tree_node {
    public:
    eq_node(tree_node *lhs, tree_node *rhs);
    ~eq_node();
};

#endif
