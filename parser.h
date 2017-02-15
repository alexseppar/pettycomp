#ifndef _PARSER__
#define _PARSER__

#include "tree_node.h"

class lexem {
    protected:
    unsigned line_;
    public:
    virtual ~lexem() {}
    virtual int         get_type() const = 0;
    unsigned            get_line() const;
};

class id_lexem final : public lexem {
    const char *name_;
    public:
    id_lexem(const char *name, unsigned line);
    id_lexem(const id_lexem& that);
    ~id_lexem();
    int         get_type() const override;
    const char* get_name() const;
};

class num_lexem final : public lexem {
    double data_;
    public:
    num_lexem(double data, unsigned line);
    num_lexem(const num_lexem& that);
    int	        get_type() const override;
    double 	    get_data() const;
};

class operator_lexem final : public lexem {
    int name_;
    public:
    operator_lexem(int name, unsigned line);
    operator_lexem(const operator_lexem& that);
    int         get_type() const override;
    int         get_operator_type() const;
};

class if_lexem final : public lexem {
    public:
    if_lexem(unsigned line);
    int         get_type() const override;
};

class endif_lexem final : public lexem {
    public:
    endif_lexem(unsigned line);
    int         get_type() const override;
};

class capture_lexem final : public lexem {
    public:
    capture_lexem(unsigned line);
    int         get_type() const override;
};

class equality_lexem final : public lexem {
    public:
    equality_lexem(unsigned line);
    int get_type() const override;
};

class comma_lexem final : public lexem {
    public:
    comma_lexem(unsigned line);
    int get_type() const override;
};

class open_bracket_lexem final : public lexem {
    public:
    open_bracket_lexem(unsigned line);
    int get_type() const override;
};

class close_bracket_lexem final : public lexem {
    public:
    close_bracket_lexem(unsigned line);
    int get_type() const override;
};

class end_of_expr_lexem final : public lexem {
    public:
    end_of_expr_lexem(unsigned line);
    int get_type() const override;
};

class full_stop_lexem final : public lexem {
    public:
    full_stop_lexem(unsigned line);
    int get_type() const override;
};

class lexer final {
    char        *info_;
    lexem       *cur_token_;
    unsigned    cur_line_,
                cur_position_;
    public:
    explicit lexer(const char *file_name);
    ~lexer();
    void    mov_to_next_token();
    lexem*  get_cur_token() const;
};

class parser final {
    lexer       lxr_;
    tree_node   *tree_;
    unsigned    last_line_;
    tree_node*  get_if              ();
    tree_node*  get_capture         ();
    tree_node*  get_trig            ();
    tree_node*  get_num             ();
    tree_node*  get_id              ();
    tree_node*  get_p               ();
    tree_node*  get_pow             ();
    tree_node*  get_term            ();
    tree_node*  get_expr            ();
    tree_node*  get_equality        ();
    tree_node*  get_equalities      (bool open_scope);
    lexem*      get_cur_token       () const;
    void        mov_to_next_token   ();
    public:
    explicit parser(const char* file_name);
    ~parser();
    bool build_tree();
    tree_node* get_tree() const;
};

#endif
