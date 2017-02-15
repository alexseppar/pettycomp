#include <cstdio>
#include <cassert>

const int NUM_TESTS = 10;

int main(int argc, char **argv)
{
    FILE *out = fopen(argv[1], "r");
    FILE *answ = fopen("answ.txt", "r");
    
    assert(out && answ);

    fseek(out, 0, SEEK_END);
    int out_size = ftell(out);
    fseek(out, 0, SEEK_SET);
    char *out_ = new char[out_size];
    fread(out_, sizeof(char), out_size, out);
    out_[out_size - 1] = '\0';

    fseek(answ, 0, SEEK_END);
    int answ_size = ftell(answ);
    fseek(answ, 0, SEEK_SET);
    char *answ_ = new char[answ_size];
    fread(answ_, sizeof(char), answ_size, answ);
    answ_[answ_size - 1] = '\0';
    
    char *tmp_out_ = out_;
    char *tmp_answ_ = answ_;

    while (*out_ && *answ_)
    {
        if (*out_ != *answ_)
        {
            fprintf(stderr, "test %s failed \n", argv[2]);
            char *tmp;
            while (*answ_)
            {
                if (*answ_ == '\n')
                    if (*(answ_ - 1) == '\n')
                    {
                        ++answ_;
                        break;
                    }
                ++answ_;
            }
            fclose(answ);
            answ = fopen("answ.txt", "w");
            fprintf(answ, "%s\n", answ_);
            fclose(answ);
            return 0;
        }
        ++out_;
        ++answ_;
    }
    #define delete_last_answ\
    while (*answ_)\
    {\
        if (*answ_ == '\n')\
            if (*(answ_ - 1) == '\n')\
            {\
                ++answ_;\
                break;\
            }\
        ++answ_;\
    }\
    fclose(answ);\
    if (*answ_)\
    {\
        answ = fopen("answ.txt", "w");\
        fprintf(answ, "%s\n", answ_);\
        fclose(answ);\
    }\
    return 0;

    if (*out_ == '\0' && *answ_ == '\n')
    {
        ++answ_;
        fprintf(stdout, "test %s succeed\n", argv[2]);
        delete_last_answ
    }
    else
    {
        fprintf(stderr, "test %s failed\n", argv[2]);
        delete_last_answ 
    }
    fclose (out);
    delete tmp_out_;
    delete tmp_answ_;
}
