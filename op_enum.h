#ifndef _ENUM__
#define _ENUM__

#define DEF_OP(name, num, code) \
    OP_##name = num,

enum OP {
    #include "cmd.h"
};

#undef DEF_OP

#endif

