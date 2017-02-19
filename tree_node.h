#ifndef __TREE_NODE__
#define __TREE_NODE__

enum node_type {
    ID_NODE,
    OP_NODE,
    NUM_NODE,
    CONNECTION_NODE,
    EQ_NODE,
    IF_NODE,
    WHILE_NODE
};

class tree_node {
    protected:
    tree_node   *lhs_,
                *rhs_;
    public:
    tree_node(tree_node *lhs, tree_node *rhs);
    virtual     ~tree_node();
    tree_node*  get_lhs() const;
    tree_node*  get_rhs() const;
    void        clean_up();
    virtual node_type get_type() const = 0;
};

class num_node final : public tree_node {
    double data_;
    public:
    num_node(tree_node *lhs, tree_node *rhs, double data);
    ~num_node();
    double      get_data() const;
    node_type   get_type() const override;
};

class id_node final : public tree_node {
    const char  *name_;
    unsigned    line_,
                pos_;
    public:
    id_node(tree_node *lhs, tree_node *rhs, const char *name, unsigned line, unsigned pos);
    ~id_node();
    const char* get_name() const;
    void        set_rhs(tree_node *rhs);
    unsigned    get_line() const;
    unsigned    get_pos() const;
    node_type   get_type() const override;
};

class operator_node final : public tree_node {
    int type_;
    int priority_;
    public:
    operator_node(tree_node *lhs, tree_node *rhs, int type);
    ~operator_node();
    int get_operator_type() const;
    int get_priority() const;
    node_type   get_type() const override;
};

class eq_node final : public tree_node {
    public:
    eq_node(tree_node *lhs, tree_node *rhs);
    ~eq_node();
    node_type   get_type() const override;
};

class if_node final : public tree_node {
    public:
    if_node(tree_node *lhs, tree_node *rhs);
    ~if_node();
    node_type   get_type() const override;
};

class connection_node final : public tree_node {
    public:
    connection_node(tree_node *lhs, tree_node *rhs);
    ~connection_node();
    node_type   get_type() const override;
};

class while_node final : public tree_node {
    public:
    while_node(tree_node *lhs, tree_node *rhs);
    ~while_node();
    node_type get_type() const override;
};

#endif
