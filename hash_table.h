#ifndef __HASH__
#define __HASH__

class list_mem final {
    const char  *name_;
    double      data_;
    list_mem    *next_;
    public:
    explicit list_mem(const char *name, double data, list_mem *next = nullptr);
    list_mem(list_mem &mem);
    ~list_mem();
    const char* get_name    () const;
    double 	    get_data    () const;
    list_mem* 	get_next    ();
    void 	    set_next    (list_mem *next);
    void        set_data    (double data);
};

class hash_table final {
    unsigned        size_;
    list_mem        **table_;
    unsigned hash(const char *str) const;
    public:
    explicit    hash_table(unsigned size);
    hash_table(hash_table &ht);
    ~hash_table();
    list_mem* 	find_mem(const char *name) const;
    void 	    add	(const char *name, double data);
    void        print_info() const;
    unsigned    get_size() const;
};

#endif
