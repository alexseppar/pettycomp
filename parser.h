#ifndef _PARSER__
#define _PARSER__

#include "tree_node.h"

#include "lexer.h"

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
    const char* get_cur_str         ()   const;
    public:
    explicit parser(const char* file_name);
    ~parser();
    bool build_tree();
    tree_node* get_tree() const;
};

#endif
