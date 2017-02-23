
#ifdef DEBUG__

#define PRINT_INFO_(operation) do {\
bool open_bracket = false;\
if (tree->get_lhs()->get_type() == OP_NODE &&\
    static_cast<operator_node*>(tree->get_lhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())\
    {\
        open_bracket = true;\
        fprintf(stderr, "(");\
    }\
double lhs = calculate(tree->get_lhs(), scope_, err);\
if (open_bracket)\
    fprintf(stderr, ") ");\
fprintf(stderr, "%s ", #operation);\
open_bracket = false;\
if (tree->get_lhs()->get_type() == OP_NODE &&\
    static_cast<operator_node*>(tree->get_rhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())\
    {\
        open_bracket = true;\
        fprintf(stderr, "(");\
    }\
double rhs = calculate(tree->get_rhs(), scope_, err);\
if (open_bracket)\
    fprintf(stderr, ") ");\
    return lhs operation rhs;}\
while (0)

#define OP_EQ(operation) do {\
    id_node *cur_node = static_cast<id_node*>(tree->get_lhs());\
    list_mem *tmp = scope_.find_mem(cur_node->get_name());\
    if (tmp != nullptr)\
    {\
        fprintf(stderr, "%s ", #operation);\
        double res = calculate(tree->get_rhs(), scope_, err);\
        scope_.add(cur_node->get_name(), tmp->get_data() operation res);\
        return 0;\
    }\
    else\
    {\
        fprintf(stdout, "error\nline %u, pos %u: variable '%s' has to be declared\n", cur_node->get_line(), cur_node->get_pos(), \
                                                                                        cur_node->get_name());\
        exit(1);\
    }\
    } while (0)
#else 
#define OP_EQ(operation) do {\
    id_node *cur_node = static_cast<id_node*>(tree->get_lhs());\
    list_mem *tmp = scope_.find_mem(cur_node->get_name());\
    if (tmp != nullptr)\
    {\
        scope_.add(cur_node->get_name(), tmp->get_data() operation calculate(tree->get_rhs(), scope_, err));\
        return 0;\
    }\
    else\
    {\
        fprintf(stdout, "error\nline %u, pos %u: variable '%s' has to be declared\n", cur_node->get_line(), cur_node->get_pos(), \
                                                                                        cur_node->get_name());\
        exit(1);\
    }\
    } while (0)
#endif


#define SIN 18
#define COS 19
#define LEQ 20
#define MEQ 21
#define ADDEQ 22
#define SUBEQ 23
#define DIVEQ 24
#define MULEQ 25

DEF_OP(sum, '+', { 
    #ifdef DEBUG__
        double lhs = calculate(tree->get_lhs(), scope_, err);
        fprintf(stderr, "+ ");
        double rhs = calculate(tree->get_rhs(), scope_, err);
        return lhs + rhs;
    #else
    return calculate(tree->get_lhs(), scope_, err) + calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(sub, '-', {
    #ifdef DEBUG__
        double lhs = calculate(tree->get_lhs(), scope_, err);
        fprintf(stderr, "- ");
        double rhs = calculate(tree->get_rhs(), scope_, err);
        return lhs - rhs; 
    #else
    return calculate(tree->get_lhs(), scope_, err) - calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(mul, '*', { 
    #ifdef DEBUG__
        PRINT_INFO_(*);
    #else
    return calculate(tree->get_lhs(), scope_, err) * calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(power, '^', { 
    #ifdef DEBUG__
    bool open_bracket = false;
    if (tree->get_lhs()->get_type() == OP_NODE &&
    static_cast<operator_node*>(tree->get_lhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())
    {
        open_bracket = true;
        fprintf(stderr, "(");
    }
    double lhs = calculate(tree->get_lhs(), scope_, err);
    if (open_bracket)
        fprintf(stderr, ") ");
    fprintf(stderr, "^");
    open_bracket = false;
    if (tree->get_rhs()->get_type() == OP_NODE &&
        static_cast<operator_node*>(tree->get_rhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())
    {
        open_bracket = true;
        fprintf(stderr, "(");
    }
    double rhs = calculate(tree->get_rhs(), scope_, err);
    if (open_bracket)
        fprintf(stderr, ") ");
    return pow(lhs,rhs);
    
    #else
    return pow(calculate(tree->get_lhs(), scope_, err), calculate(tree->get_rhs(), scope_, err));
    #endif
})
DEF_OP(divide, '/', { 
    #ifdef DEBUG__
    PRINT_INFO_(/);
    #else
    return calculate(tree->get_lhs(), scope_, err) / calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(sinus, SIN, { 
    #ifdef DEBUG__
    fprintf(stderr, "sin(");
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, ") ");
    return sin(lhs);
    #else
    return sin(calculate(tree->get_lhs(), scope_, err));
    #endif
})
DEF_OP(cosinus, COS, { 
    #ifdef DEBUG__
    fprintf(stderr, "cos(");
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, ") ");
    return cos(lhs);
    #else
    return cos(calculate(tree->get_lhs(), scope_, err));
    #endif
})
DEF_OP(condition, '?', {
    #ifdef DEBUG__
    double EXPR_1 = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, "? ");
    double EXPR_2 = calculate(tree->get_rhs()->get_lhs(), scope_, err);
    fprintf(stderr, ": ");
    double EXPR_3 = calculate(tree->get_rhs()->get_rhs(), scope_, err);
    if (EXPR_1)
        return EXPR_2;
    else
        return EXPR_3;
    #else
    if (calculate(tree->get_lhs(), scope_, err))
        return calculate(tree->get_rhs()->get_lhs(), scope_, err);
    else
        return calculate(tree->get_rhs()->get_rhs(), scope_, err);
    #endif
})
DEF_OP(more, '>', {
    #ifdef DEBUG__
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, "> ");
    double rhs = calculate(tree->get_rhs(), scope_, err);
    return static_cast<double>(lhs > rhs); 
    #else
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) > calculate(tree->get_rhs(), scope_, err));
    #endif
})
DEF_OP(less, '<', {
    #ifdef DEBUG__
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, "< ");
    double rhs = calculate(tree->get_rhs(), scope_, err);
    return static_cast<double>(lhs < rhs); 
    #else
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) < calculate(tree->get_rhs(), scope_, err));   
    #endif
})
DEF_OP(lesseq, LEQ, {
    #ifdef DEBUG__
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, "<= ");
    double rhs = calculate(tree->get_rhs(), scope_, err);
    return static_cast<double>(lhs <= rhs); 
    #else
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) <= calculate(tree->get_rhs(), scope_, err));
    #endif
})
DEF_OP(moreeq, MEQ, {
    #ifdef DEBUG__
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stderr, ">= ");
    double rhs = calculate(tree->get_rhs(), scope_, err);
    return static_cast<double>(lhs >= rhs); 
    #else
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) >= calculate(tree->get_rhs(), scope_, err));
    #endif
})
DEF_OP(addeq, ADDEQ, {
    OP_EQ(+);
})
DEF_OP(subeq, SUBEQ, {
    OP_EQ(-); 
})
DEF_OP(diveq, DIVEQ, {
    OP_EQ(/);
})
DEF_OP(muleq, MULEQ, {
    OP_EQ(*);
})

#undef SIN
#undef LEQ
#undef COS
#undef MEQ
#undef ADDEQ
#undef SUBEQ
#undef MULEQ
#undef DIVEQ
