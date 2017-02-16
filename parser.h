#ifndef _PARSER__
#define _PARSER__

#include "tree_node.h"

#include "op_enum.h"

enum lex_type {
    NUM_LEX,
    ID_LEX,
    OP_LEX,
    FULL_STOP,
    END_OF_EXPR,
    EQ_LEX,
    CLOSE_BRACKET,
    OPEN_BRACKET,
    IF_LEX,
    ENDIF_LEX,
    CAPTURE_LEX,
    COMMA_LEX
};


class lexem final {
    lex_type type_;
    unsigned line_;
    unsigned pos_;
    union {
        double      val_;
        const char  *name_;
            OP      op_type_;
    };
    public:
    lexem(lex_type type, unsigned line, unsigned pos);
    lex_type    get_type() const;
    const char* get_name() const;
    double      get_val() const;
    OP          get_operator_type() const;
    void        set_val(double val);
    void        set_op_type(OP op_type);
    void        set_name(const char* name);
    unsigned    get_pos() const;
    unsigned    get_line() const;
};

class lexer final {
    char        *info_;
    lexem       *cur_token_;
    unsigned    cur_line_,
                cur_position_,
                lex_count_;
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
