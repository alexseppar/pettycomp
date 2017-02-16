#include "hash_table.h"
#include <cstring>
#include <cstdio>

list_mem::list_mem(const char *name, double data, list_mem *next) :
    name_   (name),
    data_   (data),
    next_   (next)
    {}

list_mem::list_mem(list_mem &mem) :
    name_   (mem.get_name()),
    data_   (mem.get_data()),
    next_   (nullptr)
    {}

list_mem::~list_mem() {}

const char* list_mem::get_name() const {
        return name_;
    }
double list_mem::get_data() const {
        return data_;
    }
list_mem* list_mem::get_next() {
        return next_;
    }
void list_mem::set_next(list_mem *next) {
        next_ = next;
    }
void list_mem::set_data(double data) {
        data_ = data;
    }

unsigned hash_table::hash(const char *str) const
    {
        unsigned hash = 0;
        for (; *str; str++)
        {
            hash += (unsigned char)(*str);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }

hash_table::hash_table(unsigned size) :
    size_   (size)
    {
        table_ = new list_mem*[size_]();
    }

hash_table::hash_table(hash_table &ht) :
    size_   (ht.size_)
    {
        table_ = new list_mem*[size_]();
        list_mem *tmp, *tmp_1;
        for (unsigned i = 0; i < size_; ++i)
        {
            if (ht.table_[i])
            {
                tmp = ht.table_[i];
                table_[i] = new list_mem(*tmp);
                tmp_1 = table_[i];
                while (tmp->get_next())
                {
                    tmp_1->set_next(new list_mem(*(tmp->get_next())));
                    tmp_1 = tmp_1->get_next();
                    tmp = tmp->get_next();
                }
            }
            else
                continue;
        }
    }

hash_table::~hash_table()
    {
        for (unsigned long i = 0; i < size_; ++i)
        {
            list_mem *tmp, *tmp2;
            if ((tmp = table_[i]) != nullptr)
            {
                while (tmp)
                {
                    tmp2 = tmp;
                    tmp = tmp->get_next();
                    delete tmp2;
                }
            }
        }
        delete[] table_;
    }
list_mem* hash_table::find_mem(const char *name) const
    {
        list_mem *tmp = table_[hash(name) % size_];
        while (tmp)
        {
            if (strcmp(name, tmp->get_name()) == 0)
                return tmp;
            else
                tmp = tmp->get_next();
        }
        return nullptr;
    }
void hash_table::add(const char *name, double data)
    {
        if (table_[hash(name) % size_] == nullptr)
            table_[hash(name) % size_] = new list_mem(name, data);
        else
        {
            list_mem *tmp;
            if ((tmp = find_mem(name)) == nullptr)
            {
                while(tmp->get_next())
                    tmp = tmp->get_next();
                tmp->set_next(new list_mem(name, data));
            }
            else
                tmp->set_data(data);
        }
    }

void hash_table::print_info() const
{
    unsigned count = 0;
    bool     not_empty = false;
    while (count < size_)
    {
        if (table_[count] == nullptr)
            ++count;
        else
        {
            not_empty = true;
            fprintf(stdout, "\t%s\t=\t%lg \n", table_[count]->get_name(), table_[count]->get_data());
            ++count;
        }
    }
    if (not_empty)
        fprintf(stdout, "\n");
    else
        fprintf(stdout, "no variables \n\n");
}
