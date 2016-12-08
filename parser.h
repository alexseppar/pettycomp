#ifndef _PARSER__
#define _PARSER__

#include "tree_node.h"

class lexem {
    public:
    virtual ~lexem() {}
    virtual int get_type() const = 0;
};

class id_lexem final : public lexem {
    const char *name_;
    public:
    id_lexem(const char *name);
    id_lexem(const id_lexem& that);
    ~id_lexem();
    int         get_type() const override;
    const char* get_name() const;
};

class num_lexem final : public lexem {
    double data_;
    public:
    num_lexem(double data);
    num_lexem(const num_lexem& that);
    int	        get_type() const override;
    double 	    get_data() const;
};

class operator_lexem final : public lexem {
    int name_;
    public:
    operator_lexem(int name);
    operator_lexem(const operator_lexem& that);
    int         get_type() const override;
    int         get_operator_type() const;
};

class equality_lexem final : public lexem {
    public:
    int get_type() const override;
};

class open_bracket_lexem final : public lexem {
    public:
    int get_type() const override;
};

class close_bracket_lexem final : public lexem {
    public:
    int get_type() const override;
};

class end_of_expr_lexem final : public lexem {
    public:
    int get_type() const override;
};

class full_stop_lexem final : public lexem {
    public:
    int get_type() const override;
};

class lexer final {
    lexem       **tokens_;
    char        *info_;
    unsigned    lex_count_,
                id_count_,
                cur_token_;
    public:
    explicit lexer(const char *file_name);
    bool build_tokens();
    ~lexer();
    void        mov_to_next_token();
    lexem*      get_cur_token() const;
    unsigned    get_num_id() const;
};

class parser final {
    tree_node   *tree_;
    lexer       *lxr_;
    tree_node* get_trig         ();
    tree_node* get_num          ();
    tree_node* get_id           ();
    tree_node* get_p            ();
    tree_node* get_pow          ();
    tree_node* get_term         ();
    tree_node* get_expr         ();
    tree_node* get_equality     ();
    tree_node* get_equalities   ();
    lexem*     get_cur_token    () const;
    void       mov_to_next_token();
    public:
    explicit parser(const char* file_name);
    ~parser();
    bool build_tree();
    tree_node* get_tree() const;
};

#endif
