#include "parser.h"
#include "interpretator.h"
#include <cstdio>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid list of arguments \n", argv[0]);
        return -1;
    }
    else
    {
        parser parserok(argv[1]);
        if (!parserok.build_tree())
            return -1;
        interpretator intr(parserok.get_tree());
        if (intr.compute())
            printf("%lf \n", intr.get_answer());
        else
            return -1;
    }
    return 0;
}

