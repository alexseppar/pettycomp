
#ifdef DEBUG__

#define PRINT_INFO_(operation) do {\
bool open_bracket = false;\
if (typeid(*(tree->get_lhs())) == typeid(operator_node) &&\
    static_cast<operator_node*>(tree->get_lhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())\
    {\
        open_bracket = true;\
        fprintf(stdout, "(");\
    }\
double lhs = calculate(tree->get_lhs(), scope_, err);\
if (open_bracket)\
    fprintf(stdout, ") ");\
fprintf(stdout, "%s ", #operation);\
open_bracket = false;\
if (typeid(*(tree->get_rhs())) == typeid(operator_node) &&\
    static_cast<operator_node*>(tree->get_rhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())\
    {\
        open_bracket = true;\
        fprintf(stdout, "(");\
    }\
double rhs = calculate(tree->get_rhs(), scope_, err);\
if (open_bracket)\
    fprintf(stdout, ") ");\
    return lhs operation rhs;}\
while (0)


#endif

DEF_OP(sum, 43, { 
    #ifdef DEBUG__
        double lhs = calculate(tree->get_lhs(), scope_, err);
        fprintf(stdout, "+ ");
        double rhs = calculate(tree->get_rhs(), scope_, err);
        return lhs + rhs;
    #else
    return calculate(tree->get_lhs(), scope_, err) + calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(sub, 45, {
    #ifdef DEBUG__
        double lhs = calculate(tree->get_lhs(), scope_, err);
        fprintf(stdout, "- ");
        double rhs = calculate(tree->get_rhs(), scope_, err);
        return lhs - rhs; 
    #else
    return calculate(tree->get_lhs(), scope_, err) - calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(mul, 42, { 
    #ifdef DEBUG__
        PRINT_INFO_(*);
    #else
    return calculate(tree->get_lhs(), scope_, err) * calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(power, 94, { 
    #ifdef DEBUG__
    bool open_bracket = false;
    if (typeid(*(tree->get_lhs())) == typeid(operator_node) &&
    static_cast<operator_node*>(tree->get_lhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())
    {
        open_bracket = true;
        fprintf(stdout, "(");
    }
    double lhs = calculate(tree->get_lhs(), scope_, err);
    if (open_bracket)
        fprintf(stdout, ") ");
    fprintf(stdout, "^");
    open_bracket = false;
    if (typeid(*(tree->get_rhs())) == typeid(operator_node) &&
        static_cast<operator_node*>(tree->get_rhs())->get_priority() < static_cast<operator_node*>(tree)->get_priority())
    {
        open_bracket = true;
        fprintf(stdout, "(");
    }
    double rhs = calculate(tree->get_rhs(), scope_, err);
    if (open_bracket)
        fprintf(stdout, ") ");
    return pow(lhs,rhs);
    
    #else
    return pow(calculate(tree->get_lhs(), scope_, err), calculate(tree->get_rhs(), scope_, err));
    #endif
})
DEF_OP(divide, 47, { 
    #ifdef DEBUG__
    PRINT_INFO_(/);
    #else
    return calculate(tree->get_lhs(), scope_, err) / calculate(tree->get_rhs(), scope_, err);
    #endif
})
DEF_OP(sinus, 18, { 
    #ifdef DEBUG__
    fprintf(stdout, "sin(");
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stdout, ") ");
    return sin(lhs);
    #else
    return sin(calculate(tree->get_lhs(), scope_, err));
    #endif
})
DEF_OP(cosinus, 19, { 
    #ifdef DEBUG__
    fprintf(stdout, "cos(");
    double lhs = calculate(tree->get_lhs(), scope_, err);
    fprintf(stdout, ") ");
    return cos(lhs);
    #else
    return cos(calculate(tree->get_lhs(), scope_, err));
    #endif
})
DEF_OP(condition, 63, {
    if (calculate(tree->get_lhs(), scope_, err))
        return calculate(tree->get_rhs()->get_lhs(), scope_, err);
    else
        return calculate(tree->get_rhs()->get_rhs(), scope_, err);
})
DEF_OP(dots, 58, {})
DEF_OP(more, 62, {
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) > calculate(tree->get_rhs(), scope_, err));
})
DEF_OP(less, 60, {
    return static_cast<double>(calculate(tree->get_lhs(), scope_, err) < calculate(tree->get_rhs(), scope_, err));   
})
